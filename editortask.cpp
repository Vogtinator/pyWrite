#include "editortask.h"

#include <cstdio>
#include <libndls.h>
#include <nucleus.h>

#include "font.h"
#include "texturetools.h"
#include "browsertask.h"
#include "cursortask.h"
#include "dialogtask.h"
#include "settingstask.h"

EditorTask editor_task;

extern "C" char nio_ascii_get(int *a);

constexpr char KEY_UP = 0xF0, KEY_DOWN = 0xF1, KEY_LEFT = 0xF2, KEY_RIGHT = 0xF3;

EditorTask::EditorTask()
{
    float menuitem_width = 0;
    for(auto &item : menu_items)
    {
        item.width = fontWidth(item.name, EFont::Normal);
        menuitem_width += item.width;
    }

    float padding = menu_width / menuitem_width;
    for(auto &item : menu_items)
        item.width = padding * item.width;

    //TODO: Change dynamically
    highlighter = &python_highlighter;
}

void EditorTask::makeCurrent()
{
    Task::makeCurrent();

    //Did we come back from the file browser?
    if(requested_open)
    {
        requested_open = false;

        //User canceled
        if(browser_task.filepath == "")
            return;

        //Load file
        filepath = browser_task.filepath;
        FILE *f = fopen(browser_task.filepath.c_str(), "r");
        if(f)
        {
            fseek(f, 0, SEEK_END);
            buffer.resize(ftell(f));
            fseek(f, 0, SEEK_SET);
            fread(reinterpret_cast<void*>(const_cast<char*>(buffer.data())), 1, buffer.size(), f);
            fclose(f);

            //Reset cursor and view
            cursor_pos = x_offset = line_top = 0;
        }
        else
            dialog_task.showMessage("Could not open '" + filepath.substr(browser_task.filepath.find_last_of('/') + 1, std::string::npos) + "'!");
    }
    else if(requested_save)
    {
        requested_save = false;

        //User canceled
        if(browser_task.filepath == "")
            return;

        filepath = browser_task.filepath;

        FILE *f = fopen(filepath.c_str(), "r");
        if(f)
        {
            fclose(f);

            if(!requested_confirm)
            {
                dialog_task.confirm("Overwrite existing file '" + filepath.substr(browser_task.filepath.find_last_of('/') + 1, std::string::npos) + "'?");

                requested_save = true;
                requested_confirm = true;
                return;
            }
            else if(!dialog_task.confirmed)
                return;
        }

        f = fopen(filepath.c_str(), "wb");
        if(f)
        {
            fwrite(buffer.c_str(), buffer.length(), 1, f);
            fclose(f);
        }
        else
            dialog_task.showMessage("Could not save '" + browser_task.filepath.substr(browser_task.filepath.find_last_of('/') + 1, std::string::npos) + "'!");
    }
}

void EditorTask::logic()
{
    int i;
    char c = nio_ascii_get(&i);
    if(isKeyPressed(KEY_NSPIRE_UP))
        c = KEY_UP;
    else if(isKeyPressed(KEY_NSPIRE_DOWN))
        c = KEY_DOWN;
    else if(isKeyPressed(KEY_NSPIRE_LEFT))
        c = KEY_LEFT;
    else if(isKeyPressed(KEY_NSPIRE_RIGHT))
        c = KEY_RIGHT;

    if(isKeyPressed(KEY_NSPIRE_ESC) && !key_hold_down) //Quit
    {
        //TODO: confirm close
        running = false;

        key_hold_down = true;
    }
    else if(c > 1)
    {
        const char *current_sel;
        int x = 0;
        unsigned int cursor_line, cursor_pos_new;

        //Key repeat handling
        if(key_hold_down)
        {
            key_repeat++;
            if(key_repeat >= key_repeat_count)
                key_repeat = key_repeat_count - key_repeat_speed;
            else
                goto skip_input;
        }

        switch(c)
        {
        case KEY_UP:
        case KEY_DOWN:
            //Get line the cursor is on
            cursor_line = linesUntil(buffer.c_str() + cursor_pos);

            //Cancel if there is nothing to move the cursor to
            if(cursor_line == 0 && c == KEY_UP)
                break;

            //Find beginning of line
            current_sel = atLine(cursor_line);

            //Get x position of selection relative to start of line
            while(static_cast<unsigned int>(current_sel - buffer.c_str()) < cursor_pos)
                x += fontWidth(*current_sel++);

            //Get new line and find char at x position
            current_sel = atLine(c == KEY_UP ? cursor_line - 1 : cursor_line + 1);
            while(*current_sel && *current_sel != '\n' && x > 0)
                x -= fontWidth(*current_sel++);

            cursor_line = linesUntil(current_sel);
            cursor_pos_new = current_sel - buffer.c_str();

            //Selection
            if(isKeyPressed(KEY_NSPIRE_SHIFT))
                changeSelection(cursor_pos_new);
            else //Abort selection
                sel_start = sel_end = cursor_pos_new;

            cursor_pos = cursor_pos_new;

            updateCursor();
            break;

        case KEY_RIGHT:
            //Horizontal scrolling
            if(isKeyPressed(KEY_NSPIRE_CTRL))
            {
                //Check for overflow. Or is wrapping a feature?
                if(x_offset + SCREEN_WIDTH/2 > x_offset)
                    x_offset += SCREEN_WIDTH/2;
                break;
            }

            if(cursor_pos < buffer.length())
            {
                //Hold shift to move the selection
                if(isKeyPressed(KEY_NSPIRE_SHIFT))
                    changeSelection(cursor_pos + 1);
                else
                    sel_start = sel_end;

                ++cursor_pos;
                updateCursor();
            }
            break;
        case KEY_LEFT:
            //Horizontal scrolling
            if(isKeyPressed(KEY_NSPIRE_CTRL))
            {
                if(x_offset >= SCREEN_WIDTH/2)
                    x_offset -= SCREEN_WIDTH/2;
                break;
            }

            if(cursor_pos > 0)
            {
                //Hold shift to move the selection
                if(isKeyPressed(KEY_NSPIRE_SHIFT))
                    changeSelection(cursor_pos - 1);
                else
                    sel_start = sel_end;

                --cursor_pos;
                updateCursor();
            }
            break;

        case '\b':
        default:
            //If something is selected
            if(sel_start != sel_end)
            {
                buffer.erase(buffer.begin() + sel_start, buffer.begin() + sel_end);
                cursor_pos = sel_end = sel_start;
            }
            else if(c == '\b')
            {
                if(cursor_pos > 0)
                {
                    buffer.erase(buffer.begin() + (cursor_pos - 1));
                    --cursor_pos;
                }
            }

            if(c != '\b')
            {
                buffer.insert(buffer.begin() + cursor_pos, c);
                ++cursor_pos;
            }

            updateCursor();
            break;
        }

        skip_input:

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_CTRL)) //This comes after (c > 1) as the ctrl key is used in combination with e.g. EE to produce '@'.
    {
        if(key_hold_down)
            key_hold_down = isKeyPressed(KEY_NSPIRE_S) || isKeyPressed(KEY_NSPIRE_C) || isKeyPressed(KEY_NSPIRE_V) || isKeyPressed(KEY_NSPIRE_X);

        else if(isKeyPressed(KEY_NSPIRE_S)) //Settings
        {
            key_hold_down = true;
            settings_task.makeCurrent();
            return;
        }
        else if(isKeyPressed(KEY_NSPIRE_C) || isKeyPressed(KEY_NSPIRE_X)) //Copy or Cut
        {
            key_hold_down = true;
            if(sel_start != sel_end)
            {
                cutbuffer.resize(sel_end - sel_start);
                std::copy(buffer.begin() + sel_start, buffer.begin() + sel_end, cutbuffer.begin());

                if(isKeyPressed(KEY_NSPIRE_X))
                {
                    buffer.erase(buffer.begin() + sel_start, buffer.begin() + sel_end);
                    cursor_pos = sel_end = sel_start;
                }
            }
        }
        else if(isKeyPressed(KEY_NSPIRE_V)) //Insert
        {
            key_hold_down = true;
            if(sel_start != sel_end)
            {
                buffer.erase(buffer.begin() + sel_start, buffer.begin() + sel_end);
                cursor_pos = sel_start;
            }

            buffer.insert(cursor_pos, cutbuffer);
            cursor_pos = sel_end = sel_start = cursor_pos + cutbuffer.length();
        }
    }
    else
    {
        key_hold_down = false;
        key_repeat = 0;
    }

    //Handle menu
    if(cursor_task.state && cursor_task.y < menu_height && !key_hold_down)
    {
        unsigned int x = cursor_task.x - menu_x;
        for(auto &item : menu_items)
        {
            //If cursor on the current menu item
            if(x < item.width)
            {
                (this->*(item.action))();
                break;
            }

            x -= item.width;
        }

        key_hold_down = true;
    }

    //Cursor blinking
    if(cursor_tick == 2 * cursor_time)
        cursor_tick = 0;
    else
        ++cursor_tick;
}

void EditorTask::render()
{
    //Draw menu
    drawRectangle(*screen, menu_x, 0, menu_width, menu_height, 0x0000);
    unsigned int item_x = menu_x;
    for(auto &item : menu_items)
    {
        if(cursor_task.y < menu_height && cursor_task.x > item_x && cursor_task.x < item_x + item.width)
            drawStringCenter(item.name, 0x0000, *screen, item_x + item.width/2, 0, EFont::Large);
        else
            drawStringCenter(item.name, 0x0000, *screen, item_x + item.width/2, 4, EFont::Normal);

        item_x += item.width;
    }

    const char *ptr = atLine(line_top);
    int x = 4 - x_offset;
    unsigned int y = menu_height + 2, pos = ptr - buffer.c_str(), highlight_length = 0;
    const int start_x = x;
    const unsigned int font_height = fontHeight();
    COLOR font_color = 0x0000;

    while(y + font_height < SCREEN_HEIGHT)
    {
        //Draw '|' as cursor
        if(pos == cursor_pos && cursor_tick > cursor_time && x >= 3)
            drawChar('|', 0x0000, *screen, x - 3, y);

        //If mouse clicked and on current char change cursor position
        if(cursor_task.state && cursor_task.y >= y && cursor_task.y < y + font_height
                && static_cast<int>(cursor_task.x) >= x && static_cast<int>(cursor_task.x) < static_cast<int>(x + fontWidth(*ptr)))
        {
            unsigned int cursor_pos_new = ptr - buffer.c_str();

            //Show cursor
            cursor_tick = cursor_time;

            if(isKeyPressed(KEY_NSPIRE_SHIFT))
                changeSelection(cursor_pos_new);
            else
                sel_start = sel_end = cursor_pos_new;

            cursor_pos = cursor_pos_new;

            updateCursor();
        }

        if(!*ptr)
            break;

        if(highlighter)
        {
            if(highlight_length == 0 && !isspace(*ptr))
                font_color = highlighter->highlight(ptr, highlight_length);

            if(!isspace(*ptr))
                --highlight_length;
        }

        if(*ptr == '\n')
        {
            x = start_x;
            y += font_height;
        }
        else if(*ptr && x + fontWidth(*ptr) < SCREEN_WIDTH && x >= 0)
        {
            //Check whether in selection
            if(pos >= sel_start && pos < sel_end)
                x += drawChar(*ptr, 0xFFFF, 0x003F, *screen, x, y); //White chars on blue background
            else
                x += drawChar(*ptr, font_color, *screen, x, y);
        }
        else if(*ptr)
            x += fontWidth(*ptr);

        ++ptr;
        ++pos;
    }
}

void EditorTask::menuNew()
{
    filepath = "";
    buffer = "";

    //Reset cursor and view
    cursor_pos = x_offset = line_top = 0;
}

void EditorTask::menuOpen()
{
    requested_open = true;
    browser_task.requestOpen();
}

void EditorTask::menuSave()
{
    if(filepath == "")
    {
        menuSaveAs();
        return;
    }

    FILE *f = fopen(filepath.c_str(), "wb");
    if(f)
    {
        fwrite(buffer.c_str(), buffer.length(), 1, f);
        fclose(f);
    }
    else
        dialog_task.showMessage("Could not save '" + browser_task.filepath.substr(browser_task.filepath.find_last_of('/') + 1, std::string::npos) + "'!");
}

void EditorTask::menuSaveAs()
{
    requested_save = !(requested_confirm = false);
    browser_task.requestSave();
}

void EditorTask::menuRun()
{
    if(filepath == "")
    {
        FILE *f = fopen("/documents/tmp.py", "wb");
        if(f && fwrite(buffer.c_str(), buffer.length(), 1, f))
            runFile("/documents/tmp.py");
        else
            dialog_task.showMessage("Could not save 'tmp.py'!");

        if(f)
        {
            fclose(f);
            remove("/documents/tmp.py");
        }
    }
    else
        runFile(filepath.c_str());
}

void EditorTask::menuExit()
{
    running = false;
}

const char *EditorTask::atLine(unsigned int l)
{
    const char *ptr = buffer.c_str();
    while(l--)
        while(*ptr && *ptr++ != '\n');

    return ptr;
}

unsigned int EditorTask::linesUntil(const char *end)
{
    unsigned int ret = 0;
    const char *ptr = buffer.c_str();
    while(ptr != end && *ptr)
        if(*ptr++ == '\n')
            ++ret;

    return ret;
}

//Update horizontal and vertical scroll position according to cursor position
void EditorTask::updateCursor()
{
    int cursor_x = 0;
    unsigned int cursor_line = linesUntil(buffer.c_str() + cursor_pos);
    const char *ptr = atLine(cursor_line);
    while(ptr < buffer.c_str() + cursor_pos)
        cursor_x += fontWidth(*ptr++);

    cursor_x -= x_offset;

    while(cursor_x >= static_cast<int>(SCREEN_WIDTH - fontWidth('M')))
    {
        cursor_x -= SCREEN_WIDTH/2;
        x_offset += SCREEN_WIDTH/2;
    }

    while(cursor_x <= 3 && x_offset >= SCREEN_WIDTH/2)
    {
        cursor_x += SCREEN_WIDTH/2;
        x_offset -= SCREEN_WIDTH/2;
    }

    if(cursor_line < line_top)
        --line_top;
    else if(cursor_line >= line_top + (240 - 4 - fontHeight()) / fontHeight())
        ++line_top;

    if(sel_start > sel_end)
        std::swap(sel_start, sel_end);

    //Show cursor
    cursor_tick = cursor_time;
}

void EditorTask::changeSelection(unsigned int cursor_pos_new)
{
    if(cursor_pos == sel_end)
        sel_end = cursor_pos_new;
    else if(cursor_pos == sel_start)
        sel_start = cursor_pos_new;
    else if(cursor_pos_new <= cursor_pos)
    {
        sel_start = cursor_pos_new;
        sel_end = cursor_pos;
    }
    else
    {
        sel_end = cursor_pos_new;
        sel_start = cursor_pos;
    }
}

void EditorTask::runFile(const char *file)
{
    const char* argv[] = {file};
    key_hold_down = true;

    nl_exec(settings_task.mpython_path.content().c_str(), sizeof(argv), const_cast<char**>(argv));
}
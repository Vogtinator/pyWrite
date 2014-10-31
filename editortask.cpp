#include "editortask.h"

#include <cstdio>
#include <libndls.h>
#include <nucleus.h>

#include "font.h"
#include "texturetools.h"
#include "browsertask.h"
#include "cursortask.h"

#include "textures/menu.h"

EditorTask editor_task;

extern "C" char nio_ascii_get(int *a);

constexpr char KEY_UP = 0xF0, KEY_DOWN = 0xF1, KEY_LEFT = 0xF2, KEY_RIGHT = 0xF3;

void EditorTask::makeCurrent()
{
    //Did we come back from the file browser?
    if(requested_open && browser_task.filepath != "")
    {
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
            sel_start = sel_line = line_offset = line_top = 0;
        }
    }

    Task::makeCurrent();
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

    if(isKeyPressed(KEY_NSPIRE_ESC) && !key_hold_down)
    {
        running = false;

        key_hold_down = true;
    }
    else if(c > 1)
    {
        const char *current_sel; int x = 0, lines = (240 - 4 - fontHeight()) / fontHeight();

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
            if(sel_line == 0)
                break;

        case KEY_DOWN:
            //Find beginning of line
            current_sel = atLine(sel_line) + line_offset;

            //Get x position of selection
            while(static_cast<unsigned int>(current_sel - buffer.c_str()) < sel_start)
                x += fontWidth(*current_sel++);

            //Get new line and find char at X position
            current_sel = atLine(c == KEY_UP ? sel_line - 1 : sel_line + 1);
            while(*current_sel && *current_sel != '\n' && x > 0)
                x -= fontWidth(*current_sel++);

            line_offset = 0;
            sel_line = linesUntil(current_sel);
            sel_start = current_sel - buffer.c_str();

            if(sel_line < line_top)
                --line_top;
            else if(sel_line >= line_top + lines)
                ++line_top;

            //Show cursor
            cursor_tick = cursor_time;
            break;
        case KEY_RIGHT:
            if(sel_start < buffer.length())
            {
                if(*(buffer.c_str() + sel_start) == '\n')
                    ++sel_line;

                ++sel_start;
            }
            break;
        case KEY_LEFT:
            if(sel_start > 0)
            {
                --sel_start;

                if(*(buffer.c_str() + sel_start) == '\n')
                    --sel_line;
            }
            break;

        case '\b':
            if(sel_start > 0)
            {
                buffer.erase(buffer.begin() + (sel_start - 1));
                --sel_start;
            }
            break;

        case '\n':
            buffer.insert(buffer.begin() + sel_start, c);
            ++sel_start;
            line_offset = 0;
            sel_line = linesUntil(buffer.c_str() + sel_start);
            break;

        default:
            buffer.insert(buffer.begin() + sel_start, c);
            ++sel_start;
            break;
        }

        skip_input:

        key_hold_down = true;
    }
    else
    {
        key_hold_down = false;
        key_repeat = 0;
    }

    //Handle menu
    if(cursor_task.state && cursor_task.y < menu.height)
    {
        if(cursor_task.x > 15 && cursor_task.x < 44)
        {
            //Open
            requested_open = true;
            browser_task.requestOpen();
        }
        else if(cursor_task.x > 50 && cursor_task.x < 80)
        {
            //Save
            FILE *f = fopen(filepath.c_str(), "wb");
            fwrite(buffer.c_str(), buffer.length(), 1, f);
            fclose(f);
        }
        else if(cursor_task.x > 85 && cursor_task.x < 150)
        {
            //Save&Run
            FILE *f = fopen(filepath.c_str(), "wb");
            fwrite(buffer.c_str(), buffer.length(), 1, f);
            fclose(f);

            const char* argv[] = {filepath.c_str()};
            key_hold_down = true;
            nl_exec("/documents/ndless/micropython.tns", sizeof(argv), const_cast<char**>(argv));
        }
        else if(cursor_task.x > 155 && cursor_task.x < 176)
        {
            //Exit
            running = false;
        }
    }

    if(cursor_tick == 2 * cursor_time)
        cursor_tick = 0;
    else
        ++cursor_tick;
}

void EditorTask::render()
{
    drawTexture(menu, *screen, 0, 0, menu.width, menu.height, 0,0, menu.width, menu.height);

    const char *ptr = atLine(line_top);
    if(line_top == sel_line)
        ptr += line_offset;

    unsigned int x = 4, y = menu.height + 4;
    unsigned int line = line_top;
    const unsigned int start_x = x;
    const unsigned int font_height = fontHeight();
    while(y + font_height < SCREEN_HEIGHT)
    {
        //Draw '|' as cursor
        if((unsigned int)(ptr - buffer.c_str()) == sel_start && cursor_tick > cursor_time)
            drawChar('|', 0x0000, *screen, x - 3, y);

        //If mouse clicked and on current char set selection
        if(cursor_task.state && cursor_task.y >= y && cursor_task.y < y + font_height
                && cursor_task.x >= x && cursor_task.x < x + fontWidth(*ptr))
        {
            sel_start = ptr - buffer.c_str();
            sel_line = linesUntil(ptr);
            line_offset = 0;
        }

        if(!*ptr)
            break;

        if(*ptr == '\n')
        {
            x = start_x;
            y += font_height;
            if(++line == sel_line)
                ptr += line_offset;
        }
        else if(*ptr && x + fontWidth(*ptr) < SCREEN_WIDTH)
            x += drawChar(*ptr, 0x0000, *screen, x, y);

        ++ptr;
    }
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

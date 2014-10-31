#include "browsertask.h"

#include <libndls.h>
#include <libgen.h>
#include <algorithm>
#include <dirent.h>

#include "cursortask.h"
#include "font.h"
#include "texturetools.h"

BrowserTask browser_task;

BrowserTask::BrowserTask()
    : DialogWidget("Open file")
{
    selection_background = newTexture(list_width, 12, 0x00F, false);
}

BrowserTask::~BrowserTask()
{
    deleteTexture(selection_background);
}

void BrowserTask::requestOpen()
{
    ok_button.setTitle("Open");

    this->makeCurrent();
}

void BrowserTask::requestSave()
{
    ok_button.setTitle("Save");

    this->makeCurrent();
}

void BrowserTask::logic()
{
    DialogWidget::logic();

    if(key_hold_down)
        key_hold_down = any_key_pressed();
    else if(isKeyPressed(KEY_NSPIRE_ESC))
    {
        filepath = "";
        old_task->makeCurrent();

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_UP))
    {
        if(selected_entry > 0)
            --selected_entry;

        if(selected_entry < entry_top)
            --entry_top;

        filename_input.setContent(entries[selected_entry].first);

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_DOWN))
    {
        if(selected_entry + 1 < entries.size())
            ++selected_entry;

        if(selected_entry >= list_height/fontHeight() + entry_top)
            ++entry_top;

        filename_input.setContent(entries[selected_entry].first);

        key_hold_down = true;
    }

    if(!key_hold_down && cursor_task.state && cursor_task.x > list_x && cursor_task.x < list_x + list_width && cursor_task.y > list_y && cursor_task.y < list_y + list_height)
    {
        unsigned int click_line = (cursor_task.y - list_y) / fontHeight() + entry_top;
        if(click_line < entries.size())
        {
            filename_input.setContent(entries[selected_entry].first);

            if(click_line == selected_entry)
                selectCurrentEntry();
            else
                selected_entry = click_line;
        }

        key_hold_down = true;
    }
}

void BrowserTask::render()
{
    copyTexture(*background, *screen);

    DialogWidget::render();

    unsigned int line = 0;
    unsigned int y = list_y;
    for(auto&& entry : entries)
    {
        if(line < entry_top)
        {
            ++line;
            continue;
        }

        if(y >= list_y + list_height)
            break;

        if(line == selected_entry)
        {
            drawTexture(*selection_background, *screen, 0, 0, selection_background->width, selection_background->height, list_x, y, selection_background->width, selection_background->height);
            drawString(entry.first.c_str(), 0xFFFF, *screen, list_x, y, EFont::Normal);
        }
        else
            drawString(entry.first.c_str(), 0x0000, *screen, list_x, y, EFont::Normal);

        y += fontHeight();

        ++line;
    }
}

void BrowserTask::makeCurrent()
{
    DIR *dir = opendir(filepath.c_str());
    if(!dir)
        filepath.erase(std::find(filepath.rbegin(), filepath.rend(), '/').base(), filepath.end());
    else
        closedir(dir);

    getEntries();
    need_background = true;

    old_task = Task::current_task;
    Task::makeCurrent();
}

void BrowserTask::event(Widget *source, Widget::Event event)
{
    if(source == &close_button && event == Event::Button_Press)
    {
        filepath = "";
        old_task->makeCurrent();
    }
    else if(source == &ok_button && event == Event::Button_Press)
        selectCurrentEntry();
}

void BrowserTask::selectCurrentEntry()
{
    filepath += "/" + filename_input.content();
    DIR *dir = opendir(filepath.c_str());
    if(!dir)
        old_task->makeCurrent();
    else
    {
        closedir(dir);
        getEntries();
    }
}

void BrowserTask::getEntries()
{
    selected_entry = entry_top = 0;
    entries.clear();

    DIR *dir = opendir(filepath.c_str());
    while(dirent *ent = readdir(dir))
    {
        //Test whether it's a directory
        DIR *test = opendir((filepath + "/" + ent->d_name).c_str());
        entries.emplace_back(std::string(ent->d_name), !!test);
        if(test)
            closedir(test);
    }

    closedir(dir);

    filename_input.setContent(entries[selected_entry].first);
}

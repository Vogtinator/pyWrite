#include "browsertask.h"

#include <libndls.h>
#include <algorithm>
#include <dirent.h>

#include "cursortask.h"
#include "font.h"
#include "texturetools.h"

BrowserTask browser_task;

BrowserTask::BrowserTask()
    : DialogWidget("Open file"), ok_button("Open", 200, 190, 35, 25, this),
      close_button("Close", 40, 190, 40, 25, this),
      filename_input(80, 170, 70, this)
{
    unsigned int list_width = 226;
    selection_background = newTexture(list_width, 12, 0x00F, false);
}

BrowserTask::~BrowserTask()
{
    deleteTexture(selection_background);
}

void BrowserTask::requestOpen()
{
    filepath = "/documents";
    getEntries();
    need_background = true;

    old_task = Task::current_task;
    Task::makeCurrent();
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

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_DOWN))
    {
        if(selected_entry + 1 < entries.size())
            ++selected_entry;

        if(selected_entry >= 156/fontHeight() + entry_top)
            ++entry_top;

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_ENTER))
    {
        filepath += "/" + entries[selected_entry].first;
        //If not a directory, exit
        if(!entries[selected_entry].second)
            old_task->makeCurrent();
        else
            getEntries();

        key_hold_down = true;
    }

    if(close_button.pressed())
    {
        filepath = "";
        old_task->makeCurrent();
    }
    else if(ok_button.pressed())
    {
        filepath += "/" + entries[selected_entry].first;
        //If not a directory, exit
        if(!entries[selected_entry].second)
            old_task->makeCurrent();
        else
            getEntries();
    }

    if(!key_hold_down && cursor_task.state && cursor_task.x > 42 && cursor_task.x < 226 && cursor_task.y > 57 && cursor_task.y < 213)
    {
        unsigned int click_line = (cursor_task.y - 57) / fontHeight() + entry_top;
        if(click_line < entries.size())
        {
            if(click_line == selected_entry)
            {
                filepath += "/" + entries[selected_entry].first;
                //If not a directory, exit
                if(!entries[selected_entry].second)
                    old_task->makeCurrent();
                else
                    getEntries();
            }
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
    unsigned int x = 42, y = 57;
    for(auto&& entry : entries)
    {
        if(line < entry_top)
        {
            ++line;
            continue;
        }

        if(y >= 213)
            break;

        if(line == selected_entry)
        {
            drawTexture(*selection_background, *screen, 0, 0, selection_background->width, selection_background->height, x, y, selection_background->width, selection_background->height);
            drawString(entry.first.c_str(), 0xFFFF, *screen, x, y, e_font::NORMAL);
        }
        else
            drawString(entry.first.c_str(), 0x0000, *screen, x, y, e_font::NORMAL);

        y += fontHeight();

        ++line;
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
}

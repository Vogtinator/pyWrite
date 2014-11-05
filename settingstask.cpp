#include <cstdint>
#include <cstdlib>

#include <libndls.h>

#include "settingstask.h"
#include "texturetools.h"

SettingsTask settings_task;

enum class SettingsType : uint32_t {
    PATH_MPYTHON=0
};

struct SettingsEntry {
    SettingsType type;
    uint32_t size;
} __attribute__((packed));

SettingsTask::SettingsTask()
    : DialogWidget("Settings")
{
    //Set defaults
    mpython_path.setContent("/documents/ndless/micropython.tns");

    FILE *settings_file = fopen("/documents/ndless/pywrite.cnf.tns", "rb");

    if(!settings_file)
        return;

    while(true)
    {
        SettingsEntry entry;
        if(fread(&entry, sizeof(entry), 1, settings_file) != 1)
            break;

        char *buf;
        switch(entry.type)
        {
        case SettingsType::PATH_MPYTHON:
            buf = reinterpret_cast<char*>(malloc(entry.size));
            if(!buf)
                goto skip_entry;

            if(fread(buf, entry.size, 1, settings_file) != 1)
                goto skip_entry;

            mpython_path.setContent(buf);

            free(reinterpret_cast<void*>(buf));
            break;

        default:
        skip_entry:
            fseek(settings_file, entry.size, SEEK_CUR);
            break;
        }
    }

    fclose(settings_file);
}

void SettingsTask::render()
{
    copyTexture(*background, *screen);

    DialogWidget::render();

    drawString("Path to micropython:", 0x0000, *screen, 45, 50, EFont::Normal);
}

void SettingsTask::logic()
{
    DialogWidget::logic();

    if(key_hold_down)
        key_hold_down = any_key_pressed();
    if(isKeyPressed(KEY_NSPIRE_ESC) && !key_hold_down)
    {
        old_task->makeCurrent();

        key_hold_down = true;
    }
}

void SettingsTask::makeCurrent()
{
    need_background = true;

    old_task = Task::current_task;
    Task::makeCurrent();
}

void SettingsTask::event(Widget *source, Widget::Event event)
{
    if(source == &save_button && event == Widget::Event::Button_Press)
    {
        //TODO: Save

        old_task->makeCurrent();
    }
}

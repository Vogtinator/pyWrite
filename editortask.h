#ifndef EDITORTASK_H
#define EDITORTASK_H

#include <string>

#include "task.h"

class EditorTask : public Task
{
public:
    EditorTask();
    virtual ~EditorTask() {}

    virtual void makeCurrent() override;
    virtual void logic() override;
    virtual void render() override;

private:
    struct MenuItem {
        const char *name;
        void (EditorTask:: *action)();
        unsigned int width; //Calculated in constructor of EditorTask
    };

    void menuNew();
    void menuOpen();
    void menuSave();
    void menuSaveAs();
    void menuRun();
    void menuExit();

    MenuItem menu_items[6] =
    {{"New", &EditorTask::menuNew, 0},
     {"Open", &EditorTask::menuOpen, 0},
     {"Save",  &EditorTask::menuSave, 0},
     {"Save as", &EditorTask::menuSaveAs, 0},
     {"Run", &EditorTask::menuRun, 0},
     {"Exit", &EditorTask::menuExit, 0}};

    static const unsigned int menu_width = 260, menu_height = 20, menu_x = (SCREEN_WIDTH - menu_width) / 2;

    const char *atLine(unsigned int l);
    unsigned int linesUntil(const char *end);
    void updateCursor();

    std::string filepath;
    std::string buffer = "Hi!\nThis is pyWrite 0.1 by Vogtinator!";

    // Start of selection (buffer offset) and x offset (horizontal scrolling)
    unsigned int sel_start = 0, sel_line = 0, x_offset = 0;
    unsigned int line_top = 0;

    unsigned int cursor_tick = 0, key_repeat = 0;
    static const unsigned int cursor_time = 30, key_repeat_count = 20, key_repeat_speed = 10;

    bool requested_open = false, requested_save = false, requested_confirm = false;
};

extern EditorTask editor_task;

#endif // EDITORTASK_H

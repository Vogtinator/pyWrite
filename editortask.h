#ifndef EDITORTASK_H
#define EDITORTASK_H

#include <string>

#include "task.h"

class EditorTask : public Task
{
public:
    virtual ~EditorTask() {};

    virtual void makeCurrent() override;
    virtual void logic() override;
    virtual void render() override;

private:
    const char *atLine(unsigned int l);
    unsigned int linesUntil(const char *end);

    std::string filepath;
    std::string buffer = "Test. This is a test.\nAnother line.";

    // Start of selection (buffer offset) and line offset (if line is too long)
    unsigned int sel_start = 0, sel_line = 0, line_offset = 0;
    unsigned int line_top = 0;

    unsigned int cursor_tick = 0, key_repeat = 0;
    static const unsigned int cursor_time = 30, key_repeat_count = 20, key_repeat_speed = 10;

    bool requested_open = false, requested_save = false;
};

extern EditorTask editor_task;

#endif // EDITORTASK_H

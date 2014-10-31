#ifndef CURSORTASK_H
#define CURSORTASK_H

#include "task.h"

class CursorTask : public Task
{
public:
    virtual ~CursorTask() {};

    virtual void logic() override;
    virtual void render() override;

    unsigned int x = 0, y = 0;
    bool state = false, show = true;
};

extern CursorTask cursor_task;

#endif // CURSORTASK_H

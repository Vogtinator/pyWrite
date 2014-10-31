#ifndef TASK_H
#define TASK_H

#include "gl.h"

class Task
{
public:
    virtual ~Task() {};

    virtual void render() = 0;
    virtual void logic() = 0;

    virtual void makeCurrent();

    static Task *current_task;

    static void showCursor();
};

extern bool key_hold_down, running, need_background;
extern TEXTURE *screen, *background;

#endif // TASK_H

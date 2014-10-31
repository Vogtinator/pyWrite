#include <libndls.h>

#include "task.h"

#include "cursortask.h"

Task *Task::current_task;

bool key_hold_down, running, need_background;
TEXTURE *screen, *background;

void Task::makeCurrent()
{
    current_task = this;
}

void Task::showCursor()
{
    cursor_task.show = has_colors || is_touchpad;
}

#ifndef SETTINGSTASK_H
#define SETTINGSTASK_H

#include "task.h"
#include "widget.h"

class SettingsTask : public DialogWidget, public Task
{
public:
    SettingsTask();
    virtual ~SettingsTask() {}

    virtual void render() override;
    virtual void logic() override;
    virtual void makeCurrent() override;

    virtual void event(Widget *source, Event event) override;

    TextlineWidget mpython_path = {45, 70, 235, this};

private:
    ButtonWidget save_button = {"Save", 240, 190, 40, 23, this};

    Task *old_task = nullptr;
};

extern SettingsTask settings_task;

#endif // SETTINGSTASK_H

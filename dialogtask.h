#ifndef DIALOGTASK_H
#define DIALOGTASK_H

#include <string>
#include <vector>

#include "task.h"
#include "widget.h"

class DialogTask : public Task, public ContainerWidget
{
public:
    DialogTask();
    virtual ~DialogTask();

    virtual void showMessage(std::string msg);
    virtual void confirm(std::string msg);

    virtual void logic() override;
    virtual void render() override;
    //Don't call this yourself, use request*()
    virtual void makeCurrent() override;
    virtual void event(Widget *source, Event event) override;

    std::string text;
    bool confirmed = false;

private:
    ButtonWidget ok_button = {"Ok", 150, 120, 20, 20, this}, yes_button = {"Yes", 120, 120, 20, 20, this}, no_button = {"No", 180, 120, 20, 20, this};
    TEXTURE *my_background = nullptr, *old_background = nullptr;

    Task *old_task = nullptr; //Jump to this task after dialog closed
};

extern DialogTask dialog_task;

#endif // DIALOGTASK_H

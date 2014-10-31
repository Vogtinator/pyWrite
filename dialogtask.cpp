#include "dialogtask.h"

#include <libndls.h>
#include <libgen.h>
#include <algorithm>

#include "cursortask.h"
#include "font.h"
#include "texturetools.h"

#include "textures/message.h"

DialogTask dialog_task;

DialogTask::DialogTask()
    : ContainerWidget()
{
    my_background = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
}

DialogTask::~DialogTask()
{
    deleteTexture(my_background);
}

void DialogTask::logic()
{
    ContainerWidget::logic();
}

void DialogTask::render()
{
    copyTexture(*my_background, *screen);

    drawTexture(message, *screen, 0, 0, message.width, message.height, 0, 0, screen->width, screen->height);

    drawStringCenter(text.c_str(), 0x0000, *screen, screen->width/2, screen->height/2 - 30, EFont::Normal);

    ContainerWidget::render();
}

void DialogTask::makeCurrent()
{
    old_background = background;
    background = my_background;
    need_background = true;

    old_task = Task::current_task;
    Task::makeCurrent();
}

void DialogTask::event(Widget *source, Widget::Event event)
{
    if(event != Event::Button_Press)
        return;

    if(source == &no_button)
        confirmed = false;
    else if(source == &yes_button)
        confirmed = true;

    background = old_background;
    old_task->makeCurrent();

}

void DialogTask::showMessage(std::string msg)
{
    this->text = msg;

    children.clear();
    addChild(&ok_button);

    this->makeCurrent();
}

void DialogTask::confirm(std::string msg)
{
    this->text = msg;

    children.clear();
    addChild(&yes_button);
    addChild(&no_button);

    this->makeCurrent();
}

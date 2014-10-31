#include "widget.h"

#include <libndls.h>

#include "cursortask.h"
#include "font.h"
#include "texturetools.h"

#include "textures/dialog.h"

//Helper functions
extern "C" char nio_ascii_get(int *a);

bool cursorOn(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    return cursor_task.x >= x && cursor_task.y >= y && cursor_task.x - x < w && cursor_task.y - y < h;
}

Widget::Widget(ContainerWidget *parent) : parent(parent)
{
    if(parent)
        parent->addChild(this);
}

void Widget::focus()
{
    parent->unfocus();
    has_focus = true;
}


void ContainerWidget::addChild(Widget *c)
{
    children.push_back(c);
}

void ContainerWidget::unfocus()
{
    for(auto& c : children)
        c->unfocus();
}

void ContainerWidget::logic()
{
    for(auto& c : children)
        c->logic();
}

void ContainerWidget::focus()
{
}

void ContainerWidget::render()
{
    for(auto& c : children)
        c->render();
}


bool ButtonWidget::pressed()
{
    return is_pressed;
}

void ButtonWidget::logic()
{
    is_hovering = cursorOn(x, y, width, height);
    is_pressed = cursor_task.state && is_hovering;
}

void ButtonWidget::render()
{
    drawRectangle(*screen, x, y, width, height, 0x0000);
    if(is_hovering && !is_pressed)
        drawRectangle(*screen, x + 1, y + 1, width - 1, height - 1, 0x0000);

    drawStringCenter(title, 0x0000, *screen, x + (width/2), y + (height/2) - fontHeight() / 2);
}


void DialogWidget::logic()
{
    ContainerWidget::logic();
}

void DialogWidget::render()
{
    drawTexture(dialog, *screen, 0, 0, dialog.width, dialog.height, 0, 0, screen->width, screen->height);

    drawStringCenter("Select file", 0x0000, *screen, SCREEN_WIDTH/2, 30, e_font::LARGE);

    ContainerWidget::render();
}


void TextlineWidget::logic()
{
    if(!has_focus && cursor_task.state && cursorOn(x, y, width, height))
        focus();

    if(!has_focus)
        return;

    int i;
    char c = nio_ascii_get(&i);
    if(c >= 0x80 || c == '\n')
        return;

    if(key_hold_down)
        key_hold_down = any_key_pressed();
    else if(isKeyPressed(KEY_NSPIRE_LEFT))
    {
        if(cursor_pos > 0)
            --cursor_pos;

        key_hold_down = true;
    }
    else if(isKeyPressed(KEY_NSPIRE_RIGHT))
    {
        if(cursor_pos < text.length())
            ++cursor_pos;

        key_hold_down = true;
    }
    else if(c)
    {
        if(c == '\b')
        {
            if(cursor_pos > 0)
            {
                text.erase(text.begin() + (cursor_pos - 1));
                --cursor_pos;
            }
        }
        else
        {
            text.insert(text.begin() + cursor_pos, c);

            ++cursor_pos;
        }

        key_hold_down = true;
    }
}

void TextlineWidget::render()
{
    drawRectangle(*screen, x, y, width, height, 0x0000);

    const char *str = text.c_str(), *cursor = str + cursor_pos;
    unsigned int x1 = x + 1;

    while(*str)
    {
        if(has_focus && str == cursor)
            drawChar('|', 0x0000, *screen, x1 - 3, y + height/2 - fontHeight()/2);

        x1 += drawChar(*str, 0x0000, *screen, x1, y + height/2 - fontHeight()/2);

        ++str;
    }

    if(str == cursor)
        drawChar('|', 0x0000, *screen, x1 - 3, y + height/2 - fontHeight()/2);
}

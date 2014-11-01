#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <vector>

#include "font.h"

class ContainerWidget;

class Widget
{
public:
    Widget(ContainerWidget *parent);
    virtual ~Widget() {}

    virtual void logic() {}
    virtual void focus();
    virtual void unfocus() { has_focus = false; }
    virtual void render() {}

    enum Event {
        Button_Press = 0,
        Button_Release = 1
    };

protected:
    bool has_focus = false;
    ContainerWidget *parent;
};

class ContainerWidget : public Widget
{
public:
    ContainerWidget() : Widget(nullptr) {}
    virtual ~ContainerWidget() {}
    virtual void addChild(Widget *c);

    virtual void logic() override;
    virtual void focus() override;
    virtual void unfocus() override;
    virtual void render() override;
    virtual void event(Widget *source, Event event) = 0;

protected:
    std::vector<Widget*> children;
};

class ButtonWidget : public Widget
{
public:
    ButtonWidget(const char *title, unsigned int x, unsigned int y, unsigned int width, unsigned int height, ContainerWidget *parent)
        : Widget(parent), title(title), x(x), y(y), width(width), height(height) {}

    virtual void setTitle(const char *title);
    virtual bool pressed();
    virtual void logic() override;
    virtual void render() override;

private:
    bool is_pressed = false, is_hovering = false;
    const char *title;
    unsigned int x, y, width, height;
};

class TextlineWidget : public Widget
{
public:
    TextlineWidget(unsigned int x, unsigned int y, unsigned int width, ContainerWidget *parent)
        : Widget(parent), x(x), y(y), width(width), height(13) {}

    virtual std::string content() { return text; }
    virtual void setContent(std::string str) { text = str; cursor_pos = 0; }
    virtual void logic() override;
    virtual void render() override;

private:
    std::string text = "Hi!";
    unsigned int x, y, width, height, cursor_pos = 0;
};

class DialogWidget : public ContainerWidget
{
public:
    DialogWidget(const char *title) : title(title) {}
    virtual ~DialogWidget() {}

    virtual void logic() override;
    virtual void render() override;

protected:
    const char *title;
};

#endif // WIDGET_H

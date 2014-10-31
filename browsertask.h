#ifndef BROWSERTASK_H
#define BROWSERTASK_H

#include <string>
#include <vector>

#include "task.h"
#include "widget.h"

class BrowserTask : public Task, public DialogWidget
{
public:
    BrowserTask();
    virtual ~BrowserTask();

    virtual void requestOpen();
    virtual void requestSave();
    virtual void logic() override;
    virtual void render() override;
    //Don't call this yourself, use request*()
    virtual void makeCurrent() override;
    virtual void event(Widget *source, Event event) override;

    std::string filepath = "/documents";

private:
    void selectCurrentEntry();

    void getEntries();
    unsigned int selected_entry = 0, entry_top = 0;

    static const unsigned int list_x = 42, list_y = 56, list_width = 226, list_height = 124;

    ButtonWidget ok_button = {"Open", 242, 190, 40, 23, this}, close_button = {"Close", 38, 190, 40, 23, this};
    TextlineWidget filename_input = {100, 192, 115, this};

    TEXTURE *selection_background;
    std::vector<std::pair<std::string,bool>> entries; //bool: directory
    Task *old_task; //Jump to this task after selection made or canceled
};

extern BrowserTask browser_task;

#endif // BROWSERTASK_H

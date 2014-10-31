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

    //For the list
    TEXTURE *selection_background; //Blue bar
    std::vector<std::pair<std::string,bool>> entries; //Pair of string: filename and bool: directory or not
    static const unsigned int list_x = 40, list_y = 55, list_width = 240, list_height = 125;
    unsigned int selected_entry = 0, entry_top = 0;

    ButtonWidget ok_button = {"Open", 240, 190, 40, 23, this}, close_button = {"Close", 40, 190, 40, 23, this};
    TextlineWidget filename_input = {100, 194, 115, this};

    Task *old_task; //Jump to this task after selection made or canceled
};

extern BrowserTask browser_task;

#endif // BROWSERTASK_H

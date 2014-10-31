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
    virtual void logic() override;
    virtual void render() override;

    void openFile();

    std::string filepath;

private:
    void getEntries();
    unsigned int selected_entry = 0, entry_top = 0;

    ButtonWidget ok_button, close_button;
    TextlineWidget filename_input;

    TEXTURE *selection_background;
    std::vector<std::pair<std::string,bool>> entries; //bool: directory
    Task *old_task; //Jump to this task after selection made or canceled
};

extern BrowserTask browser_task;

#endif // BROWSERTASK_H

#include <libndls.h>

#include "gl.h"
#include "texturetools.h"
#include "task.h"

#include "editortask.h"
#include "cursortask.h"

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    nglInit();

    screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    background = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    nglSetBuffer(screen->bitmap);

    glLoadIdentity();

    editor_task.makeCurrent();

    running = true;

    while(running)
    {
        glColor3f(1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();

        cursor_task.logic();

        Task *current_task = Task::current_task;
        current_task->logic();
        current_task->render();

        if(need_background)
        {
            copyTexture(*screen, *background);
            need_background = false;
        }

        cursor_task.render();

        glPopMatrix();

        nglDisplay();
    }

    deleteTexture(background);
    deleteTexture(screen);

    nglUninit();
    return 0;
}

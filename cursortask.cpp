#include "cursortask.h"

#include <libndls.h>
#include <algorithm>

#include "texturetools.h"

#include "textures/cursor.h"

CursorTask cursor_task;

void CursorTask::logic()
{
    if(!show)
		return;
	
    touchpad_report_t touchpad;
    touchpad_scan(&touchpad);

    state = touchpad_arrow_pressed(TPAD_ARROW_CLICK);

	int dx = 0, dy = 0;
    static bool tp_last_contact = touchpad.contact;
    if(touchpad.contact && !touchpad.pressed)
    {
        static int tp_last_x = touchpad.x, tp_last_y = touchpad.y;

        if(tp_last_contact)
        {
            dx = (touchpad.x - tp_last_x) / 10;
            dy = (tp_last_y - touchpad.y) / 10;
        }

        tp_last_x = touchpad.x;
        tp_last_y = touchpad.y;
        tp_last_contact = touchpad.contact;
	}
    else
        tp_last_contact = false;

#ifdef EMU
    state = touchpad.pressed;

    //For nspire_emu
    if(state)
	{
		switch(touchpad.arrow)
		{
		case TPAD_ARROW_DOWN:
			dy = 2;
			break;
		case TPAD_ARROW_UP:
			dy = -2;
			break;
		case TPAD_ARROW_LEFT:
			dx = -2;
			break;
		case TPAD_ARROW_RIGHT:
			dx = 2;
			break;
		}
    }

    state = isKeyPressed(KEY_NSPIRE_SCRATCHPAD);
#endif

	x = std::max<int>({0, dx + x});
	y = std::max<int>({0, dy + y});

	x = std::min<int>({x, SCREEN_WIDTH});
	y = std::min<int>({y, SCREEN_HEIGHT});
}

void CursorTask::render()
{
    if(!show)
		return;

	unsigned int width = std::min<unsigned int>({cursor.width, SCREEN_WIDTH - x}), height = std::min<unsigned int>({cursor.height, SCREEN_HEIGHT - y});
	drawTexture(cursor, *screen, 0, 0, width, height, x, y, width, height);
}

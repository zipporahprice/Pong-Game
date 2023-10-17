#include "navswitch.h"
#include "ledmat.h"
#include "helper.h"
#include "bar.h"

Vector_t position;

void bar_init()
{
    position = (Vector_t) {(LEDMAT_ROWS_NUM / 2) - (BAR_LENGTH / 2), LEDMAT_COLS_NUM - 1};
}

void bar_set_position(int8_t x, int8_t y)
{
    position.x = x;
    position.y = y;
}

Vector_t bar_get_position()
{
    return position;
}

void bar_update()
{
    navswitch_update ();
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) { 
        position.x = max(0, position.x - 1);
    }
    else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        position.x = min(LEDMAT_COLS_NUM - BAR_LENGTH + 2, position.x + 1);
    }
}

void bar_display()
{
    ledmat_display_column(BAR_MASK << position.x, position.y);
}
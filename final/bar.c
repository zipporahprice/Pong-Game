/**
 * @file bar.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module containing functions relating to movement, position, or display of the Pong bar object.
*/

#include "navswitch.h"
#include "ledmat.h"
#include "helper.h"
#include "bar.h"

#define BAR_MASK ((1 << BAR_LENGTH) - 1)

Vector_t position;

/**
 * Initializes the position of the bar object to the center of the screen horizontally, in the row closest to the player.
*/
void bar_init()
{
    position = (Vector_t) {(LEDMAT_ROWS_NUM / 2) - (BAR_LENGTH / 2), LEDMAT_COLS_NUM - 1};
}

/**
 * Sets the position of the bar.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void bar_set_position(int8_t x, int8_t y)
{
    position.x = x;
    position.y = y;
}

/**
 * @return A Vector_t containing the position of the bar. 
*/
Vector_t bar_get_position()
{
    return position;
}

/**
 * Gets the user input from the navswitch, and increments or decrements
 * the bar's position depending on which way it's moved.
*/
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

/**
 * Displays the bar on the screen.
*/
void bar_display()
{
    ledmat_display_column(BAR_MASK << position.x, position.y);
}
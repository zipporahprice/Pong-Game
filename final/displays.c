/**
 * @file displays.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module containing various functions for displaying text on the screen and multiplexing handling.
*/

#include "system.h"
#include "pacer.h"
#include "../fonts/font5x7_1.h"
#include "tinygl.h"
#include "navswitch.h"
#include "displays.h"
#include "bar.h"
#include "ball.h"

/**
 * Initializes the tinygl display variables for the scrolling text 
*/
void init_display(void)
{
    tinygl_init (TINYGL_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
}

/**
 * Switches between displaying the bar and ball to avoid flickering.
 * @param isTurn Whether or not it is the player's turn. If it isn't, the ball shouldn't be rendered. 
*/
void toggle_display(int8_t isTurn)
{
        static int8_t toggle = 0;
        if (toggle == 0) {
            bar_display();
        } else if (isTurn == 1) {
            ball_display();
        }
        toggle = ~toggle;
}

/**
 * Display the scrolling text for the welcome screen.
*/
void welcome_screen(void)
{
    tinygl_text("Welcome to Pong!");
}

/**
 * Display the scrolling text for when the player wins.
*/
void won_screen(void)
{
    tinygl_text("WINNER");
}

/**
 * Display the scrolling text for when the player loses.
*/
void lost_screen(void)
{
    tinygl_text("LOSER");
}
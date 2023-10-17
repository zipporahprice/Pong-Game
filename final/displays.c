#include "system.h"
#include "pacer.h"
#include "../fonts/font5x7_1.h"
#include "tinygl.h"
#include "navswitch.h"

#include "displays.h"


void init_display(void)
{
    tinygl_init (TINY_INIT);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
}

void toggle_display(int8_t isTurn)
{
        static int8_t toggle = 0;
        if (toggle == 0) {
            bar_display();
        } else if (isTurn == 1) {
            display_ball();
        }
        toggle = ~toggle;
}

void welcome_screen(void)
{
    tinygl_text("Welcome to Pong!");
}

void won_screen(void)
{
    tinygl_text("WINNER");
}

void lost_screen(void)
{
    tinygl_text("LOSER");
}
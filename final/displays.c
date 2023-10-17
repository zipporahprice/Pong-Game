#include "navswitch.h"
#include "ledmat.h"
#include "helper.h"
#include "bar.h"
#include "ball.h"
#include "tinygl.h"

void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
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

// welcome screen
void welcome_screen(void)
{
    tinygl_text("Welcome!");
}

// game intro??
void intro_screen(void)
{
    tinygl_text("First to three points wins!");
}

// winner screen
void won_screen(void)
{
    tinygl_text("WINNER");
}

// loser screen
void lost_screen(void)
{
    tinygl_text("LOSER");
}
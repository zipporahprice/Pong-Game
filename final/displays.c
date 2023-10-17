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

// void display_message(const char* message)
// {
//     tinygl_text(message);
//     bool game_start = 0;

//     while (!game_start) {
//         navswitch_update();
//         if (ir_uart_read_ready_p ()) {
//             if (ir_uart_getc () == 'p') {
//                 is_player1 = 0;
//                 game_start = 1;
//             }
//         }
//         if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
//             ir_uart_putc('p');
//             game_start = 1;
//         }
//         tinygl_update ();
//     }
// }

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
    //bool clicked = 0;
    tinygl_text("Welcome! Click to start.");

    // while(!clicked)
    // {
    //     pacer_wait();
    //     tinygl_update();

    //     navswitch_update();
    //     if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
    //         clicked = 1;
    //     }
    // }
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
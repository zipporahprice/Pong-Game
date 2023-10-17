#include "system.h"
#include "pio.h"
#include "ledmat.h"
#include "navswitch.h"
#include "pacer.h"
#include "stdlib.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "button.h"

#include "ball.h"
#include "bar.h"
#include "displays.h"
#include "ir_uart.h"

#define PACER_RATE 500
#define MESSAGE_RATE 10

#define SCORE_PACKET 0
#define BALL_PACKET 1
#define WON 3

int8_t this_score = 0;
int8_t other_score = 0;
int8_t isTurn;
int8_t count;
int8_t speed;


void send_ball_packet(int8_t row_position, int8_t direction) {
    ir_uart_putc(BALL_PACKET);
    ir_uart_putc(LEDMAT_ROWS_NUM - (row_position + 1));
    ir_uart_putc(-1 * direction);
}

/**
 * Sends the current scores to the other player
*/
void send_score_packet(void) {
    ir_uart_putc(SCORE_PACKET);
    ir_uart_putc(this_score);
    ir_uart_putc(other_score);
}

/**
 * Receives data: either ball position or score update
*/
void receive_packet(void) {
    int8_t packet_type = ir_uart_getc();
    switch (packet_type) {
        case (BALL_PACKET):
            set_ball_position(ir_uart_getc(), 0);
            set_ball_velocity(ir_uart_getc(), 1);
            break;
        
        case (SCORE_PACKET):
            other_score = ir_uart_getc();
            this_score = ir_uart_getc();
            break;
    }
}


int8_t turn_handshake(void) {
        int8_t order = -1;
    // Handle who goes first
    while (order == -1) {
        navswitch_update ();
        uint8_t data;
        // On button push, sends a data package 1: "I want to be first!"
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) { 
            ir_uart_putc(1);
        }
        
        // Receives any messages sent by the other board
        if (ir_uart_read_ready_p()) {
            data = ir_uart_getc();
            switch (data) {
                // other board has already said they want to be first.
                case 1:
                    order = 0;
                    ir_uart_putc(2); // send an acknowledgement
                    break;

                // other board has admitted defeat, this board is first
                case 2:
                    order = 1;
                    break;
            }
        }
        tinygl_update();
    }
    return order;
}

void check_score(void)
{   
    if (this_score == WON) {
        // winner
        won_screen();
    } else if (other_score == WON) {
        // loser
        lost_screen();
    } else {
        // continue playing
    }
}

void init_game(void) {
    system_init();
    ledmat_init();
    pacer_init (100);
    navswitch_init();
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    ir_uart_init();
    button_init();
    ball_init();
    bar_init();

    count = 0;
    speed = 40;

    isTurn = turn_handshake();
}


int main (void)
{
    init_game();

    // do welcome screen
    welcome_screen();

    while(1)
    {
        pacer_wait();
        if (isTurn == 0) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
                isTurn = 1;
            }
        } else if (isTurn == 1 && count >= speed) {
            if (hits_paddle(bar_get_position())) {
                paddle_bounce();
                speed -= 3;
            }
            // if it hits the back wall, send updated scores
            if (hits_back_wall()) {
                send_score_packet();
                wall_stop();
                speed = 40;
            } 
            if (get_ball_position().y < 0 && get_velocity().y == -1) {
                isTurn = 0;
                send_ball_packet(get_ball_position().x, get_velocity().x);
            }
            if (hits_side()) {
                wall_bounce();
            }
            update_ball_position();
            count = 0;
        }

        // handle bar movement
        bar_update();

        toggle_display(isTurn);
        count++;
    }
}
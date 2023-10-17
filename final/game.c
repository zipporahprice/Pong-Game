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
    if (packet_type == SCORE_PACKET) {    
        other_score = ir_uart_getc();
        this_score = ir_uart_getc();
        check_score();
    } else if (packet_type == BALL_PACKET) {
        set_ball_position(ir_uart_getc(), 0);
        set_ball_velocity(ir_uart_getc(), 1);
        isTurn = 1;
    }
}


int8_t turn_handshake(void) {
    int8_t order = -1;
    welcome_screen();

    // Handle who goes first
    while (order == -1) {
        navswitch_update ();

        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'X') {
                order = 0;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('X');
            order = 1;
        }
        tinygl_update();
    }
    return order;
}

void check_score(void) {
    if (this_score == WON) {
        // winner
        won_screen();
        scroll_until_click();
    } else if (other_score == WON) {
        // loser
        lost_screen();
        scroll_until_click();
    }
}

void scroll_until_click(void) {
    int8_t clicked = 0;
    while (!clicked) {
        navswitch_update ();

        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'X') {
                clicked = 1;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('X');
            clicked = 1;
        }
        tinygl_update();
    }
}

void init_game(void) {
    system_init();
    
    pacer_init (PACER_RATE);
    navswitch_init();
    init_display();
    ir_uart_init();
    button_init();
    
    count = 0;
    speed = 40;

    isTurn = turn_handshake();
    ledmat_init();
    ball_init();
    bar_init();
}


int main (void)
{
    init_game();

    while(1)
    {
        button_update();
        if (button_push_event_p(BUTTON1)) {
            while (1) {
                ledmat_display_column(this_score, 0);
                ledmat_display_column(other_score, 1);
            }
        }
        pacer_wait();
        // if not turn, waits for ball
        if (isTurn == 0) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
            }
        } // is their turn:
        else if (isTurn == 1 && count >= speed) {
            // if ball hits the paddle
            if (hits_paddle(bar_get_position())) {
                paddle_bounce();
                speed -= 3;
            }
            // if ball hits the back wall, opponent gains a point - send updated scores
            if (hits_back_wall()) {
                other_score++;
                send_score_packet();
                check_score();
                
                set_ball_position(-1, -1);
                ball_stop();
                speed = 40;
                send_ball_packet(LEDMAT_ROWS_NUM / 2, 1);
                isTurn = 0;
            }
            // if ball crosses border, send ball to opponent
            if (crosses_boundary()) {
                isTurn = 0;
                send_ball_packet(get_ball_position().x, get_velocity().x);
                ball_stop();
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
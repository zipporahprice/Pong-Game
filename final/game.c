/**
 * @file game.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Main functionality for pong game.
*/

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

#define SCORE 0
#define BALL 1
#define WON 3

int8_t this_score = 0;
int8_t other_score = 0;
int8_t is_turn;
int8_t count;
int8_t speed;

/**
 * Sends the current ball position to the opponent.
*/
void send_ball_packet(int8_t row_position, int8_t direction)
{
    ir_uart_putc(BALL);
    ir_uart_putc(LEDMAT_ROWS_NUM - (row_position + 1));
    ir_uart_putc(-1 * direction);
}

/**
 * Sends the current scores to the opponent.
*/
void send_score_packet(void)
{
    ir_uart_putc(SCORE);
    ir_uart_putc(this_score);
    ir_uart_putc(other_score);
}

/**
 * Keeps current display until navswitch is clicked - then exits loop.
*/
void scroll_until_click(void)
{
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

/**
 * Checks the current scores for a winner.
*/
void check_score(void)
{
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

/**
 * Receives data: either score update or ball position.
 * If score update:
*/
void receive_packet(void)
{
    int8_t packet_type = ir_uart_getc();
    if (packet_type == SCORE) {    
        other_score = ir_uart_getc();
        this_score = ir_uart_getc();
        check_score();
    } else if (packet_type == BALL) {
        set_ball_position(ir_uart_getc(), 0);
        set_ball_velocity(ir_uart_getc(), 1);
        is_turn = 1;
    }
}

/**
 * Decides who starts the game based on which player pushes the navswitch first.
*/
int8_t decide_turn(void)
{
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

/**
 * Initialises the game conditions.
*/
void init_game(void)
{
    system_init();
    pacer_init (PACER_RATE);
    navswitch_init();
    init_display();
    ir_uart_init();
    button_init();
    count = 0;
    speed = 40;
    is_turn = turn_handshake();
    ledmat_init();
    ball_init();
    bar_init();
}

/**
 * Main gameplay for the pong game.
*/
int main (void)
{
    init_game();

    while(1) {
        button_update();
        if (button_push_event_p(BUTTON1)) {
            while (1) {
                ledmat_display_column(this_score, 0);
                ledmat_display_column(other_score, 1);
            }
        }
        pacer_wait();
        // if not turn, waits for ball
        if (is_turn == 0) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
            }
        } else if (is_turn == 1 && count >= speed) {
            // if ball hits the paddle
            int8_t hit_code = hits_paddle(bar_get_position());
            if (hit_code != NOT_HIT) {
                paddle_bounce();
                speed -= 3;
                set_ball_velocity(hit_code, get_velocity().y);
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
                is_turn = 0;
            }
            // if ball crosses border, send ball to opponent
            if (crosses_boundary()) {
                is_turn = 0;
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
        toggle_display(is_turn);
        count++;
    }
}
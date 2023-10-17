/**
 * @file game.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Main game loop for the Pong game. 
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
#define SCORE_TO_WIN 3
#define STARTING_SPEED 40

int8_t this_score = 0;
int8_t other_score = 0;
int8_t is_turn;
int8_t count;
int8_t speed;

/**
 * Sends the ball to the opponent's screen using 3 separate putc calls.
 * First call: Packet type, stored in BALL definition.
 * Second call: The row the ball should appear on, need to flip it as the other board is rotated 180 degrees.
 * Third call: The direction the ball should be travelling in, need to flip it as above.
 * @param row_position a value from 0 to (LEDMAT_ROWS_NUM - 1) representing the row the ball should appear on
 * @param direction a value from -1 to 1 representing the x-component of the ball's velocity  
*/
void send_ball_packet(int8_t row_position, int8_t direction)
{
    ir_uart_putc(BALL);
    ir_uart_putc(LEDMAT_ROWS_NUM - (row_position + 1));
    ir_uart_putc(-1 * direction);
}

/**
 * Sends a score packet to the opponent containing the player's current score and the opponent's updated score.
 * @param row_position a value from 0 to (LEDMAT_ROWS_NUM - 1) representing the row the ball should appear on
 * @param direction a value from -1 to 1 representing the x-component of the ball's velocity  
*/
void send_score_packet(void)
{
    ir_uart_putc(SCORE);
    ir_uart_putc(this_score);
    ir_uart_putc(other_score);
}

/**
 * Blocks until navswitch is clicked - then exits loop.
 * Includes tinygl_update(), so useful for scrolling text until user input.
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
 * Checks if either player has reached the winning number of points.
 * Triggers win/loss screens if so, ignores call if not.
*/
void check_score(void)
{
    if (this_score == SCORE_TO_WIN) {
        // This player is the winner
        won_screen();
        scroll_until_click();
    } else if (other_score == SCORE_TO_WIN) {
        // The opponent was the winner
        lost_screen();
        scroll_until_click();
    }
}

/**
 * Receives a packet from the opponent.
 * Starts by receiving which type of packet it is (always the first character).
 * Two more getc calls follow (what they contain depends on the type of packet).
 * For further reference see send_ball_packet() and send_score_packet().
*/
void receive_packet(void)
{
    int8_t packet_type = ir_uart_getc();
    if (packet_type == SCORE) {    
        other_score = ir_uart_getc();
        this_score = ir_uart_getc();
        check_score();
    } else if (packet_type == BALL) {
        ball_set_position(ir_uart_getc(), 0);
        int8_t received_velocity_x = clamp(ir_uart_getc(), -1, 1);
        ball_set_velocity(received_velocity_x, 1);
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
    speed = STARTING_SPEED;
    is_turn = decide_turn();
    ledmat_init();
    ball_init();
    bar_init();
}

/**
 * Main gameplay loop for the pong game.
*/
int main (void)
{
    init_game();

    while(1) {

        pacer_wait();

        // Debug menu on button push, cuts execution and displays the below variables in binary.
        button_update();
        if (button_push_event_p(BUTTON1)) {
            while (1) {
                ledmat_display_column(ball_get_position().x, 0);
                ledmat_display_column(ball_get_position().y, 1);
                ledmat_display_column(ball_get_velocity().x, 2);
                ledmat_display_column(ball_get_velocity().y, 3);
                ledmat_display_column((this_score << 4) + other_score, 4);
            }
        }

        // If it's not the player's turn, player waits for communication from the opponent
        if (is_turn == 0) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
            }
        } else if (is_turn == 1 && count >= speed) {
            if (ball_hits_paddle(bar_get_position())) {
                ball_bounce_y();
                speed -= 3;
            }
            // If ball hits the back wall, opponent gains a point - send updated scores
            if (ball_hits_back_wall()) {
                other_score++;
                send_score_packet();
                check_score();
                
                ball_set_position(-1, -1);
                ball_stop();
                speed = STARTING_SPEED;
                send_ball_packet(LEDMAT_ROWS_NUM / 2, 1);
                is_turn = 0;
            }
            // If ball crosses the border furthest from the player, send ball to opponent's screen
            if (ball_crosses_boundary()) {
                is_turn = 0;
                send_ball_packet(ball_get_position().x, ball_get_velocity().x);
                ball_stop();
            }
            if (ball_hits_side()) {
                ball_bounce_x();
            }
            ball_update_position();
            count = 0;
        }

        bar_update();
        toggle_display(is_turn);
        count++;
    }
}
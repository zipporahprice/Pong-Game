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

#define SCORE_PACKET 0
#define BALL_PACKET 1
#define SCORE_TO_WIN 3
#define STARTING_SPEED 40

int8_t player_score = 0;
int8_t opponent_score = 0;
bool is_turn;
int8_t tick_count;
int8_t speed;

/**
 * Sends the ball to the opponent's screen using 3 separate putc calls.
 * First call: Packet type, stored in BALL_PACKET definition.
 * Second call: The row the ball should appear on, need to flip it as the other board is rotated 180 degrees.
 * Third call: The direction the ball should be travelling in, need to flip it as above.
 * @param row_position a value from 0 to (LEDMAT_ROWS_NUM - 1) representing the row the ball should appear on
 * @param direction a value from -1 to 1 representing the x-component of the ball's velocity  
*/
void send_ball_packet(int8_t row_position, int8_t direction)
{
    ir_uart_putc(BALL_PACKET);
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
    ir_uart_putc(SCORE_PACKET);
    ir_uart_putc(player_score);
    ir_uart_putc(opponent_score);
}

/**
 * Blocks until navswitch is clicked - then exits loop.
 * Includes tinygl_update(), so useful for scrolling text until user input.
*/
void scroll_until_click(void)
{
    bool is_clicked = 0;
    while (!is_clicked) {
        navswitch_update();
        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'X') {
                is_clicked = TRUE;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('X');
            is_clicked = TRUE;
        }
        tinygl_update();
    }
}

/**
 * Decides who starts the game based on which player pushes the navswitch first.
*/
int8_t decide_turn(void)
{
    int8_t turn_order = -1;
    welcome_screen();
    // Handle who goes first
    while (turn_order == -1) {
        navswitch_update();
        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'X') {
                turn_order = 0;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('X');
            turn_order = 1;
        }
        tinygl_update();
    }
    return turn_order;
}

/**
 * Resets the game state back to the start.
*/
void refresh_game(void)
{
    tick_count = 0;
    speed = STARTING_SPEED;
    player_score = 0;
    opponent_score = 0;
    is_turn = decide_turn();
    ledmat_init();
    ball_init();
    bar_init();
    send_score_packet();
}

/**
 * Checks if either player has reached the winning number of points.
 * Triggers win/loss screens if so, ignores call if not.
*/
void check_if_winner(void)
{
    if (player_score == SCORE_TO_WIN) {
        // This player is the winner
        won_screen();
        scroll_until_click();
        refresh_game();
    } else if (opponent_score == SCORE_TO_WIN) {
        // The opponent was the winner
        lost_screen();
        scroll_until_click();
        refresh_game();
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
    if (packet_type == SCORE_PACKET) {    
        opponent_score = ir_uart_getc();
        player_score = ir_uart_getc();
        check_if_winner();
    } else if (packet_type == BALL_PACKET) {
        ball_set_position(ir_uart_getc(), 0);
        int8_t received_velocity_x = clamp(ir_uart_getc(), -1, 1);
        ball_set_velocity(received_velocity_x, 1);
        is_turn = TRUE;
    }
}

/**
 * Initialises the libraries and resets the game state.
*/
void init_game(void)
{
    system_init();
    pacer_init(PACER_RATE);
    navswitch_init();
    init_display();
    ir_uart_init();
    button_init();
    refresh_game();
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
                ledmat_display_column((player_score << 4) + opponent_score, 4);
            }
        }
        // If it's not the player's turn, player waits for communication from the opponent
        if (is_turn == FALSE) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
            }
        } else if (is_turn == TRUE && tick_count >= speed) {
            if (ball_hits_paddle(bar_get_position())) {
                ball_bounce_y();
                speed -= 3;
            }
            // If ball hits the back wall, opponent gains a point - send updated scores
            if (ball_hits_back_wall()) {
                opponent_score++;
                send_score_packet();
                check_if_winner();
                
                ball_set_position(-1, -1);
                ball_stop();
                speed = STARTING_SPEED;
                send_ball_packet(LEDMAT_ROWS_NUM / 2, 1);
                is_turn = FALSE;
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
            tick_count = 0;
        }
        bar_update();
        toggle_display(is_turn);
        tick_count++;
    }
}
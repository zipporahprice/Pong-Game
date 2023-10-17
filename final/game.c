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
#include "ir_uart.h"

#define PACER_RATE 500
#define MESSAGE_RATE 10

#define SCORE_PACKET 0
#define BALL_PACKET 1


void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}

void send_ball_packet(int8_t row_position, int8_t direction) {
    ir_uart_putc(BALL_PACKET);
    ir_uart_putc(LEDMAT_ROWS_NUM - (row_position + 1));
    ir_uart_putc(-1 * direction);
}

void receive_packet(void) {
    int8_t packet_type = ir_uart_getc();
    switch (packet_type) {
        case (BALL_PACKET):
            set_ball_position(ir_uart_getc(), 0);
            set_ball_velocity(ir_uart_getc(), 1);
            break;
    }
}

void toggle_display(int8_t isTurn) {
        static int8_t toggle = 0;
        if (toggle == 0) {
            bar_display();
        } else if (isTurn == 1) {
            display_ball();
        }
        toggle = ~toggle;
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

int main (void)
{
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
    int8_t count = 0;
    int8_t speed = 40;

    int8_t isTurn = turn_handshake();

    while(1)
    {
        pacer_wait();
        if (isTurn == 0) {
            if (ir_uart_read_ready_p()) {
                receive_packet();
                isTurn = 1;
            }
        } else if (isTurn == 1 && count == speed) {
            if (hits_paddle(bar_get_position())) {
                paddle_bounce();
                speed -= 5;
            }
            if (hits_back_wall()) {
                wall_stop();
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
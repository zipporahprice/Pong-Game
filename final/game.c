#include "system.h"
#include "pio.h"
#include "ledmat.h"
#include "navswitch.h"
#include "pacer.h"
#include "stdlib.h"
#include "ir_serial.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"

#define BAR_LENGTH 7
int8_t DELTA_ROW = 1;
int8_t DELTA_COL = 1;

int8_t BAR_MASK = (1 << BAR_LENGTH) - 1;

#define PACER_RATE 500
#define MESSAGE_RATE 10

#define SCORE_PACKET 0
#define BALL_PACKET 1

typedef struct
{
    int8_t x;
    int8_t y;
} Vector_t;

int8_t max(int8_t op1, int8_t op2)
{
    return op1 > op2 ? op1 : op2;
}

int8_t min(int8_t op1, int8_t op2)
{
    return op1 > op2 ? op2 : op1;
}

void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}

int8_t generate_ball_packet(int8_t row_position, int8_t y_direction) {
    int8_t packet = 0;
    packet += (BALL_PACKET << 7);
    packet += (row_position << 4);
    packet += (row_position << 2);
    return packet;
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
    ir_serial_init();

    Vector_t velocity = {1, 1};
    Vector_t ball_position = {LEDMAT_ROWS_NUM / 2, 1};
    Vector_t bar_position = {(LEDMAT_ROWS_NUM / 2) - (BAR_LENGTH / 2), LEDMAT_COLS_NUM - 1};
    int8_t toggle = 0;
    int8_t count = 0;

    int8_t order = -1;
    int8_t ret;

    int8_t isTurn = 0;

    // Handle who goes first
    while (order == -1) {
        navswitch_update ();
        int8_t data;


        // On button push, sends a data package 1: "I want to be first!"
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) { 
            for (int i= 0; i < 5; i++) {
                ir_serial_transmit(1);
            }
        }
        
        // Receives any messages sent by the other board
        ret = ir_serial_receive (&data);
        if (ret == IR_SERIAL_OK)
        {
            display_character ('1');
            switch (data) {
                // other board has already said they want to be first.
                case 1:
                    order = 2;
                    for (int i=0; i < 5; i++) {
                        ir_serial_transmit(2); // send an acknowledgement
                    }
                    isTurn = 0;
                    break;

                // other board has admitted defeat, this board is first
                case 2:
                    order = 1;
                    isTurn = 1;
                    break;
            }
        }
        tinygl_update();
    }






    while (1)
    {
        pacer_wait();

        if (isTurn == 1 && count == 10) {
            if (
                (ball_position.y == bar_position.y - 1) && 
                (0 <= (ball_position.x - bar_position.x)) && 
                ((ball_position.x - bar_position.x) < BAR_LENGTH)) {
                // should bounce if hits the paddle
                velocity.y *= -1;
            }
            if (ball_position.y == 4) {
                // hit the wall, dies
                velocity.x = 0;
                velocity.y = 0;
                ball_position.x = 3;
                ball_position.y = 0;
            } 
            if (ball_position.y < 0 && velocity.y == -1) {
                isTurn = 0;
                for (int i=0; i < 5; i++) {
                    int8_t packet = generate_ball_packet(ball_position.x, velocity.x+1);
                    ir_serial_transmit(packet); // send passover data
                }

            }
            if (ball_position.x == 0 || ball_position.x == 6) {
                // should bounce if it hits the sides
                velocity.x *= -1;
            }

            // update ball position
            ball_position.x += velocity.x;
            ball_position.y += velocity.y;
            count = 0;
        }

        int8_t recv = 0;
        ret = ir_serial_receive (&recv);
        if (ret == IR_SERIAL_OK)
        {
            if 
        }

        // handle bar movement
        navswitch_update ();
        if (navswitch_push_event_p(NAVSWITCH_NORTH)) { 
            bar_position.x = max(0, bar_position.x - 1);
        }
        else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
            bar_position.x = min(LEDMAT_COLS_NUM - BAR_LENGTH + 2, bar_position.x + 1);
        }

        // update ledmat
        if (toggle == 0) {
            ledmat_display_column(BAR_MASK << bar_position.x, bar_position.y);
            toggle = 1;
        } else {
            if (isTurn == 1) {
                ledmat_display_column(1 << ball_position.x, ball_position.y);
            }
            toggle = 0;
        }
        count++;
    }
}
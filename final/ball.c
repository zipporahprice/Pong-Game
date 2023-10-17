/**
 * @file ball.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module containing functions relating to movement, position, or display of the Pong ball object.
*/

#include "ball.h"
#include "bar.h"

Vector_t velocity;
Vector_t ball_position;

/**
 * Initializes the velocity and position of the ball object to its defaults.
*/
void ball_init()
{
    velocity = (Vector_t) {1, 1};
    ball_position = (Vector_t) {LEDMAT_ROWS_NUM / 2, 1};
}

/**
 * @return A Vector_t containing the position of the ball. 
*/
Vector_t ball_get_position()
{
    return ball_position;
}

/**
 * @return A Vector_t containing the velocity of the ball.
*/
Vector_t ball_get_velocity()
{
    return velocity;
}

/**
 * @param bar_position Position of the paddle to compare the ball to. 
 * @return A boolean flag checking if the ball is in contact with the paddle.
 * To be in contact, the ball must be on the line above the paddle, and be directly
 * adjacent to one of the paddle's LEDs. 
*/
bool ball_hits_paddle(Vector_t bar_position)
{
    return (ball_position.y == bar_position.y - 1) && 
            (0 <= (ball_position.x - bar_position.x)) && 
            ((ball_position.x - bar_position.x) < BAR_LENGTH);
}

/**
 * @return A boolean flag checking if the ball is in contact with the wall behind the paddle.
*/
bool ball_hits_back_wall()
{
    return ball_position.y >= BACK_ROW;
}

/**
 * @return A boolean flag checking if the ball is in contact with one of the sides of the game.
*/
bool ball_hits_side()
{
    return ball_position.x <= FIRST_ROW || ball_position.x >= LAST_ROW;
}

/**
 * @return A boolean flag checking if the ball is on the wall farthest from the paddle.
 * i.e. on the next movement, the ball would be moving into the opponent's board.
*/
bool ball_crosses_boundary()
{
    return ball_position.y < 0 && velocity.y == -1;
}

/**
 * Sets both the x and y of the ball's velocity vector to zero, effectively stopping it.
*/
void ball_stop()
{
    velocity.x = 0;
    velocity.y = 0;
}

/**
 * Reflects the x-component of the ball's velocity vector, effectively 'bouncing' it horizontally.
*/
void ball_bounce_x()
{
    velocity.x *= -1;
}

/**
 * Reflects the y-component of the ball's velocity vector, effectively 'bouncing' it vertically.
*/
void ball_bounce_y()
{
    velocity.y *= -1;
}

/**
 * Increases both of the ball's position components by it's respective stored velocity values.
*/
void ball_update_position() 
{
    ball_position.x += velocity.x;
    ball_position.y += velocity.y;
}

/**
 * Displays the ball on the screen.
*/
void ball_display()
{
    ledmat_display_column(1 << ball_position.x, ball_position.y);
}

/**
 * Sets the position of the ball.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void ball_set_position(int8_t x, int8_t y) {
    ball_position.x = x;
    ball_position.y = y;
}

/**
 * Sets the velocity of the ball. To avoid the ball jumping across the screen, 
 * the integer components should be kept between -1 and 1.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void ball_set_velocity(int8_t x, int8_t y) {
    velocity.x = x;
    velocity.y = y;
}

/**
 * @file ball.h
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module header providing functions and constants used by the ball module.
*/
#ifndef BALL_H
#define BALL_H

#include "ledmat.h"
#include "helper.h"

#define FIRST_ROW 0
#define LAST_ROW 6
#define BACK_ROW 4

/**
 * Initializes the velocity and position of the ball object to its defaults.
*/
void ball_init(void);

/**
 * @return A Vector_t containing the position of the ball. 
*/
Vector_t ball_get_position(void);

/**
 * @return A Vector_t containing the velocity of the ball.
*/
Vector_t ball_get_velocity(void);

/**
 * @param bar_position Position of the paddle to compare the ball to. 
 * @return A boolean flag checking if the ball is in contact with the paddle.
 * To be in contact, the ball must be on the line above the paddle, and be directly
 * adjacent to one of the paddle's LEDs. 
*/
bool ball_hits_paddle(Vector_t bar_position);

/**
 * Reflects the y-component of the ball's velocity vector, effectively 'bouncing' it vertically.
*/
void ball_bounce_y(void);

/**
 * @return A boolean flag checking if the ball is in contact with the wall behind the paddle.
*/
bool ball_hits_back_wall(void);

/**
 * Sets both the x and y of the ball's velocity vector to zero, effectively stopping it.
*/
void ball_stop(void);

/**
 * @return A boolean flag checking if the ball is in contact with one of the sides of the game.
*/
bool ball_hits_side(void);

/**
 * Reflects the x-component of the ball's velocity vector, effectively 'bouncing' it horizontally.
*/
void ball_bounce_x(void);

/**
 * @return A boolean flag checking if the ball is on the wall farthest from the paddle.
 * i.e. on the next movement, the ball would be moving into the opponent's board.
*/
bool ball_crosses_boundary(void);

/**
 * Increases both of the ball's position components by it's respective stored velocity values.
*/
void ball_update_position(void);

/**
 * Displays the ball on the screen.
*/
void ball_display(void);

/**
 * Sets the position of the ball.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void ball_set_position(int8_t x, int8_t y);

/**
 * Sets the velocity of the ball. To avoid the ball jumping across the screen, 
 * the integer components should be kept between -1 and 1.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void ball_set_velocity(int8_t x, int8_t y);

#endif
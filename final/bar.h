/**
 * @file bar.h
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module header providing functions and constants used by the bar module.
*/

#ifndef BAR_H
#define BAR_H

#include "helper.h"

#define BAR_LENGTH 3

/**
 * Initializes the position of the bar object to the center of the screen horizontally, in the row closest to the player.
*/
void bar_init(void);

/**
 * Sets the position of the bar.
 * @param x x-component of the position to set.
 * @param y y-component of the position to set.
*/
void bar_set_position(int8_t x, int8_t y);

/**
 * @return A Vector_t containing the position of the bar. 
*/
Vector_t bar_get_position(void);

/**
 * Gets the user input from the navswitch, and increments or decrements
 * the bar's position depending on which way it's moved.
*/
void bar_update(void);

/**
 * Displays the bar on the screen.
*/
void bar_display(void);

#endif
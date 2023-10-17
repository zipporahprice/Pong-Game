/**
 * @file displays.c
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module header providing functions and rate constants used by the displays module.
*/

#ifndef DISPLAYS_H
#define DISPLAYS_H

#define PACER_RATE 100
#define MESSAGE_RATE 2
#define TINYGL_RATE 500

/**
 * Initializes the tinygl display variables for the scrolling text 
*/
void init_display(void);

/**
 * Switches between displaying the bar and ball to avoid flickering.
 * @param isTurn Whether or not it is the player's turn. If it isn't, the ball shouldn't be rendered. 
*/
void toggle_display(int8_t isTurn);

/**
 * Display the scrolling text for the welcome screen.
*/
void welcome_screen(void);

/**
 * Display the scrolling text for when the player wins.
*/
void won_screen(void);

/**
 * Display the scrolling text for when the player loses.
*/
void lost_screen(void);

#endif
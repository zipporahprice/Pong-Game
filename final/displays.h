#ifndef DISPLAYS_H
#define DISPLAYS_H

#define PACER_RATE 100
#define MESSAGE_RATE 2
#define TINY_INIT 500

void init_display(void);

void toggle_display(int8_t isTurn);

void welcome_screen(void);

void won_screen(void);

void lost_screen(void);

#endif
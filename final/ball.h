#ifndef BALL_H
#define BALL_H
#define BAR_LENGTH 7
#define FIRST_ROW 0
#define LAST_ROW 6
#define BACK_ROW 4

#include "ledmat.h"
#include "helper.h"

void ball_init(void);

Vector_t get_ball_position(void);

Vector_t get_velocity(void);

bool hits_paddle(Vector_t bar_position);

void paddle_bounce(void);

bool hits_back_wall(void);

void wall_stop(void);

bool hits_side(void);

void wall_bounce(void);

bool crosses_boundary(void);

void update_ball_position(void);

void display_ball(void);

void set_ball_position(int8_t x, int8_t y);

void set_ball_velocity(int8_t x, int8_t y);

#endif
#ifndef BALL_H
#define BALL_H
#define BAR_LENGTH 7
#define FIRST_ROW 0
#define LAST_ROW 6
#define BACK_ROW 4

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "system.h"
#include "pio.h"
#include "ledmat.h"

typedef struct
{
    int8_t x;
    int8_t y;
} Vector_t;

void init_ball();

Vector_t get_ball_position();

Vector_t get_velocity();

bool hits_paddle(Vector_t bar_position);

void paddle_bounce();

bool hits_back_wall();

void wall_stop();

bool hits_side();

void wall_bounce();

bool crosses_boundary();

void update_ball_position();

#endif
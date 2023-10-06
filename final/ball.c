#include "ball.h"

Vector_t velocity;
Vector_t ball_position;

void init_ball()
{
    Vector_t velocity = {1, 1};
    Vector_t ball_position = {LEDMAT_ROWS_NUM / 2, 1};
}

Vector_t get_ball_position() {
    return ball_position;
}

Vector_t get_velocity() {
    return velocity;
}

bool hits_paddle(Vector_t bar_position)
{
    return (ball_position.y == bar_position.y - 1) && 
            (0 <= (ball_position.x - bar_position.x)) && 
            ((ball_position.x - bar_position.x) < BAR_LENGTH);
}

void paddle_bounce()
{
    velocity.y *= -1;
}

bool hits_back_wall()
{
    return ball_position.y == BACK_ROW;
}

void wall_stop()
{
    velocity.x = 0;
    velocity.y = 0;
    init_ball();
}

bool hits_side()
{
    return ball_position.x == FIRST_ROW || ball_position.x == LAST_ROW;
}

void wall_bounce()
{
    velocity.x *= -1;
}

bool crosses_boundary()
{
    return ball_position.y < 0 && velocity.y == -1;
}

void update_ball_position() 
{
    ball_position.x += velocity.x;
    ball_position.y += velocity.y;
}

void display_ball()
{
    ledmat_display_column(1 << ball_position.x, ball_position.y);
}

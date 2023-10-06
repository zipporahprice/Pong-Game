#ifndef BAR_H
#define BAR_H

#define BAR_LENGTH 7
#define BAR_MASK ((1 << BAR_LENGTH) - 1)

void bar_init(bar_length);

Vector_t bar_set_position(int8_t x, int8_t y);

void bar_update();

void bar_display();


#endif
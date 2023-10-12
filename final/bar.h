#ifndef BAR_H
#define BAR_H

#define BAR_LENGTH 3
#define BAR_MASK ((1 << BAR_LENGTH) - 1)

void bar_init(void);

void bar_set_position(int8_t x, int8_t y);

Vector_t bar_get_position(void);

void bar_update(void);

void bar_display(void);


#endif
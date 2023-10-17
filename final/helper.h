/**
 * @file helper.h
 * @author Zipporah Price
 * @author Jake Dalton
 * @date 17 Oct 2023
 * @brief Module header containing functions and structures to allow for easy data manipulation.
*/

#ifndef HELPER_H
#define HELPER_H

#include "system.h"

#define TRUE 1
#define FALSE 0

typedef struct
{
    int8_t x;
    int8_t y;
} Vector_t;

/**
 * @param op1 The first operand to compare.
 * @param op2 The second operand to compare.
 * @return The maximum of the two operands.
*/
int8_t max(int8_t op1, int8_t op2);

/**
 * @param op1 The first operand to compare.
 * @param op2 The second operand to compare.
 * @return The minimum of the two operands.
*/
int8_t min(int8_t op1, int8_t op2);

/**
 * Restricts a value to between two other given values.
 * If value is lower than the minimum, minimum will be returned.
 * If value is higher than maximum, maximum will be returned.
 * Assumes minimum <= maximum.
 * @param data The value to be limited
 * @param minimum The lower bound.
 * @param maximum The upper bound.
 * @return The value given, clamped between the minimum and maximum.
*/
int8_t clamp(int8_t data, int8_t minimum, int8_t maximum);

#endif
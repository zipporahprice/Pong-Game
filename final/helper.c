
#include "system.h"
#include "helper.h"

int8_t max(int8_t op1, int8_t op2)
{
    return op1 > op2 ? op1 : op2;
}

int8_t min(int8_t op1, int8_t op2)
{
    return op1 > op2 ? op2 : op1;
}

int8_t clamp(int8_t data, int8_t minimum, int8_t maximum) {
    return min(max(data, minimum), maximum);
}
#include "RTE_Components.h"
#include  CMSIS_device_header
#define MOTOR_H

void initMotor(void);
void setLeftPower(uint8_t);
void setRightPower(uint8_t);
void stop(void);
void forward(void);
void reverse(void);
void turnLeft(void);
void turnRight(void);

#endif
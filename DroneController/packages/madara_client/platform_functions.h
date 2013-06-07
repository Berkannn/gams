/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _PLATFORM_FUNCTIONS_H
#define _PLATFORM_FUNCTIONS_H

bool init_platform();

bool init_control_functions();

void takeoff();
void land();

void move_up();
void move_down();
void move_left();
void move_right();
void move_forward();
void move_backward();

bool init_sensor_functions();

double human_detected();

#endif
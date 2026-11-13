#ifndef __COMMAND_H
#define __COMMAND_H

typedef enum
{
    vehicle_mode_undefined = 0,
    vehicle_mode_stop,
    vehicle_mode_turn_left,
    vehicle_mode_turn_right,
    vehicle_mode_trans_left,
    vehicle_mode_trans_right,
    vehicle_mode_max
}vehicle_mode;

vehicle_mode vehicle_command_detection(void);
void get_command_value(vehicle_mode key, char* command_str);

#endif

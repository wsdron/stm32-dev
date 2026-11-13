#include "bsp_usart.h"
#include "command.h"
#include <string.h>

const char command[vehicle_mode_max][32] = {"undefined", "stop", "turnleft", "turnright", "transleft", "transright"};

vehicle_mode vehicle_command_detection(void)
{
    vehicle_mode ret_val = vehicle_mode_undefined;
    bool command_detected = false;

    for (uint8_t i=0u; i<vehicle_mode_max; i++)
    {
        if (!command_detected)
        {
            if (uart_check_contains_flash(command[i], strlen(command[i]) - 1))
            {
                ret_val = i;
                command_detected = true;
            }
        }
    }

    return ret_val;
}

void get_command_value(vehicle_mode key, char* command_str)
{
    strcpy(command_str, command[key]);
}

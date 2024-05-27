/*
Structure of the messages used by this application to update the state of the sliders
*/

#pragma once

#define PLUS_SIGN 43
#define MINUS_SIGN 45
#define X_AXIS 71
#define Y_AXIS 66

struct Message {
    char direction; // May be either B (for Y axis) or G (for X axis)
    char verse; // + or -
    int value; // Intensity
};

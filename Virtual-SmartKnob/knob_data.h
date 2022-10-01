#ifndef KNOB_DATA_H
#define KNOB_DATA_H

#include <cstdint>

struct KnobConfig {
    int32_t num_positions;
    int32_t position;
    float position_width_radians;
    float detent_strength_unit;
    float endstop_strength_unit;
    float snap_point;
    char descriptor[50];
};

struct KnobState {
    int32_t current_position;
    float sub_position_unit;
    KnobConfig config;
};

#endif // KNOB_DATA_H

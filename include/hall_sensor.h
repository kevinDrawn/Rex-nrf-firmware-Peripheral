/*
 * hall_sensor.h
 *
 *  Created on: Nov 20, 2024
 *      Author: qoxmf
 */
#pragma once

#ifndef INC_HALL_SENSOR_H_
#define INC_HALL_SENSOR_H_
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <hall_sensor_table.h>

// Hall Sensor - Distance - 1.75 mm
#define Vmax_HALL_AP 4096.0   // ADC MAX
#define Vmin_HALL_AP 120.0   // ADC MIN

#define PI 3.14159265358979323846
#define NUM_SAMPLES 500
#define HALL_OFFSET_RIGHT 0.0
#define HALL_OFFSET_LEFT  0.0
#define Hall_Bias 0.0
#define TARGET_ZERO_ANGLE_Ap 0.0  // Desired zero position in degrees
#define TARGET_ZERO_ANGLE_An 90.0  // Desired zero position in degrees
#define HALL_ADC_MAX 4096

#define LEFT_EXOSHOE_CHAR   'H'
#define RIGHT_EXOSHOE_CHAR  'S'

extern float Vamp_left;
extern float Vamp_right;

extern float Ap_avg;
extern float Ap_sum;
extern int16_t sample_count_hall;

extern float An;
extern float Ap;

float Norm_func(uint16_t measured_value, char exo_type_char);
float rad2deg(float radian);
float deg2rad(float degree);
float Norm_ADC(uint16_t adc, char exo_type_char);
void calculate_MB(float A_iA, float A_iB, float A_cA, float A_cB, float *M, float *B);
void process_ADC_value(float Ap);
float calibrate_angle_char(uint16_t adc, char exo_type_char);
void hall_lut_init(void);

extern int16_t hall_lut_left[HALL_ADC_MAX];
extern int16_t hall_lut_right[HALL_ADC_MAX];

#endif /* INC_HALL_SENSOR_H_ */

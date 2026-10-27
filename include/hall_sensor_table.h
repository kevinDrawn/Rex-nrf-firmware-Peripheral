#ifndef HALL_SENSOR_TABLE_H
#define HALL_SENSOR_TABLE_H

typedef struct {
    char id;                  // 모델 문자
    const float* measured;
    const float* hall;
    int size;
    int Vmax_HALL_ADC;
    int Vmin_HALL_ADC;
} HallModelTable;

extern const HallModelTable hall_models[];
extern const int hall_model_count;

#endif
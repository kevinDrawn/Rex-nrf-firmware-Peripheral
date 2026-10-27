#include "hall_sensor.h"

float Ap_avg = 0;
float Ap_sum = 0;
int16_t sample_count_hall = 0;
float peak_q=0.0;

float An;
float Ap;

int16_t hall_lut_left[HALL_ADC_MAX];
int16_t hall_lut_right[HALL_ADC_MAX];

void hall_lut_init(void)
{
    for (uint16_t adc = 0; adc < HALL_ADC_MAX; adc++) {
        hall_lut_left[adc] =
            (int16_t)(calibrate_angle_char(adc, LEFT_EXOSHOE_CHAR) * 100.0f);

        hall_lut_right[adc] =
            (int16_t)(calibrate_angle_char(adc, RIGHT_EXOSHOE_CHAR) * 100.0f);
    }
}

static const HallModelTable* find_model(char id)
{
    for(int i = 0; i < hall_model_count; i++)
    {
        if(hall_models[i].id == id)
            return &hall_models[i];
    }
    return NULL;
}

float Norm_func(uint16_t measured_value, char exo_type_char) {
    float Vmin, Vamp;

    const HallModelTable* model = find_model(exo_type_char);
    if(model == NULL)
    {
        return NAN;
    }

    int MAX_ADC_HALL                     = model->Vmax_HALL_ADC;
    int MIN_ADC_HALL                     = model->Vmin_HALL_ADC;

    Vmin = MIN_ADC_HALL;
    Vamp = (MAX_ADC_HALL - MIN_ADC_HALL) / 2.0f;
    return ((float)measured_value - Vmin - Vamp) / Vamp;
}

float Norm_ADC(uint16_t adc, char exo_type_char) {
    float Norm = Norm_func(adc, exo_type_char);
    Norm = fmaxf(-1.0f, fminf(1.0f, Norm));
    float radian = asin(Norm);
    float Ap = rad2deg(radian);
    return Ap;
}



void calculate_MB(float A_iA, float A_iB, float A_cA, float A_cB, float *M, float *B) {
    *M = (A_iA - A_iB) / (A_cA - A_cB);
    *B = A_iA - (A_cA * (*M));
}

void process_ADC_value(float Ap) {
    Ap_sum += Ap;
    sample_count_hall++;

    if (sample_count_hall == NUM_SAMPLES) {
        Ap_avg = Ap_sum / NUM_SAMPLES;
        Ap_sum = 0;
        sample_count_hall = 0;
    }
}

float calibrate_angle_char(uint16_t adc, char exo_type_char)
{
    float A_iA, A_iB, A_cA, A_cB;
    float M, B;

    const HallModelTable* model = find_model(exo_type_char);
    if(model == NULL)
    {
        return NAN;
    }

    float Ap = Norm_ADC(adc, exo_type_char);

    const float* measured_angles = model->measured;
    const float* hall_angles     = model->hall;
    int size                     = model->size;

    int is_descending = (hall_angles[0] > hall_angles[size - 1]);

    // 범위 제한
    if(is_descending)
    {
        if (Ap > hall_angles[0])
            Ap = hall_angles[0];
        else if (Ap < hall_angles[size - 1])
            Ap = hall_angles[size - 1];
    }
    else
    {
        if (Ap < hall_angles[0])
            Ap = hall_angles[0];
        else if (Ap > hall_angles[size - 1])
            Ap = hall_angles[size - 1];
    }

    // 이진 탐색
    int left = 0;
    int right_idx = size - 1;

    while (left < right_idx - 1)
    {
        int mid = (left + right_idx) / 2;

        int condition = is_descending ?
                        (Ap > hall_angles[mid]) :
                        (Ap < hall_angles[mid]);

        if(condition)
            right_idx = mid;
        else
            left = mid;
    }

    // 보간
    A_iA = measured_angles[left];
    A_iB = measured_angles[right_idx];
    A_cA = hall_angles[left];
    A_cB = hall_angles[right_idx];

    calculate_MB(A_iA, A_iB, A_cA, A_cB, &M, &B);

    return (M * Ap) + B + 90;
}

float rad2deg(float radian) {
    return radian * (180.0f / PI);
}

float deg2rad(float degree){
	return degree*PI/180;
}
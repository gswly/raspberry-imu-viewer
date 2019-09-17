
#include <stdio.h>

#include "error.h"
#include "vector.h"
#include "imu.h"
#include "gyro_bias.h"

#define SAMPLE_COUNT 500

void gyro_bias_init(vector* gyro_bias, imut* imu) {
    imu_output io;
    for(int i = 0; i < SAMPLE_COUNT; i++) {
        imu_read(imu, &io);
        gyro_bias->x += io.gyro.x;
        gyro_bias->y += io.gyro.y;
        gyro_bias->z += io.gyro.z;
    }
    gyro_bias->x /= SAMPLE_COUNT;
    gyro_bias->y /= SAMPLE_COUNT;
    gyro_bias->z /= SAMPLE_COUNT;
    printf("gyro bias: %f, %f, %f\n", gyro_bias->x, gyro_bias->y, gyro_bias->z);
}

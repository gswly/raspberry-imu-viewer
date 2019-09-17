
#pragma once

typedef enum {
    INVENSENSE_ACC_2G,
    INVENSENSE_ACC_4G,
    INVENSENSE_ACC_8G,
    INVENSENSE_ACC_16G,
} invensense_acc_range;

typedef enum {
    INVENSENSE_GYRO_250,
    INVENSENSE_GYRO_500,
    INVENSENSE_GYRO_1000,
    INVENSENSE_GYRO_2000,
} invensense_gyro_range;

void* imu_invensense_init(int fd, invensense_acc_range acc_range,
    invensense_gyro_range gyro_range);
void imu_invensense_read(void* obj, imu_output* r);

#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "error.h"
#include "vector.h"
#include "imu_invensense.h"
#include "imu.h"

typedef void (*sensor_read_func)(void*, imu_output*);

typedef struct {
    void* sensor;
    sensor_read_func sensor_read;
} _objt;

static error* find_sensor(_objt* _obj) {
    int fd = open("/dev/i2c-1", O_RDWR);
    if(fd < 0) {
        return "unable to open device /dev/i2c-1";
    }

    for(uint8_t address = 0x68; address <= 0x69; address++) {
        int res = ioctl(fd, I2C_SLAVE, address);
        if(res != 0) {
            continue;
        }

        uint8_t cmd = 0x75;
        res = write(fd, &cmd, 1);
        if(res != 1) {
            continue;
        }

        uint8_t who_am_i;
        res = read(fd, &who_am_i, 1);
        if(res != 1) {
            continue;
        }

        switch(who_am_i) {
        case 0x68:
            printf("found MPU6000/MPU6050 with address 0x%.2x\n", address);
            imu_invensense_init(&_obj->sensor, fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj->sensor_read = imu_invensense_read;
            return NULL;

        case 0x70:
            printf("found MPU6500 with address 0x%.2x\n", address);
            imu_invensense_init(&_obj->sensor, fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj->sensor_read = imu_invensense_read;
            return NULL;

        case 0x11:
            printf("found ICM20600 with address 0x%.2x\n", address);
            imu_invensense_init(&_obj->sensor, fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj->sensor_read = imu_invensense_read;
            return NULL;

        case 0xAC:
            printf("found ICM20601 with address 0x%.2x\n", address);
            imu_invensense_init(&_obj->sensor, fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj->sensor_read = imu_invensense_read;
            return NULL;

        case 0x12:
            printf("found ICM20602 with address 0x%.2x\n", address);
            imu_invensense_init(&_obj->sensor, fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj->sensor_read = imu_invensense_read;
            return NULL;
        }
    }

    close(fd);
    return "no IMU sensor found";
}

error* imu_init(imut** pobj) {
    _objt* _obj = malloc(sizeof(_objt));

    error* err = find_sensor(_obj);
    if(err != NULL) {
        free(_obj);
        return err;
    }

    *pobj = _obj;
    return NULL;
}

void imu_read(imut* obj, imu_output* r) {
    _objt* _obj = (_objt*)obj;

    _obj->sensor_read(_obj->sensor, r);
}

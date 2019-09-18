
#include <stdlib.h>
#include <math.h>

#include "error.h"
#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_gyro.h"

typedef struct {
    vector gyro_bias;
    matrix align;
    double prev_roll;
    double prev_pitch;
    double prev_yaw;
} _objt;

error* est_euler_gyro_init(est_euler_gyrot** pobj, imut* imu) {
    _objt* _obj = malloc(sizeof(_objt));

    error* err = gyro_bias_init(&_obj->gyro_bias, imu);
    if(err != NULL) {
        free(_obj);
        return err;
    }

    err = align_dcm_init(&_obj->align, imu);
    if(err != NULL) {
        free(_obj);
        return err;
    }

    _obj->prev_roll = 0;
    _obj->prev_pitch = 0;
    _obj->prev_yaw = 0;

    *pobj = _obj;
    return NULL;
}

void est_euler_gyro_do(est_euler_gyrot* obj, const imu_output* io, double dt,
    estimator_output* eo) {
    _objt* _obj = (_objt*)obj;

    vector tuned_gyro;
    vector_diff(&io->gyro, &_obj->gyro_bias, &tuned_gyro);

    vector aligned_gyro;
    matrix_multiply(&_obj->align, &tuned_gyro, &aligned_gyro);

    _obj->prev_roll = _obj->prev_roll + aligned_gyro.x*dt;
    _obj->prev_pitch = _obj->prev_pitch + aligned_gyro.y*dt;
    _obj->prev_yaw = _obj->prev_yaw + aligned_gyro.z*dt;

    eo->roll = _obj->prev_roll;
    eo->pitch = _obj->prev_pitch;
    //eo->yaw = _obj->prev_yaw;
    // do not use yaw, such that the result can be compared with other estimators
    eo->yaw = 0;
}


#include <string.h>
#include <math.h>

#include "error.h"
#include "vector.h"
#include "imu.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_gyrounalign.h"

typedef struct {
    vector gyro_bias;
    double prev_roll;
    double prev_pitch;
    double prev_yaw;
} _objt;

error* est_euler_gyrounalign_init(est_euler_gyrounalignt** pobj, imut* imu) {
    _objt* _obj = malloc(sizeof(_objt));

    error* err = gyro_bias_init(&_obj->gyro_bias, imu);
    if(err != NULL) {
        return err;
    }

    _obj->prev_roll = 0;
    _obj->prev_pitch = 0;
    _obj->prev_yaw = 0;

    *pobj = _obj;
    return NULL;
}

void est_euler_gyrounalign_do(est_euler_gyrounalignt* obj, const imu_output* io,
    double dt, estimator_output* eo) {
    _objt* _obj = (_objt*)obj;

    vector tuned_gyro;
    vector_diff(&io->gyro, &_obj->gyro_bias, &tuned_gyro);

    _obj->prev_roll = _obj->prev_roll + tuned_gyro.x*dt;
    _obj->prev_pitch = _obj->prev_pitch + tuned_gyro.y*dt;
    _obj->prev_yaw = _obj->prev_yaw + tuned_gyro.z*dt;

    eo->roll = _obj->prev_roll;
    eo->pitch = _obj->prev_pitch;
    // eo->yaw = _obj->prev_yaw;
    // do not use yaw, such that the result can be compared with other estimators
    eo->yaw = 0;
}

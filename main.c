
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "error.h"
#include "vector.h"
#include "imu.h"
#include "est.h"
#include "viewer.h"
#include "est_euler_acc.h"
#include "est_euler_gyro.h"
#include "est_euler_gyrounalign.h"
#include "est_euler_compl.h"
#include "est_dcm_compl.h"

static uint32_t clock_usec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int main() {
    // disable stdout buffering
    setbuf(stdout, NULL);

    imut* imu;
    error* err = imu_init(&imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    est_euler_acct* est_euler_acc;
    err = est_euler_acc_init(&est_euler_acc, imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    est_euler_gyrot* est_euler_gyro;
    err = est_euler_gyro_init(&est_euler_gyro, imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    est_euler_gyrounalignt* est_euler_gyro_unalign;
    err = est_euler_gyrounalign_init(&est_euler_gyro_unalign, imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    est_euler_complt* est_euler_compl;
    err = est_euler_compl_init(&est_euler_compl, imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    est_dcm_complt* est_dcm_compl;
    err = est_dcm_compl_init(&est_dcm_compl, imu);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    visualizert* visualizer;
    err = visualizer_init(&visualizer);
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }

    imu_output io;
    uint32_t read_count = 0;
    uint32_t last_report = clock_usec();
    uint32_t prev = last_report;
    estimator_output eo;

    while(1) {
        imu_read(imu, &io);
        read_count++;

        uint32_t now = clock_usec();
        double dt = (double)(now - prev) / 1000000;
        prev = now;

        visualizer_draw_start(visualizer);

        est_euler_acc_do(est_euler_acc, &io, dt, &eo);
        visualizer_draw_estimate(visualizer, 0, &eo);

        est_euler_gyro_do(est_euler_gyro, &io, dt, &eo);
        visualizer_draw_estimate(visualizer, 1, &eo);

        est_euler_gyrounalign_do(est_euler_gyro_unalign, &io, dt, &eo);
        visualizer_draw_estimate(visualizer, 2, &eo);

        est_euler_compl_do(est_euler_compl, &io, dt, &eo);
        visualizer_draw_estimate(visualizer, 3, &eo);

        est_dcm_compl_do(est_dcm_compl, &io, dt, &eo);
        visualizer_draw_estimate(visualizer, 4, &eo);

        visualizer_draw_end(visualizer);

        if((now - last_report) >= 1000000) {
            printf("read per sec: %d\n", read_count);
            printf("gyro x,y,z: %f, %f, %f\n", io.gyro.x, io.gyro.y, io.gyro.z);
            printf("acc x,y,z: %f, %f, %f\n", io.acc.x, io.acc.y, io.acc.z);
            printf("\n");
            last_report = now;
            read_count = 0;
        }
    }

    return 0;
}

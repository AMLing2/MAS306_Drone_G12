#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define GYRO_ADDR   0x68

int main(int argc, char *argv[])
{
    int i2c_handle;
    char buf[6];
    int16_t x, y, z;

    if (gpioInitialise() < 0) {
        printf("Unable to initialize gpio library.\n");
        return EXIT_FAILURE;
    }

    i2c_handle = i2cOpen(1, GYRO_ADDR, 0);

    // Set gyroscope to normal mode
    i2cWriteByteData(i2c_handle, 0x7E, 0x05);

    while (1) {
        // Read gyroscope data
        i2cWriteByte(i2c_handle, 0x02);   // set register pointer to 0x02
        i2cReadDevice(i2c_handle, buf, 6); // read 6 bytes starting from register pointer
        x = (int16_t)((buf[1] << 8) | buf[0]);   // combine high byte and low byte for X data
        y = (int16_t)((buf[3] << 8) | buf[2]);   // combine high byte and low byte for Y data
        z = (int16_t)((buf[5] << 8) | buf[4]);   // combine high byte and low byte for Z data
        printf("Gyroscope (raw): X=%+d Y=%+d Z=%+d\n", x, y, z);

        // Wait for 100 ms before reading again
        usleep(100000);
    }

    i2cClose(i2c_handle);
    gpioTerminate();

    return EXIT_SUCCESS;
}

#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define SPI_CHANNEL 1
#define SPI_SPEED   1000000
#define GYRO_CS_PIN 16

int main(int argc, char *argv[])
{
    int spi_handle;
    char tx_buf[2], rx_buf[6];
    int16_t x, y, z;

    if (gpioInitialise() < 0) {
        printf("Unable to initialize gpio library.\n");
        return EXIT_FAILURE;
    }

    gpioSetMode(GYRO_CS_PIN, PI_OUTPUT);
    gpioSetPullUpDown(GYRO_CS_PIN, PI_PUD_UP);

    spi_handle = spiOpen(SPI_CHANNEL, SPI_SPEED, 0);

    // Set gyroscope to normal mode
    gpioWrite(GYRO_CS_PIN, 0);
    tx_buf[0] = 0x7E;
    tx_buf[1] = 0x05;
    spiXfer(spi_handle, tx_buf, rx_buf, 2);
    gpioWrite(GYRO_CS_PIN, 1);

    while (1) {
        // Read gyroscope data
        gpioWrite(GYRO_CS_PIN, 0);
        tx_buf[0] = 0x02 | 0x80;    // MSB=1 for reading, LSB=0x02 for gyroscope data
        tx_buf[1] = 0;
        spiXfer(spi_handle, tx_buf, rx_buf, 6);
        gpioWrite(GYRO_CS_PIN, 1);

        // Extract X, Y, Z values from received data
        x = (int16_t)((rx_buf[1] << 8) | rx_buf[0]);
        y = (int16_t)((rx_buf[3] << 8) | rx_buf[2]);
        z = (int16_t)((rx_buf[5] << 8) | rx_buf[4]);

        printf("Gyroscope (raw): X=%+d Y=%+d Z=%+d\n", x, y, z);

        // Wait for 100 ms before reading again
        usleep(100000);
    }

    spiClose(spi_handle);
    gpioTerminate();

    return EXIT_SUCCESS;
}

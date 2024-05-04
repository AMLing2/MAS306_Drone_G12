import spidev
import time

GYRO_CS = 13

spi = spidev.SpiDev()
spi.open(0, 1)
spi.max_speed_hz = 1000000  # Set SPI clock speed to 1 MHz


def read_gyroscope():
    # Set gyroscope to normal mode
    spi.xfer([0x7E, 0x05])#, cs_change=True)

    while True:
        # Read gyroscope data
        spi.xfer([0x02 | 0x80])  # set register pointer to 0x02
        buf = spi.readbytes(6)  # read 6 bytes starting from register pointer
        x = (buf[1] << 8) | buf[0]  # combine high byte and low byte for X data
        y = (buf[3] << 8) | buf[2]  # combine high byte and low byte for Y data
        z = (buf[5] << 8) | buf[4]  # combine high byte and low byte for Z data
        print(f"Gyroscope (raw): X={x} Y={y} Z={z}")

        # Wait for 100 ms before reading again
        time.sleep(0.1)


if __name__ == '__main__':
    read_gyroscope()

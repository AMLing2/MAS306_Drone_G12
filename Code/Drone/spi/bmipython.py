import spidev
spi = spidev.SpiDev()
spi.open(1, 0)
spi.max_speed_hz = 1000000
spi.mode = 0b11
spi.writebytes([0x00])
spi.readbytes(1)
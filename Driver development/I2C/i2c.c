#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#define I2C_ADDR 0x48 // I2C address of temperature sensor
#define I2C_BUS "/dev/i2c-1" // I2C bus device file

int main(int argc, char *argv[])
{
    int fd;    char buf[2];
    float temperature;
    // Open the I2C bus device file
    fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // Set the I2C slave device address
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("ioctl");        exit(EXIT_FAILURE);
    }
    // Read the temperature data from the sensor
    buf[0] = 0x00; // Temperature register address
    if (write(fd, buf, 1) != 1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buf, 2) != 2) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    // Convert the temperature data to Celsius
    temperature = (float)((buf[0] << 8) | buf[1]) / 256.0;
    // Print the temperature
    printf("Temperature: %.2f C\n", temperature);
    // Close the I2C bus device file
    close(fd);
    return 0;
}
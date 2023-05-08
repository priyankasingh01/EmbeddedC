#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define GPIO_BASE 0x3F200000 // Base address of GPIO controller
#define GPIO_SIZE 4096 // Size of GPIO controller memory map
#define GPIO_PIN 4 // GPIO pin number for LED

int main(int argc, char *argv[])
{
    int fd;
    void *gpio_map;
    volatile unsigned *gpio;
    int value;

    // Open the memory map of the GPIO controller
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    gpio_map = mmap(NULL, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    if (gpio_map == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    gpio = (volatile unsigned *)gpio_map;

    // Set the GPIO pin as output
    *(gpio + ((GPIO_PIN)/10)) &= ~(7<<(((GPIO_PIN)%10)*3));
    *(gpio + ((GPIO_PIN)/10)) |= 1<<(((GPIO_PIN)%10)*3);

    // Toggle the LED on and off
    while (1) {
        value = *(gpio + 7); // Read the current value of the GPIO port
        if (value & (1<<GPIO_PIN)) {
            *(gpio + 10) = 1<<GPIO_PIN; // Clear the GPIO pin
        } else {
            *(gpio + 7) = 1<<GPIO_PIN; // Set the GPIO pin
        }
        usleep(500000); // Delay for 500 ms
    }

    // Close the memory map of the GPIO controller
    munmap(gpio_map, GPIO_SIZE);
    close(fd);

    return 0;
}
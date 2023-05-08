#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/mmc/ioctl.h>
#define SDMC_DEVICE "/dev/mmcblk0" // SDMC device node
#define SDMC_BLOCK_SIZE 512 // SDMC block size in bytes
int main(int argc, char *argv[])
{
    int fd;
    char buffer[SDMC_BLOCK_SIZE];
    struct mmc_ioc_cmd command;
    memset(&command, 0, sizeof(command));
    // Open the SDMC device node
    fd = open(SDMC_DEVICE, O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // Set the block size
    command.opcode = MMC_SET_BLOCKLEN;
    command.arg = SDMC_BLOCK_SIZE;
    if (ioctl(fd, MMC_IOC_CMD, &command) < 0) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    // Write data to the SD card
    command.opcode = MMC_WRITE_BLOCK;
    command.flags = MMC_RSP_SPI_S2 | MMC_RSP_R1B | MMC_CMD_ADTC;
    command.arg = 0;
    command.blksz = SDMC_BLOCK_SIZE;
    command.blocks = 1;
    memcpy(buffer, "Hello, world!", 14);
    if (write(fd, buffer, SDMC_BLOCK_SIZE) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, MMC_IOC_CMD, &command) < 0) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    // Read data from the SD card
    command.opcode = MMC_READ_SINGLE_BLOCK;
    command.flags = MMC_RSP_SPI_S2 | MMC_RSP_R1 | MMC_CMD_ADTC;
    command.arg = 0;
    command.blksz = SDMC_BLOCK_SIZE;
    command.blocks = 1;
    if (ioctl(fd, MMC_IOC_CMD, &command) < 0) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buffer, SDMC_BLOCK_SIZE) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Data read from SD card: %s\n", buffer);
    // Close the SDMC device node
    close(fd);
    return 0;
}
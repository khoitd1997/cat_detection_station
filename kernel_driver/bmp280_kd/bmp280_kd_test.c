#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DRIVER_PATH "/dev/bmp280_kd_1"

int main() {
  int fd;
  int ret = 0;

  printf("Starting test of bmp280\n");

  printf("Opening device file bmp280\n");
  fd = open(DRIVER_PATH, O_RDONLY);
  if (fd == -1) {
    printf("Error opening file %s\n", strerror(errno));
    return -1;
  }
  //   fprintf(fp, "This is testing for fprintf...\n");
  //   fputs("This is testing for fputs...\n", fp);

  printf("Closing device file bmp280\n");
  ret = close(fd);
  if (ret == -1) {
    printf("Error closing file %s\n", strerror(errno));
    return -1;
  }

  return 0;
}
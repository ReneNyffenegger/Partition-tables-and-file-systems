#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "structs/BlockDevice.h"

void openBlockDevice(const char* path, BlockDevice* b) {
  //
  // Modelled after libparted: linux_open() - libparted/arch/linux.c
  //

  b->fd = open(path, O_RDONLY);

  if (b->fd == -1) {
    printf("b->fd = %d\n", b->fd);
    exit(-1);
  }

}

int main() {

  BlockDevice blockDevice;
  openBlockDevice("/dev/sda", &blockDevice);

}

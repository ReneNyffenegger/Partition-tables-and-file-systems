#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

#include "structs/BlockDevice.h"

#define SCSI_DISK0_MAJOR         8
#define SCSI_CDROM_MAJOR         11
#define SCSI_DISK1_MAJOR         65
#define SCSI_DISK7_MAJOR         71
#define SCSI_DISK8_MAJOR        128
#define SCSI_DISK15_MAJOR       135

// libparted/arch/linux.c: 
#define SCSI_BLK_MAJOR(M) (                                             \
                (M) == SCSI_DISK0_MAJOR                                 \
                || (M) == SCSI_CDROM_MAJOR                              \
                || ((M) >= SCSI_DISK1_MAJOR && (M) <= SCSI_DISK7_MAJOR) \
                || ((M) >= SCSI_DISK8_MAJOR && (M) <= SCSI_DISK15_MAJOR))

void openBlockDevice(BlockDevice* b) {
 //
 // Modelled after libparted: linux_open() - libparted/arch/linux.c
 //

    b->fd   = open(b->path, O_RDONLY);

    if (b->fd == -1) {
      printf("b->fd = %d\n", b->fd);
      exit(-1);
    }

}

void stat_(BlockDevice* b) {

    if (stat(b->path, &b->stat_)) {
      printf("Could not stat %s\n", b->path);
      exit(-1);
    }

    if (! S_ISBLK(b->stat_.st_mode)) {
      printf("Not a block device\n");
      exit(-1);
    }

 //
 // major() and minor() are defined in <sys/sysmacros.h>
 // 
    b->major = major(b->stat_.st_rdev);
    b->minor = minor(b->stat_.st_rdev);
}

void determineDeviceType(BlockDevice* b) {
 //
 // See libparted/arch/linux.c - _device_probe_type
 //
    if (SCSI_BLK_MAJOR (b->major) && (b->minor % 0x10 == 0)) {
      printf("SCSI\n");
    }
    else if (b->major ==  3 ||
             b->major == 22 ||
             b->major == 33 ||
             b->major == 34 ||
             b->major == 56 ||
             b->major == 57) {
 
       if (b->minor & 0x40) {
         printf("IDE");
       }
       else {
         printf("almost IDE");
       }
     }
     else {
        printf("TODO ?\n");
     }
}

int main() {

  BlockDevice blockDevice;
  blockDevice.path = "/dev/sda";

  openBlockDevice    (&blockDevice);

  stat_(&blockDevice);

  determineDeviceType(&blockDevice);

}

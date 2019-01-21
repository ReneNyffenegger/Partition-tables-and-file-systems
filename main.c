// #include <stdio.h>
// #include <fcntl.h>
#include <stdlib.h>
// #include <stdint.h>
#include <sys/sysmacros.h>

#include "structs/BlockDevice.h"
#include "structs/MSDosPartitionTable.h"

#include <stdio.h>

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

    b->f    = fopen(b->path, "r");

    if (!b->f) exit(1);
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

int isSCSI(BlockDevice* b) {
 //
 // See libparted/arch/linux.c - _device_probe_type
 //
    return SCSI_BLK_MAJOR (b->major) && (b->minor % 0x10 == 0);
}
int isIDE(BlockDevice* b) {
 //
 // See libparted/arch/linux.c - _device_probe_type
 //

    if (b->major ==  3 ||
        b->major == 22 ||
        b->major == 33 ||
        b->major == 34 ||
        b->major == 56 ||
        b->major == 57) {
 
       if (b->minor & 0x40) {
         return 1;
       }
       else {
         printf("almost IDE");
       }
    }
    return 0;
}

int isMSDosPartitionTable(BlockDevice *b, struct MSDosPartitionTable *partitionTable) {

 //
 // Read first sector
 //
    fseek(b->f, 0, SEEK_SET);
    fread(partitionTable, sizeof(*partitionTable), 1, b->f);

    return partitionTable->magic[0] == 0x55 && 
           partitionTable->magic[1] == 0xaa;

//  printf("low: %x, high: %x\n", partitionTable->magic[0], partitionTable->magic[1]);

}

void dissaemble_msdos_boot_code(struct MSDosPartitionTable *t) {

  FILE *f = fopen("msdos_boot_code.bin", "w");
  if (!f) exit(2);

  fwrite(t->boot_code, sizeof(t->boot_code), 1, f);
  fclose(f);

  system("objdump -D -Mintel,x86-64 -b binary -m i386 msdos_boot_code.bin");

}

int main() {

  BlockDevice blockDevice;
  blockDevice.path = "/dev/sda";

  openBlockDevice    (&blockDevice);
  stat_              (&blockDevice);
  
  if (isSCSI(&blockDevice)) {
     printf("SCSI\n");
  }
  else if (isIDE(&blockDevice)) {
     printf("IDE\n");
  }

  struct MSDosPartitionTable msdosPartitionTable;
  if (isMSDosPartitionTable(&blockDevice, &msdosPartitionTable)) {
    printf("Device has a MSDos partition table.\n");

    dissaemble_msdos_boot_code(&msdosPartitionTable);

  }

}

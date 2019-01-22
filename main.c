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

void disassemble_msdos_boot_code(struct MSDosPartitionTable *t) {

  FILE *f = fopen("msdos_boot_code.bin", "w");
  if (!f) exit(2);

  fwrite(t->boot_code, sizeof(t->boot_code), 1, f);
  fclose(f);

//system("objdump -D -b binary -m i386  -Mintel,x86-64 -z msdos_boot_code.bin");
//system("objdump -D -b binary -m i386  -Mintel           msdos_boot_code.bin");
  system("objdump -D -b binary -m i8086 -Mintel           msdos_boot_code.bin");

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

    if      (msdosPartitionTable.type[0] == 0xfa && msdosPartitionTable.type[1] == 0x33) printf("  Type = Dos 3.3 through Windows 95a\n");
    else if (msdosPartitionTable.type[0] == 0x33 && msdosPartitionTable.type[1] == 0xc0) printf("  Type = Windows 95B, 98, 98SE, ME, 2K, XP or Vista\n");
    else if (msdosPartitionTable.type[0] == 0xfa && msdosPartitionTable.type[1] == 0xeb) printf("  Type = LILO\n");
    else if (msdosPartitionTable.type[0] == 0xeb && msdosPartitionTable.type[1] == 0x3c) printf("  Type = Windows floopy disk\n");
    else                                                                                 printf("  Unrecognized type\n");

//  disassemble_msdos_boot_code(&msdosPartitionTable);

    for (int partNo = 0; partNo < NOF_MSDOS_PRIMARY_PARTITIONS; partNo++) {
      printf("    Partition %2d: type = 0x%02x\n", partNo, msdosPartitionTable.partitions[partNo].type);
    }

  }

}

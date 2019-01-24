// #include <stdio.h>
// #include <fcntl.h>
#include <stdlib.h>
// #include <stdint.h>
#include <sys/sysmacros.h>

#include "structs/BlockDevice.h"
#include "structs/MSDosMBR.h"

#include <stdio.h>
#include <inttypes.h> // PRIu32 etc.

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

int isMSDosMBR(BlockDevice *b, struct MSDosMBR *mbr) {

 //
 // Read first sector
 //
    fseek(b->f, 0, SEEK_SET);
    fread(mbr, sizeof(*mbr), 1, b->f);

    return mbr->magic[0] == 0x55 && 
           mbr->magic[1] == 0xaa;

//  printf("low: %x, high: %x\n", mbr->magic[0], mbr->magic[1]);

}

void disassemble_msdos_boot_code(struct MSDosMBR *t) {

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

  struct MSDosMBR mbr;
  if (isMSDosMBR(&blockDevice, &mbr)) {
    printf("Device has a MSDos partition table.\n");

    if      (mbr.version[0] == 0xfa && mbr.version[1] == 0x33) printf("  MBR version = Dos 3.3 through Windows 95a\n");
    else if (mbr.version[0] == 0x33 && mbr.version[1] == 0xc0) printf("  MBR version = Windows 95B, 98, 98SE, ME, 2K, XP or Vista\n");
    else if (mbr.version[0] == 0xfa && mbr.version[1] == 0xeb) printf("  MBR version = LILO\n");
    else if (mbr.version[0] == 0xeb && mbr.version[1] == 0x3c) printf("  MBR version = Windows floopy disk\n");
    else                                                                                       printf("  Unrecognized MBR version %02X%02X.\n", mbr.version[0], mbr.version[1]);

//  disassemble_msdos_boot_code(&mbr);

    printf("     #   type  1st sector      length\n");
    for (int partNo = 0; partNo < NOF_MSDOS_PRIMARY_PARTITIONS; partNo++) {
      printf("    %2d:  0x%02x  %10"PRIu32"  %10"PRIu32"\n", partNo, mbr.partitions[partNo].type, mbr.partitions[partNo].first_sector, mbr.partitions[partNo].nof_sectors);
    }

  }

}

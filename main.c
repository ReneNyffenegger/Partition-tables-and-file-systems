// #include <stdio.h>
// #include <fcntl.h>
#include <stdlib.h>
// #include <stdint.h>
#include <sys/sysmacros.h>

typedef unsigned long long sector;

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

char const* partitionTypeToString(uint8_t type) {
   if (type == 0x00) return "Empty";
   if (type == 0x01) return "FAT12 ";
   if (type == 0x02) return "Xenix";
   if (type == 0x03) return "Xenix";
   if (type == 0x04) return "FAT16 (max. 32 MB)";
   if (type == 0x05) return "Extended DOS-Partition" ;  // Max. 2 GB
   if (type == 0x06) return "FAT16"                  ;  // Max. 2 GB
   if (type == 0x07) return "HPFS/NTFS";
   if (type == 0x08) return "AIX ";
   if (type == 0x09) return "AIX bootable";
   if (type == 0x0A) return "OS/2 Bootmanager";
   if (type == 0x0B) return "FAT32 (CHS)";
   if (type == 0x0C) return "FAT32 (LBA)";
   if (type == 0x0E) return "FAT16 (LBA)";
   if (type == 0x0F) return "Extended partition (LBA)";  // More than 1024 cylinders
   if (type == 0x10) return "OPUS"                    ;  // Or »hidden« ?
   if (type == 0x11) return "Hidden FAT12";
   if (type == 0x12) return "Compaq diagnost";
   if (type == 0x14) return "Hidden FAT16 bis 32MB";
   if (type == 0x16) return "Hidden FAT16";
   if (type == 0x17) return "Hidden HPFS / NTFS";
   if (type == 0x18) return "AST Windows swap";
   if (type == 0x1B) return "Hidden WIN95 FAT32";
   if (type == 0x1C) return "Hidden WIN95 FAT32 (LBA)";
   if (type == 0x1E) return "Hidden WIN95 FAT16 (LBA)";
   if (type == 0x24) return "NEC DOS";
   if (type == 0x27) return "MSFT Recovery";
   if (type == 0x39) return "Plan 9";
   if (type == 0x3C) return "Partition Magic";
   if (type == 0x40) return "Venix 80286";
   if (type == 0x41) return "PPC PReP boot";
   if (type == 0x42) return "LDM"                            ;// or LDM ?
   if (type == 0x4D) return "QNX4.x";
   if (type == 0x4E) return "QNX4.x 2nd partition";
   if (type == 0x4F) return "QNX4.x 3rd partition";
   if (type == 0x50) return "OnTrack DM";
   if (type == 0x51) return "OnTrack DM6 Aux";
   if (type == 0x52) return "CP/M";
   if (type == 0x53) return "OnTrack DM6 Aux";
   if (type == 0x54) return "OnTrack DM6";
   if (type == 0x55) return "EZ-Drive";
   if (type == 0x56) return "Golden Bow";
   if (type == 0x5c) return "Priam Edisk";
   if (type == 0x61) return "Speed Stor";
   if (type == 0x63) return "GNU HURD or SYS";
   if (type == 0x64) return "Novell NetWare";
   if (type == 0x65) return "Novell";
   if (type == 0x70) return "Disk Secure Mult";
   if (type == 0x75) return "UNIX PC/IX";
   if (type == 0x80) return "aktiv (old Minix)";
   if (type == 0x81) return "Booten von Laufwerk D:";
   if (type == 0x82) return "Linux Swap";
   if (type == 0x83) return "Linux native";
   if (type == 0x84) return "OS/2 hidden C:";
   if (type == 0x85) return "LINUX extended";
   if (type == 0x86) return "NTFS volume set";
   if (type == 0x87) return "NTFS volume set";
   if (type == 0x8e) return "LINUX LVM";
   if (type == 0x93) return "Amoebla";
   if (type == 0x94) return "Amoebla BBT";
   if (type == 0x9F) return "BSD/OS";
   if (type == 0xA0) return "IBM Thinkpad hidden";
   if (type == 0xA5) return "BSD/386";
   if (type == 0xA6) return "Open BSD";
   if (type == 0xA7) return "NeXT STEP";
   if (type == 0xAF) return "HFS";
   if (type == 0xB7) return "BSDI fs";
   if (type == 0xB8) return "BSDI swap";
   if (type == 0xBF) return "SUN UFS";
   if (type == 0xC1) return "DRDOS/sec (FAT32)";
   if (type == 0xC4) return "DRDOS/sec (FAT32(LBA))";
   if (type == 0xC6) return "DRDOS/sec (FAT16(LBA))";
   if (type == 0xC7) return "Syrinx";
   if (type == 0xDA) return "Non-Fs data";
   if (type == 0xDB) return "Concurrent DOS, CP/M, CTOS";
   if (type == 0xDE) return "Dell Utility";                       // Or »Dell Diagnostics«?
   if (type == 0xE1) return "DOS access";
   if (type == 0xE3) return "DOS R/o";
   if (type == 0xE4) return "Speed Stor";
   if (type == 0xEB) return "BeOS fs";
   if (type == 0xEE) return "EFI GPT";
   if (type == 0xEF) return "EFI (FAT12/16/32)";
   if (type == 0xF0) return "Palo";
   if (type == 0xF1) return "Speed Stor";
   if (type == 0xF2) return "DOS secondary";
   if (type == 0xF4) return "Speed Stor";
   if (type == 0xFD) return "LINUX raid auto";
   if (type == 0xFE) return "LVM old";                            // Or LANstep ?
   if (type == 0xFF) return "BBT";
   return "?";

}

int isExtendedPartitiontype(uint8_t type) {
  if (type == 0x05  /* Extended DOS Partition */ ||
      type == 0x0F  /* Extended LBA           */ ||
      type == 0x85  /* LINUX extended         */) {
    return 1;
  }
  return 0;
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
    else                                                       printf("  Unrecognized MBR version %02X%02X.\n", mbr.version[0], mbr.version[1]);

//  disassemble_msdos_boot_code(&mbr);


    int partNo = 1;

    printf("     #   Type                            1st sector      length   last sector  Ext\n");
    for (struct MSDosPartition *part = &mbr.partitions[0]; part< &mbr.partitions[NOF_MSDOS_PRIMARY_PARTITIONS]; part ++, partNo++) {
      sector sector_1st  = part -> first_sector;
      sector nof_sectors = part -> nof_sectors ;
      sector sector_last = sector_1st + nof_sectors - 1;

      char const* type = partitionTypeToString(part -> type);

      printf("    %2d:  0x%02x %-25s  %10llu  %10llu    %10llu  %-3s\n", partNo, part->type, type, sector_1st, nof_sectors, sector_last, isExtendedPartitiontype(part->type) ? "Ext": "");
    }

  }

}

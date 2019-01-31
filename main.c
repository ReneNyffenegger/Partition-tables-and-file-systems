// #include <stdio.h>
// #include <fcntl.h>
#include <stdlib.h>
// #include <stdint.h>
#include <sys/sysmacros.h>

typedef unsigned long long sector;
const sector sector_size = 512;

#include "structs/BlockDevice.h"
#include "structs/MSDosMasterBootRecord.h"
#include "structs/MSDosExtendedBootRecord.h"

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

int checkMSDosBootRecordSignature(MSDosBootRecordSignature *signature) {

    return (*signature)[0] == 0x55 && 
           (*signature)[1] == 0xaa;

}

int isMSDosMasterBootRecord(BlockDevice *b, MSDosMasterBootRecord *mbr) {

 //
 // Read first sector
 //
    fseek(b->f, 0, SEEK_SET);
    fread(mbr, sizeof(*mbr), 1, b->f);

    return checkMSDosBootRecordSignature(&mbr->msdos_boot_record_signature);
}

void disassemble_msdos_boot_code(MSDosMasterBootRecord *t) {

  FILE *f = fopen("msdos_boot_code.bin", "w");
  if (!f) exit(2);

  fwrite(t->boot_code, sizeof(t->boot_code), 1, f);
  fclose(f);

//system("objdump -D -b binary -m i386  -Mintel,x86-64 -z msdos_boot_code.bin");
//system("objdump -D -b binary -m i386  -Mintel           msdos_boot_code.bin");
  system("objdump -D -b binary -m i8086 -Mintel           msdos_boot_code.bin");

}

char const* partitionTypeToString(MSDosPartitionTableEntry *entry) {

//
// Wikipedia: Other extended partition types which may hold EBRs include the
//            deliberately hidden types 0x15, 0x1F, 0x91 and 0x9B, the access-restricted
//            types 0x5E and 0x5F, and the secured types 0xCF and 0xD5. 

   if (entry->partition_type == 0x00) return "Empty";
   if (entry->partition_type == 0x01) return "FAT12 ";
   if (entry->partition_type == 0x02) return "Xenix";
   if (entry->partition_type == 0x03) return "Xenix";
   if (entry->partition_type == 0x04) return "FAT16 (max. 32 MB)";
   if (entry->partition_type == 0x05) return "Extended DOS-Partition" ;  // Max. 2 GB
   if (entry->partition_type == 0x06) return "FAT16"                  ;  // Max. 2 GB
   if (entry->partition_type == 0x07) return "HPFS/NTFS";
   if (entry->partition_type == 0x08) return "AIX ";
   if (entry->partition_type == 0x09) return "AIX bootable";
   if (entry->partition_type == 0x0A) return "OS/2 Bootmanager";
   if (entry->partition_type == 0x0B) return "FAT32 (CHS)";
   if (entry->partition_type == 0x0C) return "FAT32 (LBA)";
   if (entry->partition_type == 0x0E) return "FAT16 (LBA)";
   if (entry->partition_type == 0x0F) return "Extended partition (LBA)";  // More than 1024 cylinders
   if (entry->partition_type == 0x10) return "OPUS"                    ;  // Or »hidden« ?
   if (entry->partition_type == 0x11) return "Hidden FAT12";
   if (entry->partition_type == 0x12) return "Compaq diagnost";
   if (entry->partition_type == 0x14) return "Hidden FAT16 bis 32MB";
   if (entry->partition_type == 0x16) return "Hidden FAT16";
   if (entry->partition_type == 0x17) return "Hidden HPFS / NTFS";
   if (entry->partition_type == 0x18) return "AST Windows swap";
   if (entry->partition_type == 0x1B) return "Hidden WIN95 FAT32";
   if (entry->partition_type == 0x1C) return "Hidden WIN95 FAT32 (LBA)";
   if (entry->partition_type == 0x1E) return "Hidden WIN95 FAT16 (LBA)";
   if (entry->partition_type == 0x24) return "NEC DOS";
   if (entry->partition_type == 0x27) return "MSFT Recovery";
   if (entry->partition_type == 0x39) return "Plan 9";
   if (entry->partition_type == 0x3C) return "Partition Magic";
   if (entry->partition_type == 0x40) return "Venix 80286";
   if (entry->partition_type == 0x41) return "PPC PReP boot";
   if (entry->partition_type == 0x42) return "LDM"                            ;// or LDM ?
   if (entry->partition_type == 0x4D) return "QNX4.x";
   if (entry->partition_type == 0x4E) return "QNX4.x 2nd partition";
   if (entry->partition_type == 0x4F) return "QNX4.x 3rd partition";
   if (entry->partition_type == 0x50) return "OnTrack DM";
   if (entry->partition_type == 0x51) return "OnTrack DM6 Aux";
   if (entry->partition_type == 0x52) return "CP/M";
   if (entry->partition_type == 0x53) return "OnTrack DM6 Aux";
   if (entry->partition_type == 0x54) return "OnTrack DM6";
   if (entry->partition_type == 0x55) return "EZ-Drive";
   if (entry->partition_type == 0x56) return "Golden Bow";
   if (entry->partition_type == 0x5c) return "Priam Edisk";
   if (entry->partition_type == 0x61) return "Speed Stor";
   if (entry->partition_type == 0x63) return "GNU HURD or SYS";
   if (entry->partition_type == 0x64) return "Novell NetWare";
   if (entry->partition_type == 0x65) return "Novell";
   if (entry->partition_type == 0x70) return "Disk Secure Mult";
   if (entry->partition_type == 0x75) return "UNIX PC/IX";
   if (entry->partition_type == 0x80) return "aktiv (old Minix)";
   if (entry->partition_type == 0x81) return "Booten von Laufwerk D:";
   if (entry->partition_type == 0x82) return "Linux Swap"            ;
   if (entry->partition_type == 0x83) return "Linux native"          ;
   if (entry->partition_type == 0x84) return "OS/2 hidden C:"        ;
   if (entry->partition_type == 0x85) return "LINUX extended"        ;  // Wikipedia:  Linux supports the concept of a second extended partition chain with type 0x85 — this type is hidden (unknown) for other operating systems supporting only one chain.
   if (entry->partition_type == 0x86) return "NTFS volume set"       ;
   if (entry->partition_type == 0x87) return "NTFS volume set"       ;
   if (entry->partition_type == 0x8e) return "LINUX LVM";
   if (entry->partition_type == 0x93) return "Amoebla";
   if (entry->partition_type == 0x94) return "Amoebla BBT";
   if (entry->partition_type == 0x9F) return "BSD/OS";
   if (entry->partition_type == 0xA0) return "IBM Thinkpad hidden";
   if (entry->partition_type == 0xA5) return "BSD/386";
   if (entry->partition_type == 0xA6) return "Open BSD";
   if (entry->partition_type == 0xA7) return "NeXT STEP";
   if (entry->partition_type == 0xAF) return "HFS";
   if (entry->partition_type == 0xB7) return "BSDI fs";
   if (entry->partition_type == 0xB8) return "BSDI swap";
   if (entry->partition_type == 0xBF) return "SUN UFS";
   if (entry->partition_type == 0xC1) return "DRDOS/sec (FAT32)";
   if (entry->partition_type == 0xC4) return "DRDOS/sec (FAT32(LBA))";
// if (entry->partition_type == 0xC5) return "DRDOS/sec ?";                        // Wikipedia: DR DOS 6.0 and higher support secured extended partitions using 0xC5, which are invisible to other operating systems.
   if (entry->partition_type == 0xC6) return "DRDOS/sec (FAT16(LBA))";
   if (entry->partition_type == 0xC7) return "Syrinx";
   if (entry->partition_type == 0xDA) return "Non-Fs data";
   if (entry->partition_type == 0xDB) return "Concurrent DOS, CP/M, CTOS";
   if (entry->partition_type == 0xDE) return "Dell Utility";                       // Or »Dell Diagnostics«?
   if (entry->partition_type == 0xE1) return "DOS access";
   if (entry->partition_type == 0xE3) return "DOS R/o";
   if (entry->partition_type == 0xE4) return "Speed Stor";
   if (entry->partition_type == 0xEB) return "BeOS fs";
   if (entry->partition_type == 0xEE) return "EFI GPT";
   if (entry->partition_type == 0xEF) return "EFI (FAT12/16/32)";
   if (entry->partition_type == 0xF0) return "Palo";
   if (entry->partition_type == 0xF1) return "Speed Stor";
   if (entry->partition_type == 0xF2) return "DOS secondary";
   if (entry->partition_type == 0xF4) return "Speed Stor";
   if (entry->partition_type == 0xFD) return "LINUX raid auto";
   if (entry->partition_type == 0xFE) return "LVM old";                            // Or LANstep ?
   if (entry->partition_type == 0xFF) return "BBT";
   return "?";

}

int isActivePartition(MSDosPartitionTableEntry* entry) {

  if (entry->boot_indicator == 0x80) return 1;
  if (entry->boot_indicator == 0x00) return 0;

  printf("Unexpected boot_indicator %x\n", entry->boot_indicator);
  return 0;

}

int isLBAPartition(MSDosPartitionTableEntry* entry) {

  if (entry-> partition_type == 0x0C) return 1; // FAT32 LBA
  if (entry-> partition_type == 0x0E) return 1; // FAT16 LBA
  if (entry-> partition_type == 0x0F) return 1; // Extended LBA
  if (entry-> partition_type == 0x1C) return 1; // FAT32 LBA Hidden
  if (entry-> partition_type == 0x1E) return 1; // FAT16 LBA Hidden

  return 0;

}


sector CHS_to_LBA(RawCHS* chs) {

  int c = chs->cylinder + ((chs->sector >> 6) << 8);
  if (c > 1021) return 0;

	int h = chs->head;
  int s =(chs->sector & 0x3f) - 1;

  if (s<0) return 0;

 // TODO return (c * bios_geom->heads + h) * bios_geom->sectors + s;
  
}

int isExtendedPartitiontype(MSDosPartitionTableEntry* entry) {
  if (entry->partition_type == 0x05  /* Extended DOS Partition */ ||
      entry->partition_type == 0x0F  /* Extended LBA           */ ||
      entry->partition_type == 0x85  /* LINUX extended         */) {
    return 1;
  }
  return 0;
}

void printPartitionLine(MSDosPartitionTableEntry *entry, int partNo, sector sectorStartPrimaryOrExtended, sector sectorStartLogical) {

      sector start       = entry -> partition_start_lba;
      sector sector_1st  = start + sectorStartPrimaryOrExtended + sectorStartLogical;
      sector nof_sectors = entry -> nof_sectors ;
      sector sector_last = sector_1st + nof_sectors - 1;

      char const* type = partitionTypeToString(entry);

      printf("    %2d:  0x%02x %-25s  %-3s  %10llu + %10llu = %10llu  %10llu    %10llu  %-3s  %-3s | %4d %4d %4d  %4d %4d %4d\n", partNo, entry->partition_type, type,
          isLBAPartition(entry) ? "LBA" : "CHS", 
          sectorStartPrimaryOrExtended, start, sector_1st, nof_sectors, sector_last,
          isActivePartition(entry) ? "Act" : "",
          isExtendedPartitiontype(entry) ? "Ext": "",
          entry->partition_start_chs.cylinder, entry->partition_start_chs.head, entry->partition_start_chs.sector,
          entry->partition_end_chs  .cylinder, entry->partition_end_chs  .head  , entry->partition_end_chs.sector);

}

void showMSDosExtendedPartition(BlockDevice *dev, MSDosExtendedBootRecord *ebr, sector sectorStartExtended, sector sectorStartLogical) {

    int partNo=5;

    for (int i = 0; i<16; i ++) { if (ebr->unused_third_entry [i]) { printf("That was not expected!\n"); }}
    for (int i = 0; i<16; i ++) { if (ebr->unused_fourth_entry[i]) { printf("That was not expected!\n"); }}

    if (!checkMSDosBootRecordSignature(&ebr->msdos_boot_record_signature)) {
      printf("Unexpected boot record signature: %x %x\n", ebr->msdos_boot_record_signature[0], ebr->msdos_boot_record_signature[1]);
    }

    printPartitionLine(&ebr->thisLogicalPartition, partNo, sectorStartExtended, sectorStartLogical);

    if (isExtendedPartitiontype(&ebr->nextLogicalPartition)) {

         printPartitionLine(&ebr->nextLogicalPartition, partNo, sectorStartExtended, sectorStartLogical);
         MSDosExtendedBootRecord ebrNext;
 
         sector sector_1st = ebr->nextLogicalPartition.partition_start_lba + sectorStartExtended;
 
         if (fseek(dev->f, sector_1st * sector_size, SEEK_SET)) {
            perror("fseek");
         }
         if (fread(&ebrNext, sizeof(ebrNext), 1, dev->f) != 1) {
            perror("fread");
         }
 
         for (int i = 0; i<16; i ++) { if (ebrNext.unused_third_entry [i]) { printf("That was not expected!\n"); }}
         for (int i = 0; i<16; i ++) { if (ebrNext.unused_fourth_entry[i]) { printf("That was not expected!\n"); }}
 
         showMSDosExtendedPartition(dev, &ebrNext, sectorStartExtended, ebr->nextLogicalPartition.partition_start_lba                                            /*sector_1st* //*, start*/);
    }


}

void showMSDosPartitions(BlockDevice* dev, MSDosMasterBootRecord* mbr, int nofRecords, sector sectorStartPrimaryOrExtended/*, sector start__*/) {

  //  an extended partition looks like a whole disk with its own partition
  //  table and contains at most two partitions: a logical partition, and
  //  another extended partition nested inside it, which in turn may have
  //  another extended partition nested inside it, etc

    if      (mbr->version[0] == 0xfa && mbr->version[1] == 0x33) printf("  MBR version = Dos 3.3 through Windows 95a\n");
    else if (mbr->version[0] == 0x33 && mbr->version[1] == 0xc0) printf("  MBR version = Windows 95B, 98, 98SE, ME, 2K, XP or Vista\n");
    else if (mbr->version[0] == 0xfa && mbr->version[1] == 0xeb) printf("  MBR version = LILO\n");
    else if (mbr->version[0] == 0xeb && mbr->version[1] == 0x3c) printf("  MBR version = Windows floopy disk\n");
    else                                                         printf("  Unrecognized MBR version %02X%02X.\n", mbr->version[0], mbr->version[1]);

//  disassemble_msdos_boot_code(&mbr);

    int partNo = 1;

    for (MSDosPartitionTableEntry *part = &mbr->partitions[0]; part< &mbr->partitions[nofRecords]; part ++, partNo++) {

      printPartitionLine(part, partNo, 0, 0);

      if (isExtendedPartitiontype(part)) {

        MSDosExtendedBootRecord ebr;

        sector sector_1st = part->partition_start_lba;

        printf("going to read sector %llu\n", sector_1st);
        if (fseek(dev->f, sector_1st * sector_size, SEEK_SET)) {
           perror("fseek");
        }
        if (fread(&ebr, sizeof(ebr), 1, dev->f) != 1) {
           perror("fread");
        }

        if (! checkMSDosBootRecordSignature(&ebr.msdos_boot_record_signature)) {
          printf("Unexpected MSDos boot record signature: %x %x\n", ebr.msdos_boot_record_signature[0], ebr.msdos_boot_record_signature[1]);
        }

        showMSDosExtendedPartition(dev, &ebr, sector_1st, 0);

      }
    }
}

void checkSizes() {
  
  if (sizeof(MSDosMasterBootRecord) != 512) {
    printf("sizeof(MSDosMasterBootRecord) = %ld\n", sizeof(MSDosMasterBootRecord));
  }

  if (sizeof(MSDosExtendedBootRecord) != 512) {
    printf("sizeof(MSDosExtendedBootRecord) = %ld\n", sizeof(MSDosExtendedBootRecord));
  }

}

int main() {

  BlockDevice blockDevice;
  blockDevice.path = "/dev/sda";

  checkSizes();

  openBlockDevice    (&blockDevice);
  stat_              (&blockDevice);
  
  if (isSCSI(&blockDevice)) {
     printf("SCSI\n");
  }
  else if (isIDE(&blockDevice)) {
     printf("IDE\n");
  }

  MSDosMasterBootRecord mbr;
  if (isMSDosMasterBootRecord(&blockDevice, &mbr)) {
    printf("Device has a MSDos partition table.\n");

    printf("     #   Type                            C/L      st. Ext+      start = 1st sector      length   last sector  Act  Ext |    c    h    s     c    h    s\n");
    showMSDosPartitions(&blockDevice, &mbr, NOF_MSDOS_PRIMARY_PARTITIONS, 0/*, 0*/);
  }

}

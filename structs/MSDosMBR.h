#include "MSDosPartition.h"

#define NOF_MSDOS_PRIMARY_PARTITIONS 4

typedef unsigned char MSDosBootRecordSignature[2];

typedef struct /* parted names it: _DosRawTable */  {
//
//   The MBR (Master Boot Record)
//
//   Originally ripped from Linux source.
//
    union {
     //
     // The first two bytes of the boot code apparently also indicate
     // the version of the MBR as follows (https://www.bibase.com/mbr.htm):
     //    fa 33: Dos 3.3 through Windows 95 a
     //    33 c0: Windows 95B, 98, 98SE, ME, 2000, XP, Vista 
     //    fa eb: LILO
     //    eb 3c: Windows Floppy Disk boot record 
     //
     //  https://github.com/joyent/syslinux/commit/d0f275981c9289dc4b8df64e72cd9902bf85aebe says: 
     //     Apparently some BIOSes (including some Acer Travelmate machines)
     //     require an MBR to start with 0x33; apparently Micro$oft MBRs start
     //     with 33 C0, an alternate coding of the "xorw %ax,%ax" instruction.  As
     //     such, follow suit to work on these braindead BIOSes.
        unsigned char          version   [  2];
     // --------------------------------------------------------------
        char                   boot_code [440];
    };
 // ------------------------------------------------------------------
 // mbr_signature apparently is a unique ID.
 // Beginning with Windows 2K, the value of mbr_signature is stored
 // in the registry under
 //     HKLM\System\MountedDevices
 //
    uint32_t                  mbr_signature;
 // ------------------------------------------------------------------
    uint16_t                  unknown;
    MSDosPartitionTableEntry  partitions [NOF_MSDOS_PRIMARY_PARTITIONS];
 // ------------------------------------------------------------------
 // The msdos_boot_record_signature contains 0xaa55 in little endian format.
 // Thus msdos_boot_record_signature[0] is the low byte and should equal to 0x55 while
 // msdos_boot_record_signature[1] is the high byte and shoul equal to 0xaa.
 //
    MSDosBootRecordSignature  msdos_boot_record_signature;
 // ------------------------------------------------------------------
}  __attribute__((packed)) MSDosMBR;

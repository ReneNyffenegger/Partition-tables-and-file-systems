#include "MSDosPartition.h"

#define NOF_MSDOS_PRIMARY_PARTITIONS 4

/* ripped from Linux source */
struct /* parted names it: _DosRawTable */  MSDosPartitionTable {
    union {
     //
     // The first two bytes of the boot code apparently also indicate
     // the version of the MBR as follows (https://www.bibase.com/mbr.htm):
     //    fa 33: Dos 3.3 through Windows 95 a
     //    33 c0: Windows 95B, 98, 98SE, ME, 2000, XP, Vista 
     //    fa eb: LILO
     //    eb 3c: Windows Floppy Disk boot record 
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
    uint16_t                  Unknown;
    struct MSDosPartition     partitions [NOF_MSDOS_PRIMARY_PARTITIONS];
 // ------------------------------------------------------------------
 // magic contains 0xaa55 in little endian format.
 //    Thus magic[0] is the low byte and should equal to 0x55 while
 //    magic[1] is the high byte and shoul equal to 0xaa.
 //
    unsigned char             magic[2];
 // uint16_t                  magic;
 // ------------------------------------------------------------------
} __attribute__((packed));

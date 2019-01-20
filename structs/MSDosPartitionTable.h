#include "MSDosPartition.h"

#define NOF_MSDOS_PRIMARY_PARTITIONS 4

/* ripped from Linux source */
struct /* parted names it: _DosRawTable */  MSDosPartitionTable {
    char                      boot_code [440];
    uint32_t                  mbr_signature;  /* really a unique ID */
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

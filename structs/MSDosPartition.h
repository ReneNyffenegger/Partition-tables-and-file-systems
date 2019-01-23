#include <stdint.h>  // uint8_t etc.

/* note: lots of bit-bashing here, thus, you shouldn't look inside it.
 * Use chs_to_sector() and sector_to_chs() instead.
 */
typedef struct {
	uint8_t		head;
	uint8_t		sector;
	uint8_t		cylinder;
} __attribute__((packed)) RawCHS;

/* ripped from Linux source */
struct /* parted names it: _DosRawPartition */ MSDosPartition {
    uint8_t    boot_ind;   /* 00:  0x80 - active */
    RawCHS     chs_start;  /* 01: */
//  ----------------------------------------------
//    Partition type (https://www.win.tue.nl/~aeb/partitions/partition_types-1.html)
//        0x00:  empty
//        0x01:  FAT12
//        0x04:  FAT16 SM (?)
//        0x05:  Dos Ext
//        0x06:  FAT16
//        0x07:  NTFS or HPFS
//        0x0b:  FAT32
//        0x0c:  FAT32 LBA
//        0x0e:  FAT16 LBA
//        0x0f:  Ext LBA
//     The FAT* and NTFS types can be combined with the 0x10 flag which
//     means: hidden.
//
//
    uint8_t    type;            /* 04: partition type */
//  ----------------------------------------------
    RawCHS     chs_end;         /* 05: */
    uint32_t   first_sector;    /* 08: starting sector counting from 0 */
    uint32_t   nof_sectors;     /* 0c: nr of sectors in partition */
} __attribute__((packed));

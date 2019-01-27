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
typedef struct /* parted names it: _DosRawPartition */ {
 //
 // The boot indicator is either 0x80 (active) or 0x00 (inactive): 
 //
    uint8_t    boot_indicator;

    RawCHS     partition_start_chs;
//  ----------------------------------------------
//
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
//     The partition type (of extended partitions?) allows to determine if
//     CHS or LBA addressing is to be applied.
//        0x05: CHS Addressing
//        0x0F: LBA Addressing
//
//
    uint8_t    partition_type;
//  ----------------------------------------------
    RawCHS     partition_end_chs;
    uint32_t   partition_start_lba;
    uint32_t   nof_sectors;
}__attribute__((packed)) MSDosPartitionTableEntry;

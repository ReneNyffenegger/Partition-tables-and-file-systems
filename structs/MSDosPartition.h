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
 // Only ONE partition table entry should be active. And it should be
 // in the the master boot record, not in the extended boot record.
 //
    uint8_t    boot_indicator;

 // ----------------------------------------------
 //
 //    https://thestarman.pcministry.com/asm/mbr/PartTables.htm says:
 //        Starting Sector in CHS values. These values pinpoint the location of
 //        a partition's first sector, if  it's within the first 1024 cylinders
 //        of a hard disk.
 //        When a sector is beyond that point, the CHS tuples are normally set
 //        to their maximum allowed values of 1023, 254, 63; which stand for
 //        the 1024th cylinder, 255th head and 63rd sector, due to the fact,
 //        cylinder and head counts begin at zero. These values appear on the
 //        disk as the three bytes: FE FF FF (in that order).

    RawCHS     partition_start_chs;
 // ----------------------------------------------
 //
 //   Partition type (https://www.win.tue.nl/~aeb/partitions/partition_types-1.html, https://thestarman.pcministry.com/asm/mbr/PartTypes.htm)
 //       0x00:  empty
 //       0x01:  FAT12
 //       0x04:  FAT16 SM (?)
 //       0x05:  Dos Ext
 //       0x06:  FAT16
 //       0x07:  NTFS or HPFS
 //       0x0b:  FAT32
 //       0x0c:  FAT32 LBA
 //       0x0e:  FAT16 LBA
 //       0x0f:  Ext LBA
 //    The FAT* and NTFS types can be combined with the 0x10 flag which
 //    means: hidden.
 //
 //    The partition type (of extended partitions?) allows to determine if
 //    CHS or LBA addressing is to be applied.
 //       0x05: CHS Addressing
 //       0x0F: LBA Addressing
 //
 //
    uint8_t    partition_type;
//  ----------------------------------------------
    RawCHS     partition_end_chs;
    uint32_t   partition_start_lba;
    uint32_t   nof_sectors;
}__attribute__((packed)) MSDosPartitionTableEntry;

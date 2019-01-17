#include "MSDosPartition.h"

#define NOF_MSDOS_PRIMARY_PARTITIONS 4

/* ripped from Linux source */
struct /* parted names it: _DosRawTable */  MSDosPartitionTable {
  char                      boot_code [440];
  uint32_t                  mbr_signature;  /* really a unique ID */
  uint16_t                  Unknown;
  struct MSDosPartition     partitions [NOF_MSDOS_PRIMARY_PARTITIONS];
  uint16_t                  magic;
} __attribute__((packed));

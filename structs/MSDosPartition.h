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
  uint8_t    type;       /* 04: partition type */
  RawCHS     chs_end;    /* 05: */
  uint32_t   start;      /* 08: starting sector counting from 0 */
  uint32_t   length;     /* 0c: nr of sectors in partition */
} __attribute__((packed));

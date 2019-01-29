typedef struct {

 //
 // https://en.wikipedia.org/wiki/Extended_boot_record
 //

 // The first 446 bytes somewhat corresponds to boot_code in MBR (MSDosMBR) (but PROBABLY (to be verified) does not have boot code.
 // But boot code in the MBR consists of 440 bytes, though.
 //
    char unused[446];

 // --------------------------------------------------------------------------------------
 //
//                                 #define NOF_MSDOS_EXTENDED_PARTITIONS_IN_BOOT_RECORD 2
 //
 // The first entry of an EBR points to the logical partition of that EBR.
 //
 // The second entry of an EBR either
 //   - points to the next EBR in the chain, or
 //   - is filled with zeroes
 //
//  MSDosPartitionTableEntry   partitions [NOF_MSDOS_EXTENDED_PARTITIONS_IN_BOOT_RECORD];
    MSDosPartitionTableEntry   thisLogicalPartition;
    MSDosPartitionTableEntry   nextLogicalPartition;
    

    unsigned char unused_third_entry [16]; // should be filled with zeroes
    unsigned char unused_fourth_entry[16]; // should be filled with zeroes

 //
 // Analogous to msdos_boot_record_signature in MSDosMasterBootRecord.
 //
    MSDosBootRecordSignature  msdos_boot_record_signature;

} MSDosExtendedBootRecord;

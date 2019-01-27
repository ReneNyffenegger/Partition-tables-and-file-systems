#include <stdio.h>    // FILE
#include <sys/stat.h>

typedef struct /* _BlockDevice*/ {

       //
       // Modelled after libparted, PedDevice (include/parted/device.h)
       //


//        PedDevice*      next;
//
//        char*           model;          /**< \brief description of hardware
//                                             (manufacturer, model) */
          char*           path;           /**< device /dev entry */
//
//        PedDeviceType   type;           /**< SCSI, IDE, etc. \sa PedDeviceType */
//        long long       sector_size;            /**< logical sector size */
//        long long       phys_sector_size;       /**< physical sector size */
//        PedSector       length;                 /**< device length (LBA) */
//
//        int             open_count; /**< the number of times this device has
//                                         been opened with ped_device_open(). */
//        int             read_only;
//        int             external_mode;
//        int             dirty;
//        int             boot_dirty;
//
//        PedCHSGeometry  hw_geom;
//        PedCHSGeometry  bios_geom;
//        short           host, did;
//
//
//     ------------------------- 
//        void*           arch_specific;
//
//     Following is modelled after LinuxSpecific (libparted/arch/linux.h)

//	int	fd;
    FILE     *f;
  	int	      major;
  	int	      minor;
//	char*    	dmtype;         /**< device map target type */
#if USE_BLKID
              blkid_probe probe;
              blkid_topology topology;
#endif

    // TQ84...

    struct    stat stat_;

} BlockDevice;

// typedef struct _BlockDevice BlockDevice;

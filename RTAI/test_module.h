#ifndef __TEST_MODULE_H
#define __TEST_MODULE_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
//#include <linux/kern_levels.h>

//!< RTAI Include
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_sem.h>
#include <rtai_fifos.h>
#include <rtai_nam2num.h>
#include <rtai_types.h>
#include <rtai_lxrt.h>

//!< Comedi and ComediLib Include
#include <linux/comedi.h>
#include <linux/comedilib.h>


//!< License, Authors and description
MODULE_LICENSE("GPL");
MODULE_AUTHOR("GNB-UAM");
MODULE_DESCRIPTION("test_module");


//!< Prototypes
//!< init functions
static int __init rttask_init(void);

//!< clean up functions
static void __exit rttask_exit(void);


#endif // TEST_MODULE_H


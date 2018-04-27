#include "test_module.h"

static int rttask_init(void);
static void rttask_exit(void);

RTIME expected;
int period_counts;
static RT_TASK rt_get_data_task;



#define NSEC_PER_SEC (1000000000)
double freq = 20000.0;
unsigned int t = 30;
RTIME period;

/* Aux */

typedef struct {
    short id;
    int lat;
    double absol;
} message;


static void rt_latency_loop (int arg) {
	int i;
	void * dsc = NULL;
    unsigned int mask = 0x0001;
    unsigned int data;
    message msg;
    long points = t * freq;

	RTIME iter, period, start;
	int lat;
    msg.id = 0;


    if (!(dsc = comedi_open("/dev/comedi0")))
    {
        printk("ERROR: fail to open device /dev/comedi0\n");
        return 1;
    }

    comedi_dio_config (dsc, 2, 0, COMEDI_OUTPUT);

    data = 0;
    comedi_dio_bitfield (dsc, 2, mask, &data);
   

    expected += period_counts;

    rt_printk("RTAI_MODULE_TEST: start\n");
    start = rt_get_time();

	for (i = 0; i < points; i++) {
		rt_task_wait_period();

		msg.lat = (int) count2nano(rt_get_time() - expected);
        msg.absol = ((RTIME) count2nano(rt_get_time() - start)) * 0.000001;

        if (i % 2 || i < 2 || i > points-3) {
            data = 1;
        } else {
            data = 0;
        }

        msg.data = data;

        rtf_put(0, &msg, sizeof(msg));

        comedi_dio_bitfield (dsc, 2, mask, &data);
        //msleep(2000);
        //data = 0;
        //comedi_dio_bitfield (dsc, 2, mask, &data);
        //msleep(2000);

        

        expected += period_counts;
	}

    msg.id = -1;
    rtf_put(0, &msg, sizeof(msg));

	//kfree(session);
    //daq_close_device ((void**) &dsc);
    comedi_close(dsc);

    rt_printk("RTAI_MODULE_TEST: end %d %ld\n", i, points);

    return;
}


static int rttask_init(void) {
    message msg;
    long p;

    rt_printk("RTAI_MODULE_TEST: Init.\n");

    if ( rtf_create(0, sizeof(msg) * 1000) != 0) {
        rt_printk("RTAI_MODULE_TEST: Failure to create synchronous FIFO.\n");
        return;
    }

    p = (1.0/freq) * NSEC_PER_SEC;
    rt_printk("RTAI_MODULE_TEST: p = %ld\n", p);
    period = nano2count(p);


    if (rt_task_init(&rt_get_data_task, rt_latency_loop, 0, 3000, 0, 0, 0) != 0) {
        rt_printk("RTAI_MODULE_TEST: Failure to create RT Thread.\n");
        return 1;
    }

    rt_printk("RTAI_MODULE_TEST: Created RT Thread.\n");


    rt_set_oneshot_mode();
    period_counts = start_rt_timer(period);
    if (period_counts != period) {
        rt_printk("RTAI_MODULE_TEST: fail start_rt_timer with period = %lld\n", period);
        return -1;
    }

    rt_printk("RTAI_MODULE_TEST: start_rt_timer with period = %lld.\n", period);

    expected = rt_get_time() + 10 * period_counts;

    /*Make it periodic*/
    if (rt_task_make_periodic(&rt_get_data_task, expected, period_counts) != 0) {
        rt_printk("RTAI_MODULE_TEST: rt_task_make_periodic\n");
        return -1;
    }

    rt_printk("RTAI_MODULE_TEST: end init.\n");


    return 0;
}// End rttask_init


static void rttask_exit(void)
{
    stop_rt_timer();
    rt_task_delete(&rt_get_data_task);

    if ( rtf_destroy(0) < 0) {
        rt_printk("RTAI_MODULE_TEST: Failure to destroy synchronous FIFO.\n");
    }

    rt_printk("RTAI_MODULE_TEST: Cleanup module function starting point\n");
}// End rttask_exit


module_init(rttask_init);
module_exit(rttask_exit);











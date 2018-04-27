
#include <comedilib.h>
#include "queue_functions.h"







/* RT Params */
#define MAX_SAFE_STACK (8*1024)
#define PRIORITY (99)
#define MAX_LAT (900000)
#define CORE (0)
#define NSEC_PER_SEC (1000000000)


typedef struct _Daq_session Daq_session;

struct _Daq_session{
	comedi_t * device;
	int in_subdev;		/*input subdevice */
	int out_subdev;		/*output subdevice */
	int dio_subdev;
	int range;			
	int aref;		
};

pthread_t rt, writer;
FILE * f;
void * msqid_rt = NULL, * msqid_nrt = NULL;


/**
 * @brief Substracts two times (as timespec structures).
 *
 * The start time is substracted from stop and the outcome is stored in result.
 * @param[in] start Pointer to a timespec structure with the minuend
 * @param[in] stop Pointer to a timespec structure with the subtrahend
 * @param[out] result Pointer to a timespec structure with the difference
 */
void ts_substraction (struct timespec * start, struct timespec * stop, struct timespec * result) {
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}


/**
 * @brief Assigns the time stored in a timespec structure to another one.
 *
 * @param[out] dst Pointer to the destination timespec structure 
 * @param[in] src Source timespec structure
 */
void ts_assign (struct timespec * dst,  struct timespec src) {
    dst->tv_sec = src.tv_sec;
    dst->tv_nsec = src.tv_nsec;
}


/**
 * @brief Adds a number of seconds and nanoseconds to a time stored in a timespec structure.
 *
 * @param[in,out] ts Pointer to a timespec structure
 * @param[in] sec Number of seconds to be added (must be equal or greater than zero)
 * @param[in] nsec Number of nanoseconds to be added (must be equal or greater than zero)
 */
void ts_add_time (struct timespec * ts, unsigned int sec, unsigned int nsec) {
    ts->tv_nsec += nsec;

    while (ts->tv_nsec >= NSEC_PER_SEC) {
          ts->tv_nsec -= NSEC_PER_SEC;
          ts->tv_sec++;
    }

    ts->tv_sec += sec;
}


void free_pointers (int n, ...) {
    va_list l;
    void ** arg;
    int i = 0;
    va_start(l, n);

    for(i = 0; i < n; i++) {
        //syslog(LOG_INFO, "Free %d", i);
        arg = va_arg(l, void**);
        if(*arg != NULL){
            free(*arg);
            *arg = NULL;
        }
    }
    va_end(l);
}


int daq_open_device (void ** device) {
	comedi_t * dsc;

	dsc = comedi_open("/dev/comedi0");
	if(dsc == NULL)
	{
		comedi_perror("/dev/comedi0");
		return ERR;
	}

	*device = dsc;

	return OK;
}


int daq_close_device (void ** device) {
	comedi_t * dsc = *device;

	if (comedi_close(dsc) == -1) {
		comedi_perror("Error with comedi_close");
		return ERR;
	}

	return OK;
}


int daq_create_session (void  ** device, Daq_session ** session_ptr) {
	Daq_session * session;
	*session_ptr = (Daq_session *) malloc (sizeof(Daq_session));
	session = *session_ptr;
	
	session->device = (comedi_t *) *device;
	session->range = 0; //get_range_comedi(device, subdev, chan, unit, min, max);
	session->aref = AREF_GROUND;


	session->in_subdev = comedi_find_subdevice_by_type(session->device, COMEDI_SUBD_AI, 0);
	if (session->in_subdev == -1) {
		comedi_perror("Error finding input subdevice");

		free(session);
		return ERR;
	}

	session->out_subdev = comedi_find_subdevice_by_type(session->device, COMEDI_SUBD_AO, 0);
	if (session->out_subdev == -1) {
		comedi_perror("Error finding output subdevice");

		free(session);
		return ERR;
	}


	session->dio_subdev = comedi_find_subdevice_by_type(session->device, COMEDI_SUBD_DIO, 0);
	if (session->dio_subdev == -1) {
		comedi_perror("Error finding DIO subdevice");

		free(session);
		return ERR;
	} else {
		 if (comedi_dio_config (session->device, session->dio_subdev, 0, COMEDI_OUTPUT) == -1) {
		 	comedi_perror("Error setting DIO subdevice");
			free(session);
		 }

		 if (comedi_dio_config (session->device, session->dio_subdev, 1, COMEDI_INPUT) == -1) {
		 	comedi_perror("Error setting DIO subdevice");
			free(session);
		 }
	}



	return OK;
}


int daq_digital_write (Daq_session * session, int n_channels, int * channels, unsigned int * bits) {
	int i;
	comedi_range * range_info;
	lsampl_t maxdata;
	lsampl_t comedi_value;
	unsigned int mask = 0x00000011;
	unsigned int data = bits[0];


	if (comedi_dio_bitfield2(session->device, session->dio_subdev, mask, bits, 0) != 0) {
		comedi_perror("bitfield1");
		return ERR;
	}


	/*data = 0;
	bits[0] = 0;
	if (comedi_dio_bitfield2(session->device, session->dio_subdev, mask, bits, 0) != 0) {
		comedi_perror("bitfield2");
		return ERR;
	}*/

    return OK;
}




void prepare_real_time (pthread_t id) {
    struct sched_param param;
    unsigned char dummy[MAX_SAFE_STACK];


    /* Set priority */
    param.sched_priority = PRIORITY;
    if(pthread_setschedparam(id, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed");
        exit(-1);
    }

    /* Set core affinity */
    /*cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(CORE, &mask);
    if (pthread_setaffinity_np(id, sizeof(mask), &mask) != 0) {
        perror("Affinity set failure\n");
        exit(-2);
    }*/

    /* Lock memory */

    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        exit(-3);
    }

    /* Pre-fault our stack */
    memset(dummy, 0, MAX_SAFE_STACK);

    return;
}


void * writer_thread(void * arg) {
    message msg;

    for (;;) {
        receive_from_queue(msqid_nrt, NRT_QUEUE, BLOCK_QUEUE, &msg);

        if (msg.id < 0) {
            break;
        } else {
            fprintf(f, "%s\n", msg.data);
        }
    }

    printf("writer thread end\n");

    pthread_exit(NULL);
}



void * rt_thread (void * arg) {
	int i;
	long points;
	pthread_t id;
	double freq = 20000.0;
	int duration = 10;
	long period;
    int count = -1, aux;

	double t_elapsed;                           /* In milliseconds */
    long lat;
    message msg;
    msg.id = 0;

	void * dsc = NULL;
	Daq_session * session = NULL;

	unsigned int bits[] = {0, 0};

	struct timespec ts_iter, ts_result, ts_start, ts_target;



	if (daq_open_device((void**) &dsc) != OK) {
        fprintf(stderr, "RT_THREAD: error opening device.\n");
        pthread_exit(NULL);
    }

    if (daq_create_session ((void**) &dsc, &session) != OK) {
        fprintf(stderr, "RT_THREAD: error creating DAQ session.\n");
        daq_close_device ((void**) &dsc);
        pthread_exit(NULL);
	}

	if (daq_digital_write(session, 1, 0, bits) != OK) {
        fprintf(stderr, "RT_THREAD: error writing to dio DAQ.\n");
        free_pointers(1, &session);
        daq_close_device ((void**) &dsc);
        pthread_exit(NULL);
    }


	id = pthread_self();
	prepare_real_time(id);


	points = duration * freq;
	period = (1.0 / freq) * NSEC_PER_SEC;

    


	clock_gettime(CLOCK_MONOTONIC, &ts_target);
    ts_assign(&ts_start,  ts_target);
	ts_add_time(&ts_target, 0, period);



	for (i = 0; i < points; i++) {
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts_target, NULL);
		/* Wake up and get times and latency */
        clock_gettime(CLOCK_MONOTONIC, &ts_iter);
        ts_substraction(&ts_target, &ts_iter, &ts_result);
        lat = ts_result.tv_sec * NSEC_PER_SEC + ts_result.tv_nsec;
        ts_substraction(&ts_start, &ts_iter, &ts_result);
        t_elapsed = (ts_result.tv_sec * NSEC_PER_SEC + ts_result.tv_nsec) * 0.000001;

		ts_add_time(&ts_target, 0, period);


        if (i % 200 == 0 && i > 0) {
            bits[0] = 1;
            aux = 0;
            count += 1;
            //printf("dentro de 200 %d\n", i);
        } else if (aux < count) {
            bits[0] = 1;
            aux += 1;
            //printf("dentro de aux %d %d %d\n", i, aux, count);
        } else if (i % 2 == 0) {
            bits[0] = 1;
        } else {
            bits[0] = 0;
        }

        /*if (i % 2) {
            bits[0] = 1;
        } else if ((i % 200 || i % 20 || i % 2 != 0){
            bits[0] = 0;
        }*/

		sprintf(msg.data, "%f %ld %d", t_elapsed, lat, bits[0]);
    	send_to_queue(msqid_rt, RT_QUEUE, NO_BLOCK_QUEUE, &msg);



		//bits[0] = 1;
        if (daq_digital_write(session, 1, 0, bits) != OK) {
            fprintf(stderr, "RT_THREAD: error writing to dio DAQ.\n");
            free_pointers(1, &session);
            daq_close_device ((void**) &dsc);
            pthread_exit(NULL);
        }

	}

    printf("RT thread end\n");

	msg.id = -1;
	send_to_queue(msqid_rt, RT_QUEUE, NO_BLOCK_QUEUE, &msg);

	free_pointers(1, &session);
	daq_close_device ((void**) &dsc);

	pthread_exit(NULL);
}







int main (int argc, char *argv[]) {
	pthread_attr_t attr_rt, attr_wr;
    int err;

    printf("Starting RT benchmarking\n");

    f = fopen(argv[1], "w");

    if (open_queue(&msqid_rt, &msqid_nrt) != OK) {
        syslog(LOG_INFO, "Error opening rt queue.");
    	return ERR;
    }


    pthread_attr_init(&attr_wr);
    pthread_attr_setdetachstate(&attr_wr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_init(&attr_rt);
    pthread_attr_setdetachstate(&attr_rt, PTHREAD_CREATE_JOINABLE);

    err = pthread_create(&writer, &attr_wr, &writer_thread, NULL);
    if (err != 0) {
        printf("Can't create writer_thread :[%s]", strerror(err));
        return -1;
    }

    err = pthread_create(&rt, &attr_rt, &rt_thread, NULL);
    if (err != 0) {
        printf("Can't create rt_thread :[%s]", strerror(err));
        return -1;
    }


    pthread_join(writer, NULL);
    pthread_join(rt, NULL);

    fclose(f);
    close_queue(&msqid_rt, &msqid_nrt);

    printf("RT benchmarking finished\n");

    return 1;
}

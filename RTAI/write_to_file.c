#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct {
    short id;
    int lat;
    double absol;
    int data;
} message;


int main (int argc, char *argv[]) {
	FILE * f;
	int fifo_id;
    int ret;
	long lat;
	long points = 10 * 10000;
	int i;
	message msg;
    msg.id = 0;

	f = fopen(argv[1], "w");
    
    fifo_id = open("/dev/rtf0", O_RDONLY);
    if (fifo_id == -1) {
        printf("RTAI_MODULE_TEST: Error opening fifo at user_space\n");
    }

    for (;;) {
		ret = read(fifo_id, &msg, sizeof(msg));

        if (ret == -1) perror("read");

        if (msg.id < 0) {
             printf("%d\n", msg.id);
             break;
        }

    	fprintf(f, "%f %d %d\n", msg.absol, msg.lat, msg.data);
	}

    
	fclose(f);

	return 0;
}

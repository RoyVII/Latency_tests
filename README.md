# RTOS benchmarking latency tests

Simple set of programs developed to benchmark the real-time performance of the following RTOS:
- RTAI 3.4
- Xenomai 2
- Xenomai 3
- Preempt-RT

It is recommended to use another program, like Sysbench, to overload the system to run the test under stress conditions.


### Developed by
Rodrigo Amaducci (rodrigo.amaducci@uam.es - [scholar](https://scholar.google.es/citations?user=Lq4ogOQAAAAJ))


## Usage

### RTAI
The program needs to be loaded as a kernel module, while a secondary write_to_file program will run in parallel, so in order to compile and run the test you must execute:

```  
sudo make
make -f Makefile_write
./write_to_file
sudo ./insmod test_module
``` 

If you want to modify the frequency or duration of the test you have to change the variables freq and t in test_modules.c. To change the name or path of the output file, you can do so in write_to_file.c.

When the test is finished, the module needs to be removed by running:
``` 
sudo rmmod test_module
```


### Xenomai
It can be compiled for Xenomai 2 or Xenomai 3 by using their corresponding Makefiles. Once compiled, it can be run with:
``` 
sudo ./xenomai
```

The frequency, duration or filename can be modified in xenomai.c.


### Preempt-RT
``` 
make -f Makefile_preempt
sudo ./preempt
```

The frequency, duration or filename can be modified in preempt.c.

# RTOS benchmarking latency tests

Simple set of programs developed to benchmark the real-time performance of the following RTOS:
- RTAI 3.4
- Xenomai 2
- Xenomai 3
- Preempt-RT

It is recommended to use another program, like stress (https://people.seas.harvard.edu/~apw/stress/), to overload the system to run the test under pressure conditions.


The tests consists in periodic iterations of a loop, during which the elapsed time of the test and the latency of that iteration will be sent through a FIFO queue to a writer thread that will store this values in a file, and voltage pulses with be sent to DAQ device so the times can also be measured with an external device (an oscilloscope, for example).


### Developed by
Rodrigo Amaducci (rodrigo.amaducci@uam.es - [scholar](https://scholar.google.es/citations?user=Lq4ogOQAAAAJ))


## Usage

### RTAI
The program needs to be loaded as a kernel module, while a secondary write_to_file program will run in parallel, so in order to compile and run the test you must execute:

```  
sudo make
make -f Makefile_write
./write_to_file <filename>
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
sudo ./xenomai <filename>
```

The frequency, duration or filename can be modified in xenomai.c.


### Preempt-RT
``` 
make -f Makefile_preempt
sudo ./preempt <filename>
```

The frequency, duration or filename can be modified in preempt.c.



### stress
```
stress --cpu 8 --io 4 --vm 2 --vm-bytes 128M --timeout <time>s
```
Timeout determines the duration of the workload in seconds.


## Dependencies
In order to send the pulses to the DAQ device, some drivers are required.

### Preempt-RT and RTAI
Comedi driver are used, and can be installed doing:
```
sudo apt install git build-essential dkms automake linux-headers-rt-amd64
git clone https://github.com/Linux-Comedi/comedi.git
cd comedi/
./autogen.sh
cd ..
sudo dkms add ./comedi
sudo dkms install comedi/0.7.76.1+20170502git-1.nodist
sudo depmod -a
sudo apt install libcomedi0 libcomedi-dev
```

### Xenomai
Xenomai Analogy drivers for DAQ devices, as well as RTIPC drivers for FIFO queues, must be enabled.
#include <linux/gpio.h> /* you may want to look at the contents of this file, to understand
 the various elements of the data structure shown in the code below */
#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
int main(int argc, char *argv[ ])
{ int fd0 = open("/dev/gpiochip0",O_RDWR); // open the file descriptor
 struct gpiochip_info cinfo;
 ioctl(fd0,GPIO_GET_CHIPINFO_IOCTL,&cinfo); // get the chip information
 fprintf(stdout, "GPIO chip 0: %s, \"%s\", %u lines\n", cinfo.name, cinfo.label, cinfo.lines);
struct gpiohandle_request req_GY, req_R; // Green and Yellow
struct gpiohandle_data data_GY, data_R; // for data bit
req_GY.lines = 2; // 2 pins in this handler
req_GY.lineoffsets[0] = 4; //pin 4 - Green LED
req_GY.lineoffsets[1] = 17; // pin 17 - Yellow LED
req_GY.flags = GPIOHANDLE_REQUEST_OUTPUT; // set them to be output
data_GY.values[0] = 1; // set initial value of Green LED to High (ON)
data_GY.values[1] = 0; // set initial value of Yellow LED to Low (OFF)
ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_GY);
req_R.lines = 1;
req_R.lineoffsets[0] = 27;
req_R.flags = GPIOHANDLE_REQUEST_OUTPUT;
req_R.default_values[0] = 1;
ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_R); 
for (int i = 0; i < 5; ++i){
 ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // output data bits
 usleep(1000000); //sleep for 1 second
 data_GY.values[0] = !data_GY.values[0]; // toggle
 data_GY.values[1] = !data_GY.values[1];
 } //for
data_R.values[0] = 0;
ioctl(req_R.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_R);

//push button
struct gpiohandle_request req_pb;
struct gpiohandle_data data_pb;

req_pb.lines = 1;
req_pb.lineoffsets[0] = 11;
req_pb.flags = GPIOHANDLE_REQUEST_INPUT;
ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_pb);
strcpy(req_pb.consumer_label, "Push Button");

int pb_value = 0;

for(int ii=0; ii<100; ii++){
        ioctl(req_pb.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_pb);
        printf("%s @ line %d is %s\n", req_pb.consumer_label, req_pb.lineoffsets[0], data_pb.values[0] ? "high" : "low");
        if(data_pb.values[0] != pb_value){
                data_R.values[0] = !data_R.values[0];
                ioctl(req_R.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_R);
        }
        pb_value = data_pb.values[0];
        data_GY.values[0] = !data_GY.values[0];
        data_GY.values[1] = !data_GY.values[1];
        ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY);
        usleep(500000);
} 

 close(req_GY.fd); // release line
 close(fd0); // close the file
 exit(EXIT_SUCCESS);
 }//main

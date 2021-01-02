#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>//fork()
#include <stdlib.h>
#include <errno.h>//errno, EINTR
#include <string.h>//strlen()
#include <fcntl.h>
#include <signal.h> 
#include <sys/types.h>  
#include <sys/stat.h>  
#include <linux/input.h>  
#include <linux/joystick.h> 
#include <errno.h>
#include <pthread.h>


#define AXES_LLR          0x00                 
#define AXES_LUD          0x01
#define AXES_RLR          0x02
#define AXES_RUD          0x03                  
#define BUTTON_X       0x00  
#define BUTTON_A       0x01                  
#define BUTTON_B       0x02                   
#define BUTTON_Y       0x03                


int axes0, axes1;
double x, y, vth;
int len, type, number, value ;
int ret0;
pthread_t id0;  
struct js_event js;  
int js_fd;

void * read_js_data(void *);

using namespace std;

int main(int argc, char **argv)
{

	//Initializing ROS node with a name of joymsg_pub
	ros::init(argc, argv,"logitech_joymsg_pub");
	//Created a nodehandle object
	ros::NodeHandle n1;
	//Create a publisher object
	ros::Publisher number_publisher = n1.advertise<geometry_msgs::Twist>("/cmd_vel",10);
	//Create a rate object
	ros::Rate loop_rate(100);
	//Variable of the number initializing as zero

	js_fd = open("/dev/input/js0", O_RDONLY);  
    if (js_fd < 0)  
    {  
        perror("open");  
        return -1;  
    }  

    pthread_create(&id0,NULL,read_js_data,NULL); // 成功返回0，错误返回错误编号
    if(ret0!=0)
        {
            printf ("Create pthread1 error!\n");
            exit(1);
        }
    ret0=pthread_detach(id0);

	while (ros::ok())
	{	

		//Created a Int32 message
	    geometry_msgs::Twist msg;
		msg.linear.x=x;
        msg.linear.y=y;
        msg.angular.z=vth;

        //Inserted data to message header
		//Printing message data
		ROS_INFO(" linear.x  linear.y angular.z : %f  %f   %f  ",msg.linear.x,msg.linear.y,msg.angular.z);

		//Publishing the message
		number_publisher.publish(msg);
		//Spining once for doing the  all operation once
		ros::spinOnce();
		//Setting the loop rate
		loop_rate.sleep();

	}
	
	return 0;
}

void * read_js_data(void *)
{

while(1)
{

    len = read(js_fd, &js, sizeof(struct js_event));  
    if (len < 0)  
    {  
        perror("read");  
        return 0 ;  
    }  

    value = js.value;
    type = js.type;
    number = js.number;

    /*if (type == JS_EVENT_BUTTON)  
    {  
        if( number == BUTTON_X  )
            {
                vth=value;  
            }
        
        if( number == BUTTON_B  )
            {
                vth=-1*value;  
            }
     }*/

    //else 
    if (type == JS_EVENT_AXIS)  
    {
        if( number == AXES_RLR )
        {
            vth = -1*value/32767.0;
        }
        if( number == AXES_LLR )
        {
            y = -1*value/32767.0;
        }
        if( number == AXES_LUD )
        {
            x = -1*value/32767.0;
        }
        
    }

} 

}





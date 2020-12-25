#include "snap7.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h> 
#include <sys/types.h>  
#include <sys/stat.h>  
#include <linux/input.h>  
#include <linux/joystick.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
using namespace std;

#define AXES_LLR       0x00                   
#define AXES_RUD       0x03                   
#define BUTTON_X       0x00  
#define BUTTON_A       0x01                   
#define BUTTON_B       0x02                   
#define BUTTON_Y       0x03 
#define BUTTON_START   0x09

static inline int16_t bswape_16(int16_t x)
{
	return (x >> 8) | (x << 8);
}

static inline int32_t bswape_32(int32_t x)
{
	x = ((x << 8) & 0xFF00FF00 | (x >> 8) & 0xFF00FF00);
	return (x >> 16) | (x << 16);
}

struct Joy_info_struct {
	bool A = false;
	bool B = false;
	bool X = false;
	bool Y = false;
	// bool LB = false;
	// bool RB = false;
	// bool LT = false;
	// bool RT = false;
	// bool BACK = false;
	bool START = false;
    short X_axis = 0;
    short Y_axis = 0;
	// unsigned short U_axis = 0;
	// unsigned short V_axis = 0;
} joy_info;

static unsigned short spin_position=1000;
static unsigned short spin_speed=0;
static unsigned char sport_mode=0;

void * read_js_data(void *);
int len, type, number, value ,js_fd;
struct js_event js;  

int main(int argc, char** argv)
{
    const char* plc_ip = "192.168.1.41";
	TS7Client* snap7_client = new TS7Client();
	if (snap7_client->ConnectTo(plc_ip, 0, 0)!= 0) {
		cout << "plc connect fail ..." << endl;
		return -1;
	}

	//0x03 for position and speed
	const unsigned char position_speed = 3;
	snap7_client->DBWrite(1, 0, 1, (void *)&position_speed);

    js_fd=open("/dev/input/js0",O_RDONLY);
    if(js_fd<0)
    {
        cout<<"open js_input device failure..."<<endl;
        return -1;
    }

    pthread_t pth_read;
    if(pthread_create(&pth_read,NULL,read_js_data,NULL)!=0)
    {
        cout<<"creat pthread for js_input read failure..."<<endl;
        exit(-1);
    }
    pthread_detach(pth_read);
    cout<<"pthread_detach ...\nuse Ctrl+C to cancel this procession..."<<endl;

    while(true)
    {
        if (joy_info.A == 1)
		{
			sport_mode = 1;
			snap7_client->DBWrite(1, 42, 1, &sport_mode);
		}	
		if (joy_info.B == 1)
		{
			sport_mode = 2;
			snap7_client->DBWrite(1, 42, 1, &sport_mode);
		}
		if (joy_info.X == 1)
		{
			sport_mode = 3;
			snap7_client->DBWrite(1, 42, 1, &sport_mode);
		}
		if (joy_info.Y == 1)
		{
			sport_mode = 4;
			snap7_client->DBWrite(1, 42, 1, &sport_mode);
		}
		if (joy_info.START == 1)
		{
			sport_mode = 0;
			snap7_client->DBWrite(1, 42, 1, &sport_mode);
		}

		cout << "mode: " << (int)sport_mode;
        if (sport_mode != 4)
		{
			if (joy_info.X_axis > 100)
				spin_position = 1000+(joy_info.X_axis - 100) * 0.01525 ;
			else if (joy_info.X_axis < -100)
				spin_position = 1000 + (joy_info.X_axis+100) * 0.01525;
			else
				spin_position = 1000;

            cout << " position : " << spin_position;
			spin_position = bswape_16(spin_position);
			snap7_client->DBWrite(1, 2, 2, &spin_position);
		}

        if (joy_info.Y_axis > 100)
			spin_speed = (unsigned short)65535 - 0.5436 * joy_info.Y_axis;
		else if (joy_info.Y_axis < -100)
            spin_speed = -0.5436 * joy_info.Y_axis;
		else
			spin_speed = 0;
		
		cout << " speed : " << spin_speed << endl;
		spin_speed = bswape_16(spin_speed);
		snap7_client->DBWrite(1, 76, 2, &spin_speed);
    }

    snap7_client->Disconnect();
    cout<<"work finished...."<<endl;
    return 0;
}

void * read_js_data(void *)
{
    while(true)
    {
        len = read(js_fd, &js, sizeof(struct js_event));  
        if (len < 0)  
        {  
            cout<<"loop read js_statu failure..."<<endl;  
            return 0;  
        }
        value = js.value;
        type = js.type;
        number = js.number; 
        if (type == JS_EVENT_BUTTON)  
        {
            if( number == BUTTON_X  )
            {
                joy_info.X=1;  
            }else
            {
                joy_info.X=0; 
            }
            
            if( number == BUTTON_B  )
            {
                joy_info.B=1;  
            }
            else
            {
                joy_info.B=0; 
            }

            if( number == BUTTON_A  )
            {
                joy_info.A=1;  
            }
            else 
            {
                joy_info.A=0;  
            }

            if( number == BUTTON_Y  )
            {
                joy_info.Y=1;  
            }
            else
            {
                joy_info.Y=0; 
            }
            if(number == BUTTON_START)
            {
                joy_info.START=1;  
            }
            else
            {
                joy_info.START=0; 
            }
                       
        }  
        else if(type == JS_EVENT_AXIS)
        {
            if( number == AXES_RUD )
            {
                joy_info.Y_axis=value;
            }
            if( number == AXES_LLR )
            {
                joy_info.X_axis=value;
            }
        }
    }
}
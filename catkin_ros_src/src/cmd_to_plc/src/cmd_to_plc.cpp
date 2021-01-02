#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include "snap7.h"

#include <iostream>
using namespace std;

const double _pi=180.0/3.1415926;
const double _coder_resolution=6.944; //=2500线/360度;

TS7Client client;
const char* ip="192.168.1.41";

Eigen::Matrix<double,8,3> coff_mat;
Eigen::Matrix<double,3,8> coff_mat_inv;

const static double pi=3.1415926;
//front to back distance half
const double half_lena=0.32;
//left ro right distance half
const double half_widb=0.195;

Eigen::Matrix<double,8,1> P_vxy;
float v1,v2,v3,v4;
float th1,th2,th3,th4;


const short MAX_SPEED=10000;
const short MAX_ANGEL=1000;

double angle2radian(double jd)
{
  return pi*jd/180;
};

Eigen::MatrixXd pinv(Eigen::MatrixXd A)
{
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A,Eigen::ComputeFullU|Eigen::ComputeFullV);
    double pinvtoler=1e-8;
    int row=A.rows();
    int col=A.cols();
    int k=(row>col?col:row);

    Eigen::MatrixXd X=Eigen::MatrixXd::Zero(col,row);
    Eigen::MatrixXd singularValues_inv=svd.singularValues();
    Eigen::MatrixXd singularValues_inv_mat=Eigen::MatrixXd::Zero(col,row);

    for(int i=0;i<k;++i)
    {
        if(singularValues_inv(i)>pinvtoler)
        singularValues_inv(i)=1.0 / singularValues_inv(i);
        else
        singularValues_inv(i)=0;
    }
    
    for(int i=0;i<k;i++)
    {
        singularValues_inv_mat(i,i)=singularValues_inv(i);
    }

    X=svd.matrixV()*singularValues_inv_mat*svd.matrixU().transpose();
    return X;
};

static inline uint16_t bswape_16(uint16_t x)
{
    return (x>>8)|(x<<8);
} 

static inline uint32_t bswape_32(uint32_t x)
{
    x=((x<<8)&0xFF00FF00 | (x>>8)& 0xFF00FF00);
    return (x>>16)|(x<<16);
}


void callbackmodel(const geometry_msgs::Twist::ConstPtr& msg)
{
    double vx=msg->linear.x;
    double vy=msg->linear.y;
    double vth=msg->angular.z;
    int model= (int)msg->linear.z;

    
}

void callback(const geometry_msgs::Twist::ConstPtr& msg)
{
    
    Eigen::Matrix<double,3,1> vxy;
    vxy<<msg->linear.x,msg->linear.y,msg->angular.z;
    //vxy<<0,2,0;
    P_vxy=coff_mat*vxy;

    //cout<<P_vxy<<endl;

    double _A=pow(P_vxy(0,0),2);
    double _B=pow(P_vxy(1,0),2);
    double _C=pow(P_vxy(4,0),2);
    double _D=pow(P_vxy(3,0),2);
    
    th1=th2=th3=th4=0;

    v1=sqrt(_A+_B);
    if(P_vxy(0,0)<0) 
        v1=-1.0*v1;  

    if(v1!=0)
        th1=_pi*asin(P_vxy(1,0) / v1);
    

    v2=sqrt(_A+_D);
    if(P_vxy(2,0)<0) 
         v2=-1.0*v2;

    if(v2!=0)
        th2=_pi*asin(P_vxy(3,0) / v2);

    v3=sqrt(_C+_D);
    if(P_vxy(4,0)<0)
        v3=-1*v3;
        
    if(v3!=0)
        th3=_pi*asin(P_vxy(3,0) / v3);

    v4=sqrt(_C+_B);
   if(P_vxy(6,0)<0)
        v4=-1*v4;
    if(v4!=0)
        th4=_pi*asin(P_vxy(1,0) / v4);
         
    
     cout<<"v1 v2 v3 v4:"<<v1<<" "<<v2<<" "<<v3<<" "<<v4<<endl;
     cout<<"th1 th2 th3 th4:"<<th1<<" "<<th2<<" "<<th3<<" "<<th4<<endl;
    
    //write to plc

    uint16_t buf[]={
        (v1*MAX_SPEED),
        1000-(th1*_coder_resolution),

        (v2*MAX_SPEED),
        1000-(th2*_coder_resolution),
        
        (v3*MAX_SPEED),
        1000-(th3*_coder_resolution),
        
        (v4*MAX_SPEED),
        1000-(th4*_coder_resolution)
    };

    for(int i=0;i<8;i++)
    {
        cout<<hex<<buf[i]<<"  ";
        buf[i]=bswape_16(buf[i]);
        cout<<hex<<buf[i]<<"  ";
    }
    cout<<endl;



    //left_front speed angle
    client.DBWrite(1,76,2,&buf[0]);
    client.DBWrite(1,2,2,&buf[1]);

    //left_back  speed angle
    client.DBWrite(1,80,2,&buf[2]);
    client.DBWrite(1,22,2,&buf[3]);

    //right_back  speed angle
    client.DBWrite(1,82,2,&buf[4]);
    client.DBWrite(1,32,2,&buf[5]);

    //right_front speed angle
    client.DBWrite(1,78,2,&buf[6]);
    client.DBWrite(1,12,2,&buf[7]);


}


int main(int argc, char *argv[])
{
   
    //plc connect
    client.ConnectTo(ip,0,1);  
    if(!client.Connected())
    {
        cout<<"error return in "<<__FILE__<<" "<<__LINE__<<":erron type connect failed"<<endl;
        return -1;
    }else
    {
        cout<<"connect success"<<endl;
    }

    // Initialize
    int initial=0x03;
    client.DBWrite(1,0,1,&initial);

    //mode
    int free_mode=0x05;
    client.DBWrite(1,42,1,&free_mode);
    
    
    //spid_speed 50 100 150 200  250 
    uint32_t spin_speed =150;
    spin_speed=bswape_32(spin_speed);
    client.DBWrite(1,4,4,&spin_speed);
    
    //robot_model_arg_matrix
    coff_mat<<  1,0,-half_widb,
                0,1, half_lena,
                1,0,-half_widb,
                0,1,-half_lena,
                1,0, half_widb,
                0,1,-half_lena,
                1,0, half_widb,
                0,1, half_lena;

    //ros
    ros::init(argc, argv, "cmd_to_plc");
    ros::NodeHandle nh;
    ros::Subscriber sub=nh.subscribe("cmd_vel",1,callback);
    ros::spin();
    return 0;
}

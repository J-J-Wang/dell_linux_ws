#!/usr/bin/env python


import time
import serial       
import binascii
import rospy
from geometry_msgs.msg import Quaternion, Vector3
from sensor_msgs.msg import Imu
import codecs

s=serial.Serial("/dev/ttyUSB0",115200)
rospy.init_node("imu")
imuPub = rospy.Publisher("imu", Imu, queue_size=1)
rate=rospy.Rate(100)

Hex_str=codecs.decode('770500560560','hex')
s.write(Hex_str)

data= str(binascii.b2a_hex(s.read(6)))
#print(data)
print('AUTO PUT DATA TYPE SETTING SUCCESS')

Hex_str=codecs.decode('7705000C0617','hex')
s.write(Hex_str)
data=str(binascii.b2a_hex(s.read(6)))
#print(data)
print('SET 100Hz OUTPUT FREQUNSE')

try:
    while not rospy.is_shutdown():
        data= str(binascii.b2a_hex(s.read(48)))
        g_value=data[26:44]
        a_value=data[44:62]
#        print(data)
#        print(g_value)
#        print(a_value)

        #x_acc   
        x_acc=9.87933*int(g_value[1:6])/10000.0
        if int(g_value[0]):
            x_acc=-1*x_acc

        #y_acc 
        y_acc=9.87933*int(g_value[7:12])/10000.0
        if int(g_value[6]) :
            y_acc=-1*y_acc

        #z_acc  
        z_acc=9.87933*int(g_value[13:])/10000.0
        if int(g_value[12]):
            z_acc=-1*z_acc
    

        #an_x  
        an_x=int(a_value[1:6])/100.0/57.29578
        if int(a_value[0]):
            an_x=-1*an_x

        #an_y
        an_y=int(a_value[7:12])/100.0/57.29578
        if int(a_value[6]):
            an_y=-1*an_y

        #an_z
        an_z=int(a_value[13:])/100.0/57.29578
        if int(a_value[12]):
            an_z=-1*an_z

#        print('acc:x,y,z:',x_acc,y_acc,z_acc)
#        print('an_acc:x,y,z:',an_x,an_y,an_z)
    
        imuMsg = Imu()
        stamp = rospy.get_rostime()
        imuMsg.header.stamp, imuMsg.header.frame_id = stamp, "imu_link"
        #imuMsg.orientation = eul_to_qua(ef_l)
        #imuMsg.orientation.x=q1
        #imuMsg.orientation.y=q2
        #imuMsg.orientation.z=q3
        #imuMsg.orientation.w=q4

        
        #imuMsg.orientation_covariance = cov_orientation
        imuMsg.angular_velocity.x, imuMsg.angular_velocity.y, imuMsg.angular_velocity.z = -1.0*an_y,-1.0*an_x,-1.0*an_z
        #imuMsg.angular_velocity_covariance = cov_angular_velocity
        imuMsg.linear_acceleration.x, imuMsg.linear_acceleration.y, imuMsg.linear_acceleration.z = -1.0*x_acc,y_acc,-1.0*z_acc
        #imuMsg.linear_acceleration_covariance = cov_linear_acceleration
        imuPub.publish(imuMsg)
        rate.sleep()
except:
    Hex_str=codecs.decode('7705000C0011','hex')
    s.write(Hex_str)
    data= str(binascii.b2a_hex(s.read(6)))
    print('ANSER TYPE SETTING SUCCESS')
    pass


    

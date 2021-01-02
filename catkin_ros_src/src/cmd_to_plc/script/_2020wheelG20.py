#!/usr/bin/env python3
# -*- coding:utf-8 -*-
#author:ys   time:2019/8/8
import snap7
import struct

'''连接PLC'''
plc=snap7.client.Client()
plc.connect('192.168.0.1',0,1)
ret=True
#with open(r'E:\coder test\build-joystick-Desktop_Qt_5_5_0_MinGW_32bit-Debug\joy_keyinfo.txt','r') as f:
while True:
        #g_wheel
        with open(r'E:\qtcode\src\build-joystick-Desktop_Qt_5_5_0_MinGW_32bit-Debug\joy_keyinfo.txt','r') as f:
        #xbox
        #with open(r'D:\work_ws\build-joystick-Desktop_Qt_5_5_0_MinGW_32bit-Debug\xbox_keyinfo.txt','r') as f:     
                rows= f.read()
                try:
                        print("working")
                        arr=rows.split(' ')
                        
                        #上电初始化
                        #置位开关
                        plc.db_write(1,0,b'\x03')                                                 
                             
                        ####进行档位模式操作####
                        #档位操作 进行 模式
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地
                        #无档位、前轮、原地、四轮同、四轮异。                      
                        #改进，每次换挡，进入对应的挡位回调循环
                        
                        if arr[0]=='0': 
                            plc.db_write(1,42,b'\x00')                                                           
                            #四个目标位置为1000，旋转速度为20                         

                        ####################################################
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地#
                        ####################################################                         
                        if arr[0]=='1':
                            plc.db_write(1,42,b'\x01')                          
                            ####进行方向位置操作####
                            #方向盘确定跟随角度
                            theta=int(arr[1])     
                            if    0<theta<2000:                                 
                                    data1 = theta >> 8
                                    data2 = theta & 255
                                    plc.write_area(0x84, 1, 2, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 3, struct.pack('B', data2))                                                     
                            #####进行速度大小操作#### 
                            #正/反速度输入，刹车制动 (数据写入前完成)                     
                            forward=int(arr[2])
                            back=int(arr[3])
                            if   forward>1 and back==65535:                           
                                    data1 = forward >> 8
                                    data2 = forward & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            elif back<65534 and forward==0:
                                    data1 = back >> 8
                                    data2 = back & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            else:
                                    plc.db_write(1,8,b'\x00\x00')


                        ####################################################
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地#
                        ####################################################
                        if arr[0]=='2':
                            plc.db_write(1,42,b'\x02')
                            ####进行方向位置操作####
                            #方向盘确定跟随角度
                            theta=int(arr[1])     
                            if    0<theta<2000:                                 
                                    data1 = theta >> 8
                                    data2 = theta & 255
                                    plc.write_area(0x84, 1, 2, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 3, struct.pack('B', data2))                                                        
                            #####进行速度大小操作#### 
                            #正/反速度输入，刹车制动 (数据写入前完成)                     
                            forward=int(arr[2])
                            back=int(arr[3])
                            if   forward>1 and back==65535:                           
                                    data1 = forward >> 8
                                    data2 = forward & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            elif back<65534 and forward==0:
                                    data1 = back >> 8
                                    data2 = back & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            else:
                                    plc.db_write(1,8,b'\x00\x00')


                        ####################################################
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地#
                        ####################################################
                        if arr[0]=='3':
                            plc.db_write(1,42,b'\x03')
                            ####进行方向位置操作####
                            #方向盘确定跟随角度
                            theta=int(arr[1])     
                            if    0<theta<2000:                                 
                                    data1 = theta >> 8
                                    data2 = theta & 255
                                    plc.write_area(0x84, 1, 2, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 3, struct.pack('B', data2))                                                       
                            #####进行速度大小操作#### 
                            #正/反速度输入，刹车制动 (数据写入前完成)                     
                            forward=int(arr[2])
                            back=int(arr[3])
                            if   forward>1 and back==65535:                           
                                    data1 = forward >> 8
                                    data2 = forward & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            elif back<65534 and forward==0:
                                    data1 = back >> 8
                                    data2 = back & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            else:
                                    plc.db_write(1,8,b'\x00\x00')

                        ####################################################
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地#
                        ####################################################
                        if arr[0]=='4':
                            plc.db_write(1,42,b'\x04')                            
                            #####进行速度大小操作#### 
                            #正/反速度输入，刹车制动 (数据写入前完成)                     
                            forward=int(arr[2])
                            back=int(arr[3])
                            if   forward>1 and back==65535:                           
                                    data1 = forward >> 8
                                    data2 = forward & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            elif back<65534 and forward==0:
                                    data1 = back >> 8
                                    data2 = back & 255
                                    plc.write_area(0x84, 1, 8, struct.pack('B', data1))
                                    plc.write_area(0x84, 1, 9, struct.pack('B', data2))
                            else:
                                    plc.db_write(1,8,b'\x00\x00')
                        ####################################################
                        #选择运动模式，01为四轮02为四轮异向03为前轮04为原地#
                        ####################################################
                        #05   06   07 数字未分配#
                        if arr[0]=='5':
                            plc.db_write(1,42,b'\x05')
                                                 
                except:
                       continue



                        

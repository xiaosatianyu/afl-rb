#!/usr/bin/python
#coding=utf-8

import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import matplotlib
import os
import shutil
import logging
# from reportlab.lib.styles import LineStyle
l = logging.getLogger("make_afl_pic")



def get_plotdata(plot_dir,max_time):
    if not os.path.exists(plot_dir):
        return None
    x0=[0]
    x1=[0]
    x2=[0]
    x3=[0]
    x4=[0]
    x5=[0]
    x6=[0]
    x7=[0]
    x8=[0]
    x9=[0]
    x10=[0]
    with open(plot_dir) as f:
        lines = f.read()
        line=lines.split('\n')[1:-1]
        for row in line:
            if int(row.split(',')[0]) > max_time:
                break
            x0.append( row.split(',')[0])
            x1.append( row.split(',')[1])
            x2.append( row.split(',')[2])
            x3.append( row.split(',')[3])
            x4.append( row.split(',')[4])
            x5.append( row.split(',')[5])
            x6.append( row.split(',')[6])
            x7.append( row.split(',')[7])
            x8.append( row.split(',')[8])
            x9.append( row.split(',')[9])
            x10.append( row.split(',')[10])
            
    x0 = np.array( x0 ) #unix_time
    x1 = np.array( x1 ) #cycles_done
    x2 = np.array( x2 ) #cur_path
    x3 = np.array( x3 ) #paths_total
    x4 = np.array( x4 ) #pending_total
    x5 = np.array( x5 ) #pending_favs
    x6 = np.array( x6 ) #map_size
    x7 = np.array( x7 ) #unique_crashes
    x8 = np.array( x8 ) #unique_hangs
    x9 = np.array( x9 ) #max_depth
    x10 = np.array( x10 ) #execs_per_sec    
    return [x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10]    


def plot():
    print "startyyyyy"
    out_put_dir="/home/xiaosatianyu/Desktop/driller-desk/picture"
    data_dir="/tmp/driller"
    data_dir="/home/xiaosatianyu/Desktop/server/xiaosa-tmp/driller"
    pros_dir="/home/xiaosatianyu/Desktop/driller-desk/binary-cgc"
    max_time=60*50
		
    #picture output dir
    if os.path.exists(out_put_dir):
        shutil.rmtree(out_put_dir)
    os.mkdir(out_put_dir)
        
    #driller data input dir
    if not os.path.exists(out_put_dir):
        print "error,there is no input data"
        l.error("input_from is error")
          
    #the list of cgc program
    pros=[]
    for pro in os.listdir(pros_dir):
        if '#' not in pro:
            pros.append(pro)
    pros.sort()
    
    
    for pro in pros:
        #path_save_file_path
        path_save_file_path=os.path.join(out_put_dir,pro)
        crash_save_file_path=os.path.join(out_put_dir,pro+"crash")
    
        #get the plot_data path
        # strategy 0
        plot0=os.path.join(data_dir,pro+'#0','sync/fuzzer-master','plot_data')
        # strategy 1
        plot1=os.path.join(data_dir,pro+'#1','sync/fuzzer-master','plot_data')
        # strategy 2
        plot2=os.path.join(data_dir,pro+'#2','sync/fuzzer-master','plot_data')
        # strategy 3
        plot3=os.path.join(data_dir,pro+'#3','sync/fuzzer-master','plot_data')
        # strategy 4
        plot4=os.path.join(data_dir,pro+'#4','sync/fuzzer-master','plot_data')
        # strategy 5
        plot5=os.path.join(data_dir,pro+'#5','sync/fuzzer-master','plot_data')
        # strategy 6
        plot6=os.path.join(data_dir,pro+'#6','sync/fuzzer-master','plot_data')
        # strategy 7
        plot7=os.path.join(data_dir,pro+'#7','sync/fuzzer-master','plot_data')
        # strategy 8
        plot8=os.path.join(data_dir,pro+'#8','sync/fuzzer-master','plot_data')
        
        #get the plot_data
        plot0_data=get_plotdata(plot0,max_time)
        plot1_data=get_plotdata(plot1,max_time)
        plot2_data=get_plotdata(plot2,max_time)
        plot3_data=get_plotdata(plot3,max_time)
        plot4_data=get_plotdata(plot4,max_time)
        plot5_data=get_plotdata(plot5,max_time)
        plot6_data=get_plotdata(plot6,max_time)
        plot7_data=get_plotdata(plot7,max_time)
        plot8_data=get_plotdata(plot8,max_time)
    
        ##make the picture
        #路径发现速度
        #单独一个图片
        if 1:
            plt.figure(figsize=(9,4))#新建一个图片
            if  plot0_data  is  not None:
                plt.plot(plot0_data[0],plot0_data[3],color='b',label='driller_fast_0',lw=2) #marker="s" linestyle=':'ls='-.'
            if  plot1_data  is  not None:
                plt.plot(plot1_data[0],plot1_data[3],color='g',label='RS',lw=2) #marker="s" linestyle=':'    
            if  plot2_data  is  not None:
                plt.plot(plot2_data[0],plot2_data[3],color='r',label='BT1',lw=2) #marker="s" linestyle=':'
            if  plot3_data  is  not None:
                plt.plot(plot3_data[0],plot3_data[3],color='c',label='BT2',lw=2) #marker="s" linestyle=':'
            if  plot4_data  is  not None:
                plt.plot(plot4_data[0],plot4_data[3],color='m',label='BAS',lw=2) #marker="s" linestyle=':'
            if  plot5_data  is  not None:
                plt.plot(plot5_data[0],plot5_data[3],color='y',label='MIS',lw=2) #marker="s" linestyle=':'
    #         if  plot6_data  is  not None:
    #             plt.plot(plot6_data[0],plot6_data[3],color='k',label='Short_first_nodup_6',lw=2) #marker="s" linestyle=':'
            if  plot7_data  is  not None:
                plt.plot(plot7_data[0],plot7_data[3],color='gold',label='MAS',lw=2) #marker="s" linestyle=':'
            if  plot8_data  is  not None:
                plt.plot(plot8_data[0],plot8_data[3],color='olive',label='Driller-deault',lw=2,ls='-.') #marker="s" linestyle=':'
            
            plt.legend(loc='best',fontsize=10,shadow=True,framealpha=True,borderpad=False,labelspacing=False)
            plt.ylabel('num of path',fontsize=12)
            plt.xlabel("second",fontsize=12)
            plt.title(pro,fontsize=12)
            plt.savefig(path_save_file_path,edgecolor=None,transparent=False,borderpad=False,borderaxespad=False) #facecolor=None,
            plt.close()
            print "%s is ok"%pro
        
        #  一张两图
        #路径
        if 0:
            tag=211
            a1=plt.subplot(tag)
#             if  plot0_data  is  not None:
#                 a1.plot(plot0_data[0],plot0_data[3],color='b',label='driller_fast_0',lw=2) #marker="s" linestyle=':'ls='-.'
            if  plot1_data  is  not None:
                a1.plot(plot1_data[0],plot1_data[3],color='g',label='RS',lw=2) #marker="s" linestyle=':'    
            if  plot2_data  is  not None:
                a1.plot(plot2_data[0],plot2_data[3],color='r',label='BT1',lw=2) #marker="s" linestyle=':'
            if  plot3_data  is  not None:
                a1.plot(plot3_data[0],plot3_data[3],color='c',label='BT2',lw=2) #marker="s" linestyle=':'
            if  plot4_data  is  not None:
                a1.plot(plot4_data[0],plot4_data[3],color='m',label='BAS',lw=2) #marker="s" linestyle=':'
            if  plot5_data  is  not None:
                a1.plot(plot5_data[0],plot5_data[3],color='y',label='MIS',lw=2) #marker="s" linestyle=':'
    #         if  plot6_data  is  not None:
    #             a1.plot(plot6_data[0],plot6_data[3],color='k',label='Short_first_nodup_6',lw=2) #marker="s" linestyle=':'
            if  plot7_data  is  not None:
                a1.plot(plot7_data[0],plot7_data[3],color='gold',label='MAS',lw=2) #marker="s" linestyle=':'
            if  plot8_data  is  not None:
                a1.plot(plot8_data[0],plot8_data[3],color='olive',label='Driller-deault',lw=2,ls='-.') #marker="s" linestyle=':'
            
            plt.legend(loc='best',fontsize=8,shadow=True,framealpha=True,borderpad=False,labelspacing=False)
            plt.ylabel('num of path',fontsize=12)
            plt.xlabel("second",fontsize=12)
            plt.title(pro,fontsize=12)
            plt.savefig(path_save_file_path,edgecolor=None,transparent=False,borderpad=False,borderaxespad=False) #facecolor=None,
            plt.close()
            print "%s is ok"%pro
        
        
        #crash发现速度
        #单独一个图片
        if 1:
            plt.figure(figsize=(9,4))#新建一个图片
            if  plot0_data  is  not None:
                plt.plot(plot0_data[0],plot0_data[7],color='b',label='driller_fast_0',lw=2) #marker="s" linestyle=':'ls='-.'
            if  plot1_data  is  not None:
                plt.plot(plot1_data[0],plot1_data[7],color='g',label='RS',lw=2) #marker="s" linestyle=':'    
            if  plot2_data  is  not None:
                plt.plot(plot2_data[0],plot2_data[7],color='r',label='BT1',lw=2) #marker="s" linestyle=':'
            if  plot3_data  is  not None:
                plt.plot(plot3_data[0],plot3_data[7],color='c',label='BT2',lw=2) #marker="s" linestyle=':'
            if  plot4_data  is  not None:
                plt.plot(plot4_data[0],plot4_data[7],color='m',label='BAS',lw=2) #marker="s" linestyle=':'
            if  plot5_data  is  not None:
                plt.plot(plot5_data[0],plot5_data[7],color='y',label='MIS',lw=2) #marker="s" linestyle=':'
    #         if  plot6_data  is  not None:
    #             plt.plot(plot6_data[0],plot6_data[7],color='k',label='Short_first_nodup_6',lw=2) #marker="s" linestyle=':'
            if  plot7_data  is  not None:
                plt.plot(plot7_data[0],plot7_data[7],color='gold',label='MAS',lw=2) #marker="s" linestyle=':'
            if  plot8_data  is  not None:
                plt.plot(plot8_data[0],plot8_data[7],color='olive',label='Driller-deault',lw=2,ls='-.') #marker="s" linestyle=':'
            
            plt.legend(loc='best',fontsize=10,shadow=True,framealpha=True,borderpad=False,labelspacing=False)
            plt.ylabel('num of path',fontsize=12)
            plt.xlabel("second",fontsize=12)
            plt.title(pro,fontsize=12)
            plt.savefig(crash_save_file_path,edgecolor=None,transparent=False,borderpad=False,borderaxespad=False) #facecolor=None,
            plt.close()
            print "%s is ok"%pro
    
    print "end"
    
if __name__ == "__main__":
    sys.exit(plot())

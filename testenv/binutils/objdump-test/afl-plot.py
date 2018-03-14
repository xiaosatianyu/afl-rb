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
    x0=[]
    x12=[] #the  min_distance
    starttime=-1
    with open(plot_dir) as f:
        lines = f.read()
        line=lines.split('\n')[1:-1]
        for row in line:
            if (starttime==-1):
                starttime=int(row.split(',')[0])    
            if (int(row.split(',')[0]) > max_time & max_time>0):
                break
            x0.append( (int(row.split(',')[0])-starttime)/60 )      #minite   
            x12.append( int(row.split(',')[12]) )
            
    x0 = np.array( x0 ) #unix_time
    x12 = np.array( x12 ) # min_distance
    return [x0,x12]    


def plot():
    l.info("start-plot")
    cur_dir=os.path.abspath(os.curdir)
    out_put_dir=cur_dir
    data_dir=os.path.join(cur_dir, "good_result")
    target_path=os.path.join(out_put_dir, "aflgo-demo.png")
    max_time=0
		
    if not os.path.exists(data_dir):
        print "error,there is no input data"
        l.error("input_from is error")
          
    plot_aflgo=os.path.join(data_dir,'plot_data_aflgo')
    plot_rd=os.path.join(data_dir, 'plot_data_rd4')
    
    #get the plot_data
    plot_aflgo_data=get_plotdata(plot_aflgo,max_time)
    plot_rd_data=get_plotdata(plot_rd,max_time)

    ##make the picture
    #最小距离速度
    #单独一个图片
    #plt.figure(figsize=(9,4))#新建一个图片
    plt.figure()#新建一个图片
    if  plot_aflgo_data  is  not None:
        plt.plot(plot_aflgo_data[0],plot_aflgo_data[1],color='b',label='AFLGo',lw=2) #marker="s" linestyle=':'ls='-.'
    if  plot_rd_data  is  not None:
        plt.plot(plot_rd_data[0],plot_rd_data[1],color='g',label='RDFuzz',lw=2) #marker="s" linestyle=':'    
        
    plt.legend(loc='best',fontsize=18,shadow=False,framealpha=False,borderpad=True,labelspacing=False)
    plt.ylabel('mini-distance',fontsize=18)
    plt.xlabel("Time(minites)",fontsize=18)
    plt.title("AFLGo-demo",fontsize=18)
    #plt.show()
    plt.savefig(target_path,edgecolor=None,transparent=False,borderpad=False,borderaxespad=False) #facecolor=None,
    plt.close()
    
    l.info("ok---")
    
    
if __name__ == "__main__":
    sys.exit(plot())

import socket
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from matplotlib import style

num_of_sample = 60
t = np.arange(0,num_of_sample,1)

sample = np.empty(0,dtype='int')


"""
f = open("rssi.txt", "r")
for i in range(0,num_of_sample):
    rx = f.readline()
    print(int(rx[0:2]))
    sample = np.append(sample,int(rx[0:2]))

print("done")
print(sample)

plt.plot(t,sample)
plt.show()
"""
local_ip = "192.168.43.240"
local_port = 1234
buff_size = 32

udpserversckt = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
udpserversckt.bind((local_ip,local_port))

print("UDP server is ready")
"""
#open a file
f = open("rssi.txt", "w")

i=0

while(num_of_sample != 0):
    msg_addrr = udpserversckt.recvfrom(32)
    i = i +1
    
    
    print(msg_addrr[0],"\n")
    print(msg_addrr[0][1],msg_addrr[0][2],"\n")
    f.write("{}{}\n".format(msg_addrr[0][1],msg_addrr[0][2]))
    num_of_sample = num_of_sample -1

    
f.close()
print("ends")
"""


style.use('fivethirtyeight')
fig = plt.figure()

ax1 = fig.add_subplot(1,1,1)

x = 1
xs = []
ys = []
def animate(i):
    global x
    global xs 
    global ys 
    msg_addrr = udpserversckt.recvfrom(32)
    
    ys.append(int("{}{}\n".format(msg_addrr[0][1],msg_addrr[0][2])))
    xs.append(x)
    
    ax1.clear()
    ax1.plot(xs,ys)
    x = x+1
    print(int("{}{}\n".format(msg_addrr[0][1],msg_addrr[0][2])))


ani = animation.FuncAnimation(fig, animate, interval=1000)
plt.show()









                   

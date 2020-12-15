#!/usr/bin/env python
# coding: utf-8

# ## IDP Python Code
# 
# ### Computer Vision

# #### Imports

# In[15]:


from PIL import Image
import matplotlib.pyplot as plt
import numpy as np
import time
import requests
import cv2


from sklearn.cluster import KMeans


# In[24]:



cap = cv2.VideoCapture(0) # video capture source camera (Here webcam of laptop) 

if cap.isOpened():
    width = int(cap.get(3))
    height =int(cap.get(4))
    print("Image Dimensions: Width is ",width," and Height is ",height)
    ret,frame = cap.read() # return a single frame in variable `frame`

    cap.release()
    

cv2.imshow('image',frame)
k = cv2.waitKey(0)
if k == 27:                 # wait for ESC key to exit
    cv2.destroyAllWindows()
elif k == ord('s'):        # wait for 's' key to save and exit
    cv2.imwrite('img.jpg', frame)
    cv2.destroyAllWindows()

plt.imshow(np.asarray(frame[:,:,[2,1,0]])) #convert BGR to RGB


# #### Locating "Red" colour & Robot

# In[18]:


"""
Defining constants
"""
#imagefile = 'img.jpg' #Image filename
imagefile = 'd:\我的文档\IDP\idp_m112_2020\IDP_PCSoftware\idpimage.png'
target_RBG = (230,50,80) #Red colour of fruit
target_RBG_robot = (255,255,50) #colour of robot
MinMatchError = 50 #Tolerance (determined thru trial & error)

"""
Other colours:
Yellow = (255,255,50)
Red box = (215,131,152)
White line/box = (253, 248, 250)

"""

"""
Creating empty containers for the selected datapoints
"""
points = []
x_plot = []
y_plot = []
robot_x_plot = []
robot_y_plot = []
robot_points = []

"""
Load image using Pillow
"""
print("Searching for {} in {}".format(target_RBG, imagefile))
img = Image.open(imagefile)
pix = img.load()
width, height = img.size[0], img.size[1]
print('Image loaded, with a height of {} and width of {}'.format(height, width))

"""
Iterate through every pixel in the image.
Save the data point to our containers if the colour is within tolerable limits.
"""

print("Scanning in progress...")
for y in range(height): #Iterating y
    for x in range(width): #Iterating x
        RBG = pix[x,y] #Current pixel colour
        
        #Absolute error between pixel and target colour
        MatchError = abs(RBG[0]-target_RBG[0])+abs(RBG[1]-target_RBG[1])+abs(RBG[2]-target_RBG[2])
        
        if MatchError < MinMatchError: #If withn tolerable limits, save pixel
            x_plot.append(x)
            y_plot.append(-y)
            points.append([x,-y])
            
for y in range(height): #Iterating y
    for x in range(width): #Iterating x
        RBG = pix[x,y] #Current pixel colour
        
        #Absolute error between pixel and target colour
        MatchError = abs(RBG[0]-target_RBG_robot[0])+abs(RBG[1]-target_RBG_robot[1])+abs(RBG[2]-target_RBG_robot[2])
        
        if MatchError < MinMatchError: #If withn tolerable limits, save pixel
            robot_x_plot.append(x)
            robot_y_plot.append(-y)
            robot_points.append([x,-y])
            
print("Scanning complete!")


# In[19]:


#Plot image
plt.figure(figsize=(8,6))
plt.imshow(np.asarray(img))


# In[20]:


#Plot saved datapoints
fig, ax = plt.subplots(figsize=(8,6))
ax.plot(x_plot,y_plot, 'o', c='red')
ax.plot(robot_x_plot,robot_y_plot, 'o', c='black')
ax.set_xlim(0, width);
ax.set_ylim(-height, 0);


# In[21]:


#Find the centre of the 6 datapoints
kmeans = KMeans(n_clusters=6)
kmeans.fit(points)
kmeans.cluster_centers_


# In[22]:


"""
Moving function, set motor start to move 
"""
def move_forward():
    
    #default speed 100
    s1 = requests.get("http://192.168.137.60/HM1?speed=100&direction=0")
    s2 = requests.get("http://192.168.137.60/HM2?speed=100&direction=0")

    return s1, s2

"""
Stop the motor
"""
def stop():
    
    e1 = requests.get("http://192.168.137.60/LM1")
    e2 = requests.get("http://192.168.137.60/LM2")
    
    return e1, e2
"""
Rotation function , set robot start to rotate about the LED at the back
"""
def rotate():
    
    s1 = requests.get("http://192.168.137.60/HM1?speed=100&direction=0")
    s2_reverse = requests.get("http://192.168.137.60/HM2?speed=100&direction=1")
    
    return s1, s2_reverse



"""
Receive a,b as the coordinates of LEDS (a at the front) and c as the fruit coordinate 
Datatype: numpy array
"""
def approach_fruit(a, b, c):
    
    #orientation and fruit distance
    ori = a - b
    fd = c - b
    
    #Angle in radian
    lo = np.sqrt(ori.dot(ori))
    lfd = np.sqrt(fd.dot(fd))
    cos = fd.dot(ori)/(lo*lfd)
    angle = np.arccos(cos)
    
    """
    Degree covertion
    
    ad = angle*180/np.pi
    """
    
    """
    Need to test to find the angular speed when two motors run in speed = 100 and different directions
    """
    angular_speed = 10 
    rotation_time = angle / angular_speed
    
    rotate()
    time.sleep(rotation_time)
    stop()
    #rotation finish
    
    
    distance = lfd - lo #in unit pixel
    forward_time = (distance - 50)/100 #default speed 100,when to stop depend on test
 
    move_forward() 
    time.sleep(forward_time) 
    stop()
    #stop moving
    
    return 


# In[23]:


#fetch data from the picture
"""fc = kmeans.cluster_centers_
for f in fc:
    approach_fruit(a, b, f)"""
    


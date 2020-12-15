#
# Concept demonstration file: not in use. Numpy approach has been integrated into image_processing. 
# 

import numpy as np
from PIL import Image
import time

def filter_image(imagefile, targetandtolleranceRGB):
    
    points = []
    x_plot = []
    y_plot = []

    with Image.open(imagefile) as img: # don't litter
        pix = img.load()
        width, height = img.size[0], img.size[1]

        print("Filtering...")
        for y in range(height): # Iterating y
            for x in range(width): # Iterating x
                RBG = pix[x,y] # Current pixel colour
                
                #Absolute error between pixel and target colour
                MatchError = abs(RBG[0]-targetandtolleranceRGB[0])+abs(RBG[1]-targetandtolleranceRGB[1])+abs(RBG[2]-targetandtolleranceRGB[2])
                
                if MatchError < targetandtolleranceRGB[3]: #If withn tolerable limits, save pixel
                    x_plot.append(x)
                    y_plot.append(-y)
                    points.append([x,-y])
                
    return x_plot, y_plot, width, height, points
    
def filter_image_np(imagefile, targetandtolleranceRGB):
    pix = np.array(Image.open(imagefile))
    height, width = pix.shape[0], pix.shape[1]
    print("Filtering...")
    RGB_ref = np.array(list(targetandtolleranceRGB[:-1]))
    tolerance = targetandtolleranceRGB[-1]
    a = np.transpose(np.abs(pix - RGB_ref), (2,0,1))
    b = (a[0] + a[1] + a[2]) < tolerance
    y_plot, x_plot = np.where(b)
    y_plot = -y_plot #make y_plot negative
    points = np.transpose(np.array([x_plot,y_plot]))                
    return x_plot.tolist(), y_plot.tolist(), width, height, points.tolist()

fruit_RGB = (241, 66, 95, 75) 
start = time.time()
print(filter_image('LatestFrame.jpg', fruit_RGB))
print("time taken for normal: %f seconds" %(time.time()-start))

start = time.time()
print(filter_image_np('LatestFrame.jpg', fruit_RGB))
print("time taken for numpy: %f seconds" %(time.time()-start))
# Runs on laptop
# Collects frame from network stream
# Detects robot & fruit
# Packages coordinates
# Called by nav policy

# imports
from PIL import Image
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import numpy as np
import os # for finding current script location, making relative paths behave.


# local imports
from image_capture import PullFrameFromStream


# settings

 # RGB
fruit_RGB = (241, 66, 95, 75) # sampled lego red. last term is match tollerance.
main_beacon_RGB = (43, 175, 215, 100) # Sampled from beacon. last term is match tollerance.
orientation_beacon_RGB = (136, 251, 255, 100) # Sampled from beacon with green cover. 
#orientation_beacon_RGB = (247, 247, 253, 5) # Sampled from white beacon. last term is match tollerance.

 # Files
testfilename =  os.path.dirname(__file__) + "/LatestFrame.jpg"
LatestFrame = os.path.dirname(__file__) + "/LatestFrame.jpg"

# Functions
def load_image_from_file(imagefile):
    # Returns PIL object...
    img = Image.open(imagefile) # "with . open" did not work here 
    return img

def display_image(img, shownow=True):
        #img.show() # works but not in desired frame
        plt.figure()
        plt.imshow(np.asarray(img))
        if shownow:
            plt.show()

def filter_image_np_plot(img, targetandtolleranceRGB):
    # print("Filtering numpy plot")
    pix = np.array(img)
    RGB_ref = np.array(list(targetandtolleranceRGB[:-1]))
    a = np.transpose(np.abs(pix - RGB_ref), (2,0,1))
    b = (a[0] + a[1] + a[2]) < targetandtolleranceRGB[-1]
    y_plot, x_plot = np.where(b)
    points = np.transpose(np.array([x_plot,-y_plot]))                
    return x_plot.tolist(), y_plot.tolist(), pix.shape[1], pix.shape[0], points.tolist()

def filter_image_np(img, targetandtolleranceRGB):
    # print("Filtering numpy style")
    pix = np.array(img)
    RGB_ref = np.array(list(targetandtolleranceRGB[:-1]))
    a = np.transpose(np.abs(pix - RGB_ref), (2,0,1))
    b = (a[0] + a[1] + a[2]) < targetandtolleranceRGB[-1]
    y_plot, x_plot = np.where(b)
    points = np.transpose(np.array([x_plot,-y_plot]))                
    return points.tolist()

def plot_filtrate(x_points, y_points, width, height, colour, shownow=True):
    fig, ax = plt.subplots()
    ax.plot(x_points,y_points, 'o', c=colour)
    ax.set_xlim(0, width)
    ax.set_ylim(-height, 0)
    if shownow:
        plt.show()

def detect_cluster_centres(points, numclusters):
    kmeans = KMeans(n_clusters=numclusters)
    if points:
        kmeans.fit(points)
        return kmeans.cluster_centers_
    else:
        return None

def GetFruitLocations():
    #CaptureFrame()
    return detect_cluster_centres(filter_image_np(PullFrameFromStream(), fruit_RGB), 6)

def GetRobotPosition():
    #CaptureFrame()
    img = PullFrameFromStream()
    if img: # eg if http response was 500, PFFS returns None. Trying to process this as an image ... gg. 
        core_position = detect_cluster_centres(filter_image_np(img, main_beacon_RGB), 1)
        if core_position is None:
            print("GRP core match failed")
            return None
        orient_position = detect_cluster_centres(filter_image_np(img, orientation_beacon_RGB), 1)
        if orient_position is None:
            print("GRP orient match failed")
            return None
        print("Returning GRP", core_position[0], orient_position[0])
        return (core_position[0], orient_position[0])
    else:
        print("GRP could not aquire an image")
        return None

def test_filter_and_display(imagefile): # fixed
    img = load_image_from_file(imagefile)
    display_image(img, False)
    filtered_image = filter_image_np_plot(img, fruit_RGB)
    plot_filtrate(filtered_image[0], filtered_image[1], filtered_image[2], filtered_image[3], 'red', False)
    print("Fruit detected at:")
    print(detect_cluster_centres(filtered_image[4], 6))

    filtered_image = filter_image_np_plot(img, main_beacon_RGB)
    plot_filtrate(filtered_image[0], filtered_image[1], filtered_image[2], filtered_image[3], 'blue', False)
    print("main beacon detected at:")
    print(detect_cluster_centres(filtered_image[4], 1))

    filtered_image = filter_image_np_plot(img, orientation_beacon_RGB)
    plot_filtrate(filtered_image[0], filtered_image[1], filtered_image[2], filtered_image[3], 'yellow')
    print("orientation beacon detected at:")
    print(detect_cluster_centres(filtered_image[4], 1))



# tests:
#display_image(load_image_from_file(testfilename)) # works
#display_image(PullFrameFromStream()) # works
#test_filter_and_display(testfilename) # works

#for FruitLocation in GetFruitLocations():
#    print(FruitLocation)

"""
GRP = GetRobotPosition()
if GRP:
    print(GRP)
"""
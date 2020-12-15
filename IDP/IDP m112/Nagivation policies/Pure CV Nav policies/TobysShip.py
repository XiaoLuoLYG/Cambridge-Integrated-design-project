# standard imports
from math import hypot # what's it really called?
from time import sleep

# local imports
from IDPImageProcessing import GetFruitLocations, GetCameraFruitColour, GetRobotPosition # GRP should return [(x,y), eulerangle] or False if failure
from AlphaPID import PID
from IDPComms import remotecontrol # comms function dedicated to using arduino as simple slave

# settings
ExpectedRipeFruitCount = 4 # int number of red blocks to be recovered. Will stop looking after this many are relocated. 
robotlength = 50 # number of pixels from front blade leading edge to core beacon
slow_approach_distance = 50 # number of pixels from target to drive slowly
slow_approach_speed = 30 # % of full speed to fine tune approach
fruit_scan_distance = 20 # distance to stop away from fruit for camera scan
scooptime = 2 # seconds to spin to capture fruit
reversetime = 3 # second to reverse if fruit is unripe or unloading

# hard coded waypoints, tuples
waypoint_start = (0,0)
waypoint_drop = (0,0)
waypoint_1 = (0,0) # roughly horizontal line across middle from left most (orchard) to right most.
waypoint_2 = (0,0)
waypoint_3 = (0,0)
waypoint_4 = (0,0)
waypoint_5 = (0,0)
waypoint_6 = (0,0)


# setup
fruitlist = GetFruitLocations() # list of tuples

#   add status tag - mostly for debuging
for fruit in fruitlist: # each fruit is a tuple of a pair of coords
    fruit.append("unchecked") # to be overwritten when examined or recovered.

# sub functions
def GetDistance(point1, point2): # return hypot of cart deltas
    return hypot(point1[0]-point2[0], point1[1]-point2[1])

def scoop(): # spin via one wheel forward to collect fruit while turining around
    remotecontrol(100, 0) # start spin
    sleep(scooptime)
    remotecontrol(0,0) # stop
    pass

def GetAngle(point1, point2):
    pass

def Drive(forwardspeedpercent, rotation): # translates forward speed and rotation (-1 to 1) into left and right tank steering. Could be very simple, but could get tuned clever. 
    if rotation == 0:
        remotecontrol(forwardspeedpercent, forwardspeedpercent) # will it need to send percent or 255? adjust here.
    elif abs(rotation) > 0.5: # if rotation is substantial, supress all non-rotation motion
        remotecontrol(100*rotation, -100*rotation)
    elif abs(rotation) > 0.25: # if rotation moderate, halve forward speed
        remotecontrol(100*rotation + 0.5*forwardspeedpercent, -100*rotation + 0.5*forwardspeedpercent)
    else: # if rotation small, make adjustment to forward (or reverse?) motion
        pass


def DriveToWaypoint(waypoint, finaldistance=0, stop_at_target=True):

    while True: # doesn't feel good. Needs some limit. 
        GRP = GetRobotPosition() # request from camera et al
        if GRP: # GRP might have not detected both beacons, in which case it would return False

            DistanceToWaypoint = GetDistance(GRP[0], waypoint)
            if DistanceToWaypoint < finaldistance:
                if stop_at_target:
                    remotecontrol(0,0)
                return True # sucessfully arrived at waypoint - end DTW function
            elif DistanceToWaypoint < (finaldistance + slowdistance):
                # approach slowly
                pass
            else:
                #approach fast
                pass


def harvest():
    DriveToWaypoint(waypoint_6, robotlength) # get front blade to line of WP6, but body can lag behind
    DriveToWaypoint(waypoint_5, 0, True) # to ensure good course to point 4
    DriveToWaypoint(waypoint_4, 0, False) # keep driving through the tunnel

    DriveToWaypoint(waypoint_1) # middle of the orchard

    # drive from WP6 to orchard
    for fruit in fruitlist:
        DriveToWaypoint(fruit[0], fruit_scan_distance + robotlength) # approach fruit
        if GetCameraFruitColour == "Ripe": # assess fruit
            scoop()
            DriveToWaypoint(waypoint_1, robotlength)
            DriveToWaypoint(waypoint_2)
            DriveToWaypoint(waypoint_3, stop_at_target=False)
            DriveToWaypoint(waypoint_5)
            DriveToWaypoint(waypoint_drop, robotlength)
            Drive(-100, 0) # back off and leave ripe fruit at WPdrop 
            sleep(reversetime)
            remotecontrol(0,0)
            DriveToWaypoint(waypoint_6, robotlength)
            # if expecting more fruit and not all fruit assessed:
                # drive back to orchard:
            DriveToWaypoint(waypoint_5)
            DriveToWaypoint(waypoint_4, stop_at_target=False)
            DriveToWaypoint(waypoint_1)

        pass

def testship(): # run up and down the right hand side of the course
    while True:
        DriveToWaypoint(waypoint_start)
        DriveToWaypoint(waypoint_drop)
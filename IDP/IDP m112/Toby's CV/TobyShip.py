# standard imports
from time import sleep, time
from math import pi, copysign

# installed imports
#none

# local imports
from image_processing import GetFruitLocations, GetRobotPosition#, GetCameraFruitColour # GRP should return [(x,y)[core], (x,y)[beacon]] or False if failure on either
from image_capture import crop_area
#from AlphaPID import PID
from IDPComms import SendSpeedsToRobot # comms function dedicated to using arduino as simple slave. Send(leftspeed, rightspeed)
from utils import GetDistance, GetHeadingError

# settings
robotlength = 50 # number of pixels from front blade leading edge to core beacon # 50 was a guess but measurements are close!
fruit_scan_distance = 10 # px ~30mm # distance to stop away from fruit for camera scan
slow_approach_distance = 70 # px ~200mm # number of pixels from target to drive slowly
slow_approach_speed = 0.3 # fraction of full speed to fine tune approach
full_rotation_speed = 100 # out of 255

scooptime = 4.15 # seconds to spin to capture fruit, aprox 180deg turn
reversetime = 2 # second to reverse if fruit is unripe or unloading

rotation_gain_largeangle = 2 # make it too big and drive @ moderate angles may clip
rotation_gain_smallangle = 1 # fine tuning when going practically straight

standard_navigation_tollerance = 10 # px # Measuring from pictures, bot platform length is about 61px = 175mm. So a target tollerance radius of 30mm is 10px.

# hard coded waypoints, tuples. updated for camera 2.
waypoint_start = (774-crop_area[0],117-crop_area[1])
waypoint_drop = (700-crop_area[0],655-crop_area[1])
waypoint_1 = (0-crop_area[0],0-crop_area[1]) # roughly horizontal line across middle from left most (orchard) to right most.
waypoint_2 = (292-crop_area[0],357-crop_area[1]) # only points 2 and 4 are used for simplest harvesting policy. 
waypoint_3 = (0-crop_area[0],0-crop_area[1])
waypoint_4 = (760-crop_area[0],357-crop_area[1])
waypoint_5 = (0-crop_area[0],0-crop_area[1])
waypoint_6 = (0-crop_area[0],0-crop_area[1])

print(waypoint_start)

# Functions!
def scoop(): # spin via one wheel forward to collect fruit while turining around
    print("scooping")
    SendSpeedsToRobot(0, 255) # start spin clockwise
    sleep(scooptime)
    SendSpeedsToRobot(0,0) # stop
    pass

def back_off(): # simple reverse function
    print("reversing")
    SendSpeedsToRobot(-255,-255)
    sleep(reversetime)
    SendSpeedsToRobot(0,0)

def Drive(ForwardSpeedFraction, rotation): 
    # translates forward speed and rotation (-1 to 1) into left and right tank steering. abs(rotation) = 1 ==> facing completely wrong way
    # Positive rotation turns right. 
    # output is integers +-255
    forwardspeed = int(255*ForwardSpeedFraction) # int(), unlike round(), always rounds towards zero. Very useful in this case.

    if rotation == 0: # Going straight (unlikely)
        SendSpeedsToRobot(forwardspeed, forwardspeed)

    elif abs(rotation) > 0.25: # if rotation is substantial, supress all non-rotation motion and rotate at full speed
        SendSpeedsToRobot(int(copysign(full_rotation_speed, rotation)), int(copysign(full_rotation_speed, -rotation))) # int only required to ensure int format, as copysign returns a float 

    elif abs(rotation) > 0.15: # if rotation moderate, halve forward speed and rotate proportionally
        SendSpeedsToRobot(int(255*rotation_gain_largeangle*rotation + 0.5*forwardspeed), int(-full_rotation_speed*rotation_gain_largeangle*rotation + 0.5*forwardspeed))

    else: # if rotation small, make adjustment to linear motion
        if rotation > 0: # if need to turn right, ie slow right
            SendSpeedsToRobot(forwardspeed, int(forwardspeed - 255*rotation_gain_smallangle*rotation))
        else: # if rotation < 0, need to turn left, ie slow left
            SendSpeedsToRobot(int(forwardspeed + 255*rotation_gain_smallangle*rotation), forwardspeed)

def DrivePID(ForwardSpeedFraction, rotation):
    #Advanced version of Drive(), coming soon
    pass

def DriveToWaypoint(waypoint, finaldistance=0, stop_at_target=True, loop=True):
    # Moves the core beacon onto the waypoint coordinate. 
    while True:
        print("Cycle starting...")
        CycleStartTime = time()  
        GRP = GetRobotPosition() # request from camera et al
        if GRP: # GRP might have not detected both beacons, in which case it would return False
            DistanceToWaypoint = GetDistance(GRP[0], waypoint)
            heading_error = GetHeadingError(waypoint,GRP[0],GRP[1])/pi # express in -1 to 1 form
            print("Driving: DistanceToWaypoint = ", DistanceToWaypoint, "heading_error = ", heading_error)
            if DistanceToWaypoint < finaldistance:
                if stop_at_target:
                    SendSpeedsToRobot(0,0)
                print("waypoint reached")
                return True # sucessfully arrived at waypoint - end DTW function
            elif DistanceToWaypoint < (finaldistance + slow_approach_distance):
                Drive(slow_approach_speed, heading_error/2) # various reasons to dampen steering when close
            else:
                Drive(1, heading_error) # approach at full speed
            
            if not loop: # looping edition for hardcoded points, disable to use in an external loop that updates the target before it is reached.
                return True # sucessfully provided one round of iteration to motion
        
        else: # if beacons were not detected
            if not loop:
                return False # failed to update motion with one attempt
            # else no problem, try again 
        print("Cycle time:", round(time()-CycleStartTime, 3))

def SimpleHarvest():
    # Only two floating waypoints, collect all fruit and ignore colour, banded proportional control.
    
    fruitlist = GetFruitLocations() # list of tuples

    DriveToWaypoint(waypoint_4, standard_navigation_tollerance)

    for fruit in fruitlist:
        print("Working on a new fruit")
        DriveToWaypoint(waypoint_2, standard_navigation_tollerance)
        DriveToWaypoint(fruit, fruit_scan_distance + robotlength) # additional tollerance probably not required as robotlength is aready large.
        scoop()
        DriveToWaypoint(waypoint_2, standard_navigation_tollerance)
        DriveToWaypoint(waypoint_4, standard_navigation_tollerance)
        DriveToWaypoint(waypoint_drop, standard_navigation_tollerance)
        reverse()
        DriveToWaypoint(waypoint_4, standard_navigation_tollerance)
    
    DriveToWaypoint(waypoint_start, standard_navigation_tollerance)

def harvest():
    # Over complicating it. Why?
    fruitlist = GetFruitLocations() # list of tuples

    DriveToWaypoint(waypoint_6, robotlength) # get front blade to line of WP6, but body can lag behind
    DriveToWaypoint(waypoint_5, 0, True) # to ensure good course to point 4
    DriveToWaypoint(waypoint_4, 0, False) # keep driving through the tunnel

    DriveToWaypoint(waypoint_1) # middle of the orchard

    # drive from WP6 to orchard
    for fruit in fruitlist:
        DriveToWaypoint(fruit, fruit_scan_distance + robotlength) # approach fruit
        if GetCameraFruitColour == "Ripe": # assess fruit
            scoop()
            DriveToWaypoint(waypoint_1, robotlength)
            DriveToWaypoint(waypoint_2)
            DriveToWaypoint(waypoint_3, stop_at_target=False)
            DriveToWaypoint(waypoint_5)
            DriveToWaypoint(waypoint_drop, robotlength)
            back_off()
            DriveToWaypoint(waypoint_6, robotlength)
            DriveToWaypoint(waypoint_5)
            DriveToWaypoint(waypoint_4, stop_at_target=False)
            DriveToWaypoint(waypoint_1)


def testship(): # run up and down the right hand side of the course
    try:    
        while True:
            DriveToWaypoint(waypoint_drop, 50)
            DriveToWaypoint(waypoint_start, 50)
    except KeyboardInterrupt:
        sleep(2) # necessary to ensure stop command doesn't get garbled
        SendSpeedsToRobot(0,0)
        print("test aborted")

def testdriving():
    try:    
        while True:
            back_off()
            sleep(0.1) # 1.1 seconds is stable - 1 second or less and the arduino separation between messages gets confused.
            scoop()
            sleep(0.1)

    except KeyboardInterrupt:
        sleep(2) # necessary to ensure stop command doesn't get garbled
        SendSpeedsToRobot(0,0)
        print("test aborted")

# testdriving()
testship()
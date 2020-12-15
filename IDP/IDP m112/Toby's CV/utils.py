# provides useful little functions that are not dedicated to a specific nav policy

from math import atan2, pi, hypot # takes two values rather than ratio (so gets sign right), returns -pi to +pi.

def GetHeadingError(target, core, orientation): 
    # Takes three coord pairs.
    # Are all points on a straight line? The coordinate system kinda doesn't matter, but assuming pillow top left origin. 
    # some say you could just do atan2(sin(x-y), cos(x-y))...

    # Assumes orient is behind core when moving forward.
    current_heading = atan2(core[1]-orientation[1], core[0]-orientation[0])

    target_heading = atan2(target[1]-core[1], target[0]-core[0])

    heading_error = current_heading - target_heading

    if heading_error > pi: # if more than half way round, spin the other way...
        heading_error -= 2*pi
    elif heading_error < -pi:
        heading_error += 2*pi

    return heading_error # radians. If positive, turn right. 

def GetDistance(point1, point2): # Takes two coord pairs. return positive hypot of cart deltas
    return hypot(point1[0]-point2[0], point1[1]-point2[1])

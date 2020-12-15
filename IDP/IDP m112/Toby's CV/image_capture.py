# Runs on laptop
# Opens network stream and saves a frame locally. 

# It might be really easy...

import requests
from PIL import Image
from io import BytesIO 
import os # for finding current script location, making relative paths behave.

# settings
#url = 'http://idpcam1.eng.cam.ac.uk:8080/stream/video.mjpeg' # Internal cam 1
#url = 'http://idpcam2.eng.cam.ac.uk:8080/stream/video.mjpeg' # Internal cam 2
#url = 'http://localhost:8081/stream/video.mjpeg' #  Cam 1 When tunneling in...
url = 'http://localhost:8082/stream/video.mjpeg' #  Cam 2 When tunneling in...

crop_area = (140, 20, 850, 730) # (left, upper, right, lower) # https://pillow.readthedocs.io/en/stable/reference/Image.html#PIL.Image.Image.crop

# Manually get a frame by right clicking on the stream, Copy Image, open Paint and paste. Save suitably. 

# Or try and automate that...
def SaveFrameFromStream():
    print("Requesting a frame from the stream...")
    resp = requests.get(url, stream=True)
    print("got response", resp.status_code)
    if (resp.status_code == 200): # The mess of turning a mjpeg into frames without cv2. 
        jpeg_chunk = bytearray()
        for chunk in resp.iter_content(chunk_size=1024): 
            jpeg_chunk += chunk 
            a = jpeg_chunk.find(b'\xff\xd8')
            b = jpeg_chunk.find(b'\xff\xd9')
            if a != -1 and b != -1: 
                jpg = jpeg_chunk[a:b+2]
                break

        img = Image.open(BytesIO(jpg))
        print("img is open")
        img_crop = img.crop(crop_area)
        print("image is cropped")
        img_crop.save(os.path.dirname(__file__) + '/LatestFrame.jpg', 'jpeg') # saves in same directory as this script
        print("saved")
    else:
        print(resp)

def PullFrameFromStream():
    print("Requesting a frame from the stream...")
    resp = requests.get(url, stream=True)
    print("got response", resp.status_code)
    if (resp.status_code == 200): # The mess of turning a mjpeg into frames without cv2:
        jpeg_chunk = bytearray()
        for chunk in resp.iter_content(chunk_size=1024): 
            jpeg_chunk += chunk 
            a = jpeg_chunk.find(b'\xff\xd8')
            b = jpeg_chunk.find(b'\xff\xd9')
            if a != -1 and b != -1: 
                print("returning image data")
                jpg = jpeg_chunk[a:b+2]
                img = Image.open(BytesIO(jpg))
                img_crop = img.crop(crop_area)
                return img_crop
    else: # Other http responses will return None. 500 is common.
        print(resp)

# tests:
# SaveFrameFromStream()
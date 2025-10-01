import cv2
import ctypes
import numpy as np
import math

FILENAME = "mbfa.txt" #input("Enter filename: ")


def generateLED(remainingMines):
    led = np.zeros((35,60,3), dtype=np.uint8);
    if remainingMines > 1000:
        remainingMines = 999
    digits = str(remainingMines)
    while len(digits) < 3:
        digits = '0' + digits;
    for i, num in enumerate(digits):
        texture = cv2.imread("images/led" + num + ".png", cv2.IMREAD_COLOR)
        x1 = i * 20
        x2 = (i+1)* 20
        led[0:35,x1:x2] = texture
        pass
    cv2.imwrite('led.png', led)

def getRect(x, y):
    return x*48, y*48, (x+1)*48, (y+1)*48

def blend(im1, im2, alpha):
    im3 = im1 * alpha + im2  * (1-alpha)
    im3 = np.rint(im3).astype(np.uint8)
    return im3



#load dll
filename = FILENAME.encode();
mydll = ctypes.CDLL("main.dll", winmode = 0)
mydll.getWinningProbability.restype = ctypes.c_double
mydll.run(filename);

#generate LED
remainingMines = mydll.getRemainingMines();
generateLED(remainingMines);

#load textures
textures = []
for i in range(12):
    texture = cv2.imread("images/" +str(i)  + ".bmp", cv2.IMREAD_COLOR)
    texture = cv2.resize(texture, (48, 48));
    textures.append(texture)

#generate highlighted tile
highlight = np.zeros((48,48,3),dtype=np.uint8);
highlight[:,:] = (127,255,127)
highlighted = blend(highlight, textures[9], 0.5)


#get width / height
f = open(FILENAME, "r");
line = f.readline().split('x');
width, height, mineCount = [int(i) for i in line]
f.close()

#create blank image
w, h = width * 48, height * 48;
img = np.zeros((h,w,3), dtype=np.uint8);

# paste textures
for y in range(height):
    for x in range(width):
        i = y * width + x
        c = mydll.getContent(i);
 
        x1, y1, x2, y2 = getRect(x,y)
        img[y1:y2,x1:x2] = textures[c]


#get safest cell
safest = -1;
wp_max = 0;
for y in range(height):
    for x in range(width):
        i = y * width + x
        wp = mydll.getWinningProbability(i);
        if (wp > wp_max):
            wp_max = wp
            safest = (x,y)

x1, y1, x2, y2 = getRect(*safest)
img[y1:y2,x1:x2] = highlighted

# paste labels
font = cv2.FONT_HERSHEY_SIMPLEX;
font_scale = 0.6
color = (0,0,0);
thickness = 2

for y in range(height):
    for x in range(width):
        i = y * width + x
        wp = mydll.getWinningProbability(i);
    
        if wp == -1:
            continue

        wp100 = round(wp*100, 1)
        if wp100 < 10:
            org = (x*48+12, y*48 + 32)
        else:
            org = (x*48+2, y*48 + 32)
        cv2.putText(img, str(wp100) , org, font, font_scale, color, thickness)



cv2.imwrite('output_image.png', img)
#cv2.imshow('a', img);
#cv2.waitKey(0);

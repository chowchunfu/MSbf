import cv2
import ctypes
import numpy as np
import math

FILENAME = "bfboard.txt" #input("Enter filename: ")

def CharToEnum(char):
    if char == 'H':
        return 9
    if char == 'F':
        return 10
    if char == 'M':
        return 11
    return int(char)

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
    cv2.imwrite('remainingmines.png', led)

def getRect(x, y):
    return x*48, y*48, (x+1)*48, (y+1)*48

def blend(im1, im2, alpha):
    im3 = im1 * alpha + im2  * (1-alpha)
    im3 = np.rint(im3).astype(np.uint8)
    return im3

filename = FILENAME.encode();
mydll = ctypes.CDLL("main.dll", winmode = 0)
mydll.no_bfa(filename);


#load board
filename = "bfboard.txt"
f = open(filename, "r")
line = f.readline()
width, height, mineCount = [int(i) for i in line.split('x')]
board = np.zeros((height, width),dtype=np.uint8)
for y in range(height):
    line = f.readline()
    for x in range(width):
        c = CharToEnum(line[x])
        board[y][x] = c

        mineCount-= c == 10

generateLED(mineCount)

#load textures
textures = []
for i in range(12):
    texture = cv2.imread("images/" +str(i)  + ".bmp", cv2.IMREAD_COLOR)
    texture = cv2.resize(texture, (48, 48));
    textures.append(texture)

#create blank image
w, h = width * 48, height * 48;
img = np.zeros((h,w,3), dtype=np.uint8);

# paste textures
for y in range(height):
    for x in range(width):
        c = board[y][x]
 
        x1, y1, x2, y2 = getRect(x,y)
        img[y1:y2,x1:x2] = textures[c]

cv2.imwrite('board.png', img)

f = open("prepare.txt", "r")
for line in f.readlines():
    print(line,end="")
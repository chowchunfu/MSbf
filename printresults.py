import cv2
import numpy as np
import math



def CharToEnum(char):
    if char == 'H':
        return 9
    if char == 'F':
        return 10
    if char == 'M':
        return 11
    return int(char)



#load board

f = open("bfboard.txt", "r")
line = f.readline()
width, height, mineCount = [int(i) for i in line.split('x')]
board = np.zeros((height, width),dtype=np.uint8)
for y in range(height):
    line = f.readline()
    for x in range(width):
        board[y][x] = CharToEnum(line[x])


def getRect(x, y):
    return x*48, y*48, (x+1)*48, (y+1)*48

def blend(im1, im2, alpha):
    im3 = im1 * alpha + im2  * (1-alpha)
    im3 = np.rint(im3).astype(np.uint8)
    return im3


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


#create blank image
w, h = width * 48, height * 48;
img = np.zeros((h,w,3), dtype=np.uint8);

# paste textures
for y in range(height):
    for x in range(width):
        c = board[y][x]
 
        x1, y1, x2, y2 = getRect(x,y)
        img[y1:y2,x1:x2] = textures[c]

# load result
W = np.zeros((height,width),dtype=np.int32)
Wp = np.zeros((height,width),dtype=np.float64)

filename = "bfresult.csv"
f = open(filename, "r")
bfresultlines = f.readlines()
bfresultlines = [i.rstrip("\n") for i in bfresultlines]

# Parse the CSV data
for line in bfresultlines[1:]:  # Skip the header
    if not line:
        break
    line = line.split(",")

    x,y, wins, winprob = line
    
    x = int(x.lstrip('"('))
    y = int(y.rstrip(')"'))
    
    wins = int(wins)
    winprob = float(winprob.rstrip('%')) / 100
    W[y][x] = wins
    Wp[y][x] = winprob
    
index = np.argmax(W)
x = index % width
y = index // width

x1, y1, x2, y2 = getRect(x,y)
img[y1:y2,x1:x2] = highlighted

# paste labels
font = cv2.FONT_HERSHEY_SIMPLEX;
font_scale = 0.6
color = (0,0,0);
thickness = 2

for y in range(height):
    for x in range(width):
        wp = Wp[y][x]
        if wp == 0:
            continue
        

        wp100 = round(wp*100, 1)
        if wp100 == 100:
            wp100 = int(wp100)

        if wp100 < 10:
            org = (x*48+12, y*48 + 32)
        else:
            org = (x*48+2, y*48 + 32)
        cv2.putText(img, str(wp100) , org, font, font_scale, color, thickness)



cv2.imwrite('winprobabilities.png', img)
#cv2.imshow('wp', img);
#cv2.waitKey(0);

#print info
def format_time(seconds_elapsed):
    seconds_elapsed = int(seconds_elapsed)
    # Calculate days, hours, minutes, and seconds
    days = seconds_elapsed // 86400  # 86400 seconds in a day
    hours = (seconds_elapsed % 86400) // 3600
    minutes = (seconds_elapsed % 3600) // 60
    seconds = seconds_elapsed % 60
    
    # Build the output list
    time_parts = []
    if days > 0:
        time_parts.append(f"{days}d")
    if hours > 0 or days > 0:  # Show hours if there are days
        time_parts.append(f"{hours}h")
    if minutes > 0 or hours > 0 or days > 0:  # Show minutes if there are hours or days
        time_parts.append(f"{minutes}m")
    
    time_parts.append(f"{seconds}s")  # Always show seconds

    # Join the parts into a single string
    formatted_time = " ".join(time_parts)
    print("("+formatted_time+")")

filename = "bfinfo.txt"
f = open(filename, "r")
for line in f.readlines():
    print(line,end="")
    if "Elapsed" in line:
        elapsed = line.lstrip("Elapsed: ").rstrip(" s\n")
        elapsed = int(float(elapsed))
        format_time(elapsed)


print()

#print result
def print_best_tiles(lines):
    # Split the input data into lines
    lines = [i.rstrip('\n') for i in lines]
    
    # Parse the CSV data
    tiles = []
    for line in lines[1:]:  # Skip the header
        if not line:
            break
        x,y, wins, winprob = line.split(',')
        x = x.lstrip('"')
        y = y.rstrip('"')
        wins = int(wins)
        winprob = float(winprob.rstrip('%')) / 100
        tiles.append((x,y, wins, winprob))
    
    # Sort the tiles based on the number of wins (descending)
    tiles.sort(key=lambda x: x[2], reverse=True)
    
    # Initialize ranking system
    rank = 0
    last_wins = None
    output = []
    
    print("Best cells / Wins:")
    for i, (x,y, wins, winprob) in enumerate(tiles):
        if last_wins is None or wins != last_wins:
            rank += 1
            if rank > 5:
                break

            rank_str = f"{rank}th" if rank > 3 else f"{['1st', '2nd', '3rd'][rank-1]}"

            
            output.append(f"{rank_str}: {x},{y} {wins} ({winprob * 100:.2f}%)")
            last_wins = wins
            
            
            
        else:
            
            output.append(f"  : {x},{y} {wins} ({winprob * 100:.2f}%)")
            


    # Print the output
    for line in output:
        print(line)

print_best_tiles(bfresultlines)
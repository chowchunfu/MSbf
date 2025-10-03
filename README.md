## How to do a brute force analysis

Download and unpack the zip archive from this page

<img width="465" height="380" alt="image" src="https://github.com/user-attachments/assets/7bef89a1-2ef4-4a94-91d0-13191681ef7b" />



Open command prompt

<img width="592" height="122" alt="image" src="https://github.com/user-attachments/assets/bc9042f3-a3e4-4017-b6cc-22d5d4131b91" />


Type the following command to locate this folder: 

cd C:\"some path"\MSbf

<img width="575" height="25" alt="image" src="https://github.com/user-attachments/assets/bbdf1268-48f5-49e0-8088-8067a883a539" />

## 3 methods to execute the program

Method 1: Download GCC compiler following the tutorial
https://code.visualstudio.com/docs/cpp/config-mingw

Type the following command to compile the source codes:

g++ main.cpp -o main.exe

<img width="574" height="24" alt="image" src="https://github.com/user-attachments/assets/b6d94f29-377a-469f-a4eb-4ae75346075b" />

Type the following command to run the program:

main.exe

<img width="576" height="23" alt="image" src="https://github.com/user-attachments/assets/36be9634-188e-435e-b3d0-cee487555cd2" />

The program will recursively place the numbers until all combinations have been tested.

<img width="442" height="151" alt="image" src="https://github.com/user-attachments/assets/a415d9a7-0a20-4846-a4cc-0475f4e9816d" />


##
Method 2: Use the existing main.exe to run the program

<img width="576" height="23" alt="image" src="https://github.com/user-attachments/assets/36be9634-188e-435e-b3d0-cee487555cd2" />

If the file is blocked by the antivirus softwares, please use the first method instead.
##
Method 3: (require Python)

Type the following command to run the python script:

py main.py

<img width="604" height="26" alt="image" src="https://github.com/user-attachments/assets/b071eafa-311e-4b8e-aa5b-98a5d9e6376b" />



## View the results

After the program is finished, two text files "bfinfo.txt" and "bfresult.csv" are generated. Click them to view the result.

Generate an image of win probability by typing: (Python required)

py printresults.py

<img width="288" height="288" alt="winprobabilities" src="https://github.com/user-attachments/assets/0bf7e135-5368-4bb4-81e0-e8dd77364049" />



## Editing the board

Use the board editor from https://davidnhill.github.io/JSMinesweeper/index.html

<img width="1015" height="618" alt="image" src="https://github.com/user-attachments/assets/28b74301-ccbf-42c9-8718-a62a60da3499" />

Click "Save position and replace the existing "bfboard.txt".

## Resume an unfinished brute force

The brute force program will automatically make a savestate every 100,000,000 iterations (about 2 minutes).

When starting the program, it will first detect if there is an existing savestate. If success, it will continue to compute the remaining combinations.




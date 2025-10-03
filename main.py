import ctypes


FILENAME = "bfboard.txt" #input("Enter filename: ")

#load dll
filename = FILENAME.encode();
mydll = ctypes.CDLL("main.dll", winmode = 0)
mydll.run(filename);

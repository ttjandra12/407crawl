#This code converts the raw data saved on the .CSV file from the inspection robot to user-friendly relative height measurements.
import pandas as pd
import numpy as np
import math
import tkinter as tk
from tkinter import filedialog

#input initial scissor jack dimensions
#X_m = input("What is the initial width of the scissor jack (in inches)? ")
#Y_m = input("What is the vertical height of the scissor jack? (in inches)")
X_m = 0.5
Y_m = 0.5
X_i = 0.5*(float(X_m) - 1)
Y_i = 0.5*(float(Y_m) - 1)

#reading raw data from SD card
df = pd.read_csv("TEST2.CSV", sep=",", header=None)
df.columns = ["Pitch", "Roll", "ToF", "Angular Disp."] #adding column headers
df["ToF"] = pd.to_numeric(df["ToF"], errors="coerce") #converting string to float
df["Delta X"] = df["Angular Disp."] * (0.0625/360) #change in horizontal width
df["Delta Y"] = "" #initializing column
for i in range(len(df.index)): #iterating through rows to calculate delta Y
    x_val = df.loc[i, "Delta X"]
    df.loc[i, "Delta Y"] = 2*(math.sqrt((6.484**2)-(X_i+0.5*x_val))**2)-Y_i
df["Rel. Height"] = df["Delta Y"] + df["ToF"] #calculating relative height
print(df["Rel. Height"].max())
df["Height Diff."] = df["Rel. Height"].max() - df["Rel. Height"] #shim height needed to level each pier
df["Height Diff."] = pd.to_numeric(df["Height Diff."], errors="coerce") #converting string to float

def find_nearest(value): #finding shim thickness to offset leveling
    if (value == 0 or np.isnan(value) == True):
        return 0
    else:
        shim_thickness = [0.125, 0.25, 0.5, 0.75, 1]  # common shim thicknesses
        idx = (np.abs(shim_thickness - value)).argmin()
        return shim_thickness[idx]

for i in range(len(df.index)): #iterating through rows to find shim thickness required for each pier
    value = df.loc[i, "Height Diff."]
    df.loc[i, "Shim Thick."] = find_nearest(value)
print(df)

#export file window pop-up
root = tk.Tk()

canvas1 = tk.Canvas(root, width = 300, height = 300, bg = 'lightsteelblue2', relief = 'raised')
canvas1.pack()

def exportCSV():
    global df

    export_file_path = filedialog.asksaveasfilename(defaultextension='.csv')
    df.to_csv(export_file_path, index=None, header=True)


saveAsButton_CSV = tk.Button(text='Export CSV', command=exportCSV, bg='green', fg='white',
                             font=('helvetica', 12, 'bold'))
canvas1.create_window(150, 150, window=saveAsButton_CSV)

root.mainloop()


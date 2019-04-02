from tkinter import *
from tkinter import filedialog
import pandas as pd
import numpy as np
import math

X_m = 11
Y_m = 9.25
X_i = 0.5*(float(X_m) - 1)
Y_i = 0.5*(float(Y_m) - 1)

class App:

    def __init__(self, master):
        frame = Frame(master)
        frame.pack()
        Label(frame, text="407 CRAWL USER GUI", command=None, bg=None, fg='black',
              font=('helvetica', 20, 'bold')).grid(row=0, columnspan=2)
        button1 = Button(frame, text='Post-Process Data', command=self.post_proc_data, bg='green', fg='white', font=('helvetica', 12, 'bold')).grid(row=2, columnspan=2)
        Label(frame, text="Written for 407Crawl's Inspection Robot").grid(row=6, columnspan=2)

    def post_proc_data(self):

        import_file_path = filedialog.askopenfilename(defaultextension='.csv')
        # reading raw data from SD card
        df = pd.read_csv(import_file_path, sep=",", header=None)
        df.columns = ["Pitch", "Roll", "ToF", "Angular Disp."]  # adding column headers
        df["ToF"] = pd.to_numeric(df["ToF"], errors="coerce")  # converting string to float
        df["Delta X"] = df["Angular Disp."] * (0.0625 / 360)  # change in horizontal width
        df["Delta Y"] = ""  # initializing column
        for i in range(len(df.index)):  # iterating through rows to calculate delta Y
            x_val = df.loc[i, "Delta X"]
            df.loc[i, "Delta Y"] = 2 * (math.sqrt((6.484 ** 2) - (X_i + 0.5 * x_val)) ** 2) - Y_i
        df["Rel. Height"] = df["Delta Y"] + df["ToF"]  # calculating relative height
        df["Height Diff."] = df["Rel. Height"].max() - df["Rel. Height"]  # shim height needed to level each pier
        df["Height Diff."] = pd.to_numeric(df["Height Diff."], errors="coerce")  # converting string to float

        def find_nearest(value):  # finding shim thickness to offset leveling
            if (value == 0 or np.isnan(value) == True):
                return 0
            else:
                shim_thickness = [0.125, 0.25, 0.5, 0.75, 1]  # common shim thicknesses
                idx = (np.abs(shim_thickness - value)).argmin()
                return shim_thickness[idx]

        for i in range(len(df.index)):  # iterating through rows to find shim thickness required for each pier
            value = df.loc[i, "Height Diff."]
            df.loc[i, "Shim Thick."] = find_nearest(value)

        export_file_path = filedialog.asksaveasfilename(defaultextension='.csv')
        df.to_csv(export_file_path, index=None, header=True)

root = Tk()
root.wm_title('USER GUI')
app = App(root)
root.mainloop()
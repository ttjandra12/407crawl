# The code for changing pages was derived from: http://stackoverflow.com/questions/7546050/switch-between-two-frames-in-tkinter
# License: http://creativecommons.org/licenses/by-sa/3.0/

import tkinter as tk
from tkinter import *
from tkinter import filedialog
import pandas as pd
import numpy as np
import math

LARGE_FONT= ("Verdana", 12)

class App(tk.Tk):

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        container = tk.Frame(self)

        container.pack(side="top", fill="both", expand=True)

        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.shared_data = {
            "x_m": tk.DoubleVar(),
            "y_m": tk.DoubleVar()
        }

        self.frames = {}

        for F in (StartPage, PageOne, PageTwo):
            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()


class StartPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)

        self.controller = controller

        label = tk.Label(self, text="407 Crawl User GUI", font=LARGE_FONT)
        label.pack(pady=10,padx=10)

        button = tk.Button(self, text="Import Data", command=self.post_proc_data, height=1, width=15).pack()
        button = tk.Button(self, text="Setup", command=lambda: controller.show_frame(PageTwo), height=1, width=15).pack()
        button2 = tk.Button(self, text="Post-Process Data", command=lambda: controller.show_frame(PageOne),
                            height=1, width=15).pack()

    def post_proc_data(self):
        global df

        x_m = self.controller.shared_data["x_m"].get()
        y_m = self.controller.shared_data["y_m"].get()

        print(x_m)
        print(y_m)

        X_i = 0.5 * (float(x_m) - 1)
        Y_i = 0.5 * (float(y_m) - 1)

        import_file_path = filedialog.askopenfilename(defaultextension='.csv')
        # reading raw data from SD card
        df = pd.read_csv(import_file_path, sep=",", header=None)
        df.columns = ["Pitch", "Roll", "ToF", "ToF Scissor Jack"]  # adding column headers
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

class PageOne(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(self, text="Export Data", font=LARGE_FONT)
        label.pack(pady=10,padx=10)

        button1 = tk.Button(self, text="Export Data", command=self.exportCSV).pack()

    def exportCSV(self):
        export_file_path = filedialog.asksaveasfilename(defaultextension='.csv')
        df.to_csv(export_file_path, index=None, header=True)

class PageTwo(tk.Frame):
    global x_m, y_m

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(self, text="X measured").grid(row=0)
        label1 = tk.Label(self, text="Y measured").grid(row=1)
        button2 = tk.Button(self, text="Restart", command=lambda: controller.show_frame(StartPage)).grid(row=3)
        self.e1 = DoubleVar()
        self.e2 = DoubleVar()
        e1 = tk.Entry(self, textvariable=self.controller.shared_data["x_m"]).grid(row=0, column=1)
        e2 = tk.Entry(self, textvariable=self.controller.shared_data["y_m"]).grid(row=1, column=1)

app = App()
app.mainloop()
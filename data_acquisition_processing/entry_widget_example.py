import tkinter as tk
from tkinter import ttk

# Creating and placing the widgets
root = tk.Tk()
root.wm_title('floating point entry')
qedit = ttk.Entry(root, justify='right')
qedit.insert(0, '100')
qedit.grid(row=0, column=0, sticky='ew')
result = ttk.Label(root, text='100')
result.grid(row=1, column=0)
ttk.Button(root, text="Exit", command=root.quit).grid(row=2, column=0)


# Callback functions
def is_number(data):
    if data == '':
        return True
    try:
        float(data)
        print('value:', data)
    except ValueError:
        return False
    result.event_generate('<<UpdateNeeded>>', when='tail')
    return True


def do_update(event):
    w = event.widget
    number = float(qedit.get())
    w['text'] = '{}'.format(number)

# The following settings can only be done after both the
# widgets and callbacks have been created.
vcmd = root.register(is_number)
qedit['validate'] = 'key'
qedit['validatecommand'] = (vcmd, '%P')
result.bind('<<UpdateNeeded>>', do_update)

# Run the event loop.
root.mainloop()
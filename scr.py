
import platform
import os
import time
import tkinter as tk

def lock_screen():
    os_name = platform.system()
    print(f"Locking the screen on {os_name}")
    if os_name == 'Darwin':
        print("Activating screen saver to lock the screen")
        os.system('/System/Library/CoreServices/"ScreenSaverEngine.app"/Contents/MacOS/"ScreenSaverEngine"')

def on_key_press(event):
    print("")
    lock_screen()
    root.destroy() 
    exit()

def on_mouse_click(event):
    print("")
    lock_screen()
    root.destroy()
    exit()

def on_mouse_move(event):
    print("")
    lock_screen()
    root.destroy()
    exit()

def main():
    global root
    print("")
    
    while True:
        time.sleep(0.5)
        root = tk.Tk()
        root.bind_all('<Key>', on_key_press)
        root.bind_all('<Button>', on_mouse_click)
        root.bind_all('<Motion>', on_mouse_move)
        root.mainloop()

if __name__ == "__main__":
    main()
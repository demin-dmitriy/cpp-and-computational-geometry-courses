from tkinter import *

width = 800
height = 600

def rgb2hex(rgb):
    return '#%02x%02x%02x' % rgb

class Display:
    class Range:
        def __init__(self, min, max):
            self.min = min
            self.max = max

    def canvas_click(self, event):
        relative = self.absolute2relative(event.x, event.y)
        digit_count = max(len(str(width)), len(str(height)))
        self.status_text.configure(text = "Absolute: {:<{digit_count}} {:<{digit_count}}. Relative: {:+.2f} {:+.2f}."
                .format(event.x, event.y, relative[0], relative[1], digit_count=digit_count))

    def __init__(self, width, height, xRange=Range(-10, 10), yRange=Range(-10, 10)):
        self.root = Tk()
        self.root.title('Visualizer')
        self.bottomPanel = Frame(self.root)
        self.bottomPanel.pack(side=BOTTOM, fill=BOTH, expand=YES)
        self.canvas = Canvas(self.root, bg='black', width=width, height=height)
        self.canvas.bind("<Button-1>", self.canvas_click)
        self.canvas.pack(side=TOP)
        self.status_text = Label(self.bottomPanel, text="some text")
        self.status_text.pack()
        self.width = width
        self.height = height
        self.xRange = xRange
        self.yRange = yRange
        self.root.bind('<Key-Escape>', self.end)
    
    def end(self, event):
        exit(0)

    def relative2absolute(self, x, y):
        x_res = (x - self.xRange.min) / (self.xRange.max - self.xRange.min) * self.width
        y_res = (y - self.yRange.min) / (self.yRange.max - self.yRange.min) * self.height
        return (int(x_res), int(y_res))
    
    def absolute2relative(self, x, y):
        x_res = x * ((self.xRange.max - self.xRange.min) / self.width) + self.xRange.min
        y_res = y * ((self.yRange.max - self.yRange.min) / self.height) + self.yRange.min
        return (x_res, y_res)
        
    def checkCordInArea(self, x, y):
        if self.xRange.min <= x <= self.xRange.max and self.yRange.min <= y <= self.yRange.max:
            return True
        else:
            return False

    def putPoint(self, p, color=(255, 0, 0)):
        color = rgb2hex(color)
        x, y = p
        if self.checkCordInArea(x, y):
            x, y = self.relative2absolute(x, y)
            self.canvas.create_rectangle(x, y, x, y, fill=color, outline=color)
        
    def putPoints(self, point_list, color=(255, 0, 0)):
        for point in point_list:
            self.putPoint(point, color)
    
    def putPolygon(self, point_list, color=(0, 150, 150)):
        cord_list = []
        for point in point_list:
            p = self.relative2absolute(*point)
            cord_list.append(p[0])
            cord_list.append(p[1])
        self.canvas.create_polygon(*cord_list, outline=rgb2hex(color), fill='')
        
    def run(self):
        self.root.mainloop()

def loadPointsFromString(str):
    l = []
    for line in str.split('\n'):
        if line:
            point = line.split(' ')
            l.append((float(point[0]), float(point[1])))
    return l

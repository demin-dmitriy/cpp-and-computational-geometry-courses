import sys
sys.path.insert(0, "../lib")
from visualizer_base import*
from subprocess import Popen, PIPE

width = 800
height = 800

solution = ".\\..\\18-cgal-solution-build\\voronoi_diagram_cgal.exe"
sys.path.insert(0, "../18-build")

def norm(vector):
    return vector[0] ** 2 + vector[1] ** 2

def test(file):
    input = open(file).read().partition('\n')[2]
    points = loadPointsFromString(input)

    p = Popen([solution], stdin=open(file), stdout=PIPE, shell=True, universal_newlines=True)
    
    lines = list(filter(None, map(str.strip, p.stdout.read().split('\n'))))
    n = int(lines[0])
    points_desc = [list(map(lambda x: int(x) - 1, line.split()[1:])) for line in lines[1:1+n]]
    vertices = []
    for p in points_desc:
        D = 2 * (points[p[0]][0] * (points[p[1]][1] - points[p[2]][1]) + points[p[1]][0] * (points[p[2]][1] - points[p[0]][1]) + points[p[2]][0] * (points[p[0]][1] - points[p[1]][1]))
        x = (norm(points[p[0]]) * (points[p[1]][1] - points[p[2]][1]) + norm(points[p[1]]) * (points[p[2]][1] - points[p[0]][1]) + norm(points[p[2]]) * (points[p[0]][1] - points[p[1]][1])) / D
        y = -(norm(points[p[0]]) * (points[p[1]][0] - points[p[2]][0]) + norm(points[p[1]]) * (points[p[2]][0] - points[p[0]][0]) + norm(points[p[2]]) * (points[p[0]][0] - points[p[1]][0])) / D
        vertices.append((x, y))
    
    if points:
        union = points + vertices
        minX = min(union, key=lambda x: x[0])[0]
        maxX = max(union, key=lambda x: x[0])[0]
        minY = min(union, key=lambda x: x[1])[1]
        maxY = max(union, key=lambda x: x[1])[1]
        d = abs((maxX - minX) - (maxY - minY)) / 2
        if maxX - minX > maxY - minY:
            maxY += d
            minY -= d
        else:
            maxX += d
            minX -= d
        xRange = Display.Range(minX, maxX)
        yRange = Display.Range(minY, maxY)
    else:
        xRange = Display.Range(-1, 1)
        yRange = Display.Range(-1, 1)

    d = Display(width, height, xRange, yRange)
    d.putPoints(points)
    
    m = int(lines[1 + n])
    for edge in lines[2 + n : 2 + n + m]:
        token = edge.split()
        if token[0] == "infinity":
            if token[1] != "infinity":
                print("Incorrect format.")
                exit(-1)
            a = int(token[2]) - 1
            b = int(token[3]) - 1
            center = ((points[a][0] + points[b][0]) / 2, (points[a][1] + points[b][1]) / 2)
            direction =  (points[a][1] - points[b][1], points[a][0] - points[b][0])
            d.putStraightLine(center, direction)
        else:
            if token[1] != "infinity":
                d.putLine(vertices[int(token[0]) - 1], vertices[int(token[1]) - 1])
            else:
                a = int(token[2]) - 1
                b = int(token[3]) - 1
                direction = (points[a][1] - points[b][1], points[b][0] - points[a][0])
                d.putRay(vertices[int(token[0]) - 1], direction)
    d.putPoints(vertices, color = (0, 0, 255))
    d.run()

if __name__ == "__main__":
    for i in range(48):
        print("test {}".format(i))
        test(".\\correctness_tests\\{}.in".format(str(i).zfill(3)))
    for i in range(3):
        print("test {}".format(i))
        test(".\\performance_tests\\{}.in".format(str(i).zfill(3)))
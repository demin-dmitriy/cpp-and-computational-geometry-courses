import sys
sys.path.insert(0, "../lib")
from visualizer_base import*
from subprocess import Popen, PIPE

width = 800
height = 600

solution = ".\\..\\10-build\\minimum_circumscribed_circle.exe"

def test(file):
    input = open(file).read().partition('\n')[2]
    points = loadPointsFromString(input)

    if points:
        minX = min(points, key=lambda x: x[0])[0]
        maxX = max(points, key=lambda x: x[0])[0]
        minY = min(points, key=lambda x: x[1])[1]
        maxY = max(points, key=lambda x: x[1])[1]
        xRange = Display.Range(minX, maxX)
        yRange = Display.Range(minY, maxY)
    else:
        xRange = Display.Range(-1, 1)
        yRange = Display.Range(-1, 1)

    d = Display(width, height, xRange, yRange)
    d.putPoints(points)

    p = Popen([solution], stdin=open(file), stdout=PIPE, shell=True, universal_newlines=True)
    out_str = p.stdout.read().partition('\n')[2]
    out = [points[int(index) - 1] for index in filter(None, out_str.split(' '))]
    print("Out: ", out)
    if out:
        d.putPolygon(out)
    d.run()

if __name__ == "__main__":
    print("custom test 0")
    for i in range(1, 7):
        print("test {}".format(i))
        test(".\\..\\10-build\\{}.in".format(i))
    for i in range(1, 51):
        print("test {}".format(i))
        test(".\\..\\10-build\\correctness_tests\\{}.in".format(str(i)))
    for i in range(1, 21):
        print("test {}".format(i))
        test(".\\..\\10-build\\correctness_tests\\{}.in".format(str(i)))
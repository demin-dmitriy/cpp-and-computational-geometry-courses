import sys
sys.path.insert(0, "../lib")
from visualizer_base import*
from subprocess import Popen, PIPE

width = 800
height = 600

solution = ".\\..\\01-build\\convex_hull.exe"

def test(file):

    input = open(file).read().partition('\n')[2]
    points = loadPointsFromString(input)
    #print("In:  ", points)

    minX = min(points, key=lambda x: x[0])[0]
    maxX = max(points, key=lambda x: x[0])[0]
    minY = min(points, key=lambda x: x[1])[1]
    maxY = max(points, key=lambda x: x[1])[1]
    xRange = Display.Range(minX - 100, maxX + 100)
    yRange = Display.Range(minY - 100, maxY + 100)

    d = Display(width, height, xRange, yRange)
    d.putPoints(points)

    p = Popen([solution], stdin=open(file), stdout=PIPE, shell=True, universal_newlines=True)
    out = loadPointsFromString(p.stdout.read().partition('\n')[2])
    print("Out: ", out)
    d.putPolygon(out)

    d.run()

if __name__ == "__main__":
    test(".\\..\\01-build\\correctness_tests\\035.in")
    # for i in range(1, 48):
        # print("test {}".format(i))
        # test(".\\..\\01-build\\correctness_tests\\{}.in".format(str(i).zfill(3)))
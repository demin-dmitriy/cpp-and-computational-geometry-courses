import sys
sys.path.insert(0, "../lib")
from visualizer_base import*
from subprocess import Popen, PIPE

width = 800
height = 600

solution = ".\\..\\15-build\\segment_set_intersection.exe"

class Segment:
    def __init__(self, a, b):
        self.a = a
        self.b = b
        
def loadSegmentsFromString(str):
    segments = []
    for line in str.split('\n'):
        t = list(filter(None, line.split(' ')))
        if t:
            x1, y1, x2, y2 = t
            segments.append(Segment((float(x1), float(y1)), (float(x2), float(y2))))
    return segments

def test(file):
    input = open(file).read().partition('\n')[2]
    segments = loadSegmentsFromString(input)

    minX = min(segments, key=lambda x: min(x.a[0], x.b[0]))
    maxX = max(segments, key=lambda x: max(x.a[0], x.b[0]))
    minY = min(segments, key=lambda x: min(x.a[1], x.b[1]))
    maxY = max(segments, key=lambda x: max(x.a[1], x.b[1]))
    xRange = Display.Range(min(minX.a[0], minX.b[0]), max(maxX.a[0], maxX.b[0]))
    yRange = Display.Range(min(minY.a[1], minY.b[1]), max(maxY.a[1], maxY.b[1]))

    d = Display(width, height, xRange, yRange)
    for segment in segments:
        d.putLine(segment.a, segment.b)

    p = Popen([solution], stdin=open(file), stdout=PIPE, shell=True, universal_newlines=True)
    intersections = []
    out = filter(None, p.stdout.read().split('\n')[1:])
    for line in out:
        tokens = line.split()
        intersections.append(list(map(int, tokens[1:])))
    print("Out: ", intersections)
    for intersection in intersections:
        s1 = segments[intersection[0] - 1]
        s2 = segments[intersection[1] - 1]
        A1 = s1.a[1] - s1.b[1]
        B1 = s1.b[0] - s1.a[0]
        C1 = s1.a[0] * s1.b[1] - s1.a[1] * s1.b[0]
        A2 = s2.a[1] - s2.b[1]
        B2 = s2.b[0] - s2.a[0]
        C2 = s2.a[0] * s2.b[1] - s2.a[1] * s2.b[0]
        Det = A1 * B2 - A2 * B1
        point = ((B1 * C2 - B2 * C1) / Det, (C1 * A2 - C2 * A1) / Det)
        print(point)
        if Det != 0:
            d.putPoint(point)
    d.run()

if __name__ == "__main__":
    test(".\\..\\15-build\\correctness_tests\\13.in")
    print("Correctness tests:")
    for i in range(1, 51):
        print("test {}".format(i))
        test(".\\..\\15-build\\correctness_tests\\{}.in".format(str(i).zfill(2)))
    print("Performance tests:")
    for i in range(1, 2):
        print("test {}".format(i))
        test(".\\..\\15-build\\performance_tests\\{}.in".format(str(i).zfill(2)))

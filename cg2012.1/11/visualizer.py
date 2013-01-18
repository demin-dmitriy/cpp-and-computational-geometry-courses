import sys
sys.path.insert(0, "../lib")
from visualizer_base import*
from subprocess import Popen, PIPE

width = 800
height = 600

solution = ".\\..\\11-build\\point_set_diameter.exe"

def test(file):

    input = open(file).read().partition('\n')[2]
    points = loadPointsFromString(input)
    answer = 0
    p_answer = None
    q_answer = None
    for p in points:
        for q in points:
            V = (p[0] - q[0]) ** 2 + (p[1] - q[1]) ** 2
            if V > answer:
                p_answer = p
                q_answer = q
                answer = V
    print("Answer: ", answer)

    minX = min(points, key=lambda x: x[0])[0]
    maxX = max(points, key=lambda x: x[0])[0]
    minY = min(points, key=lambda x: x[1])[1]
    maxY = max(points, key=lambda x: x[1])[1]
    xRange = Display.Range(minX - 1, maxX + 1)
    yRange = Display.Range(minY - 1, maxY + 1)

    d = Display(width, height, xRange, yRange)
    d.putPoints(points)

    p = Popen([solution], stdin=open(file), stdout=PIPE, shell=True, universal_newlines=True)
    out = loadPointsFromString(p.stdout.read())
    print("Out: ", out)
    d.putPolygon(out)
    #d.putPolygon([p_answer, q_answer])

    d.run()

if __name__ == "__main__":
    # test(".\\..\\11-build\\correctness_tests\\06.in")
    # test(".\\..\\11-build\\correctness_tests\\14.in")
    for i in range(1, 21):
        print("test {}".format(i))
        test(".\\..\\11-build\\correctness_tests\\{}.in".format(str(i).zfill(2)))
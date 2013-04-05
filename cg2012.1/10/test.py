from subprocess import call
from time import perf_counter

from os import chdir
chdir("..\\10-build")

solution = "minimum_circumscribed_circle.exe"
checker = "minimumcircumscribedcircle-checker.exe"

tmp_file = "tmp.out"

def test(file):
    file_in = open(file)
    output_name = "res.out"
    file_out = open(output_name, "w")
    t1 = perf_counter()
    res = call([solution], stdin=file_in, stdout=file_out)
    t2 = perf_counter()
    print("    Completed in {}.".format(t2 - t1))
    if res != 0:
        print("    Error: program returned non-zero.")
        return
    file_out.close()
    t1 = perf_counter()
    res = call([checker, file, output_name])
    t2 = perf_counter()
    print("    Checker running time is {}.".format(t2 - t1))
    if res == 0:
        print("    Correct.")
    else:
        print("    Wrong answer.")

if __name__ == "__main__":
    print("My tests:")
    for i in range(78):
        print("test {}:".format(i))
        test("..\\testgen-build\\tests\\{}.in".format(i))
    print("Custom tests:")
    for i in range(1, 7):
        print("test {}:".format(i))
        test("{}.in".format(str(i)))
    print("Correctness tests:")
    for i in range(1, 51):
        print("test {}:".format(i))
        test("correctness_tests\\{}.in".format(str(i)))
    print("Performance tests:")
    for i in range(1, 21):
        print("test {}:".format(i))
        test("performance_tests\\{}.in".format(str(i)))
    
# Note: running checker for performance tests takes forever.

from subprocess import call
from time import perf_counter

from os import chdir
from sys import argv
chdir("..\\13-build")

solution = "delaunay_triangulation.exe"
checker = "delaunay_triangulation-checker.exe"

tmp_file = "res.out"

def run_solution(solution, input_file, output_file):
    file_in = open(input_file)
    file_out = open(output_file, "w")
    t1 = perf_counter()
    res = call([solution], stdin=file_in, stdout=file_out)
    t2 = perf_counter()
    if res != 0:
        print("    Error: program returned non-zero.")
        return
    file_out.close()
    file_in.close()
    return t2 - t1

def test(file):
    t = run_solution(solution, file, tmp_file)
    print("    Solution completed in {}.".format(t))
    t1 = perf_counter()
    res = call([checker, file, tmp_file])
    t2 = perf_counter()
    print("    Checker completed in {}.".format(t2 - t1))
    if res == 0:
        print("    Correct.")
    else:
        print("    Wrong answer.")

def run_all_tests():
    print("Correctness tests:")
    for i in range(1, 21):
        print("test {}:".format(i))
        test("correctness_tests\\{}.in".format(str(i).zfill(3)))
    print("Performance tests:")
    for i in range(1, 3):
        print("test {}:".format(i))
        test("performance_tests\\{}.in".format(str(i).zfill(3)))
 
if __name__ == "__main__":
    if len(argv) >= 2:
        prefix = "correctness_tests"
        if len(argv) == 2:
            num = int(argv[1])
        else:
            if argv[1] == "p":
                prefix = "performance_tests"
            num = int(argv[2])
        test("{}\\{}.in".format(prefix, str(num).zfill(3)))
    else:
        run_all_tests()
    
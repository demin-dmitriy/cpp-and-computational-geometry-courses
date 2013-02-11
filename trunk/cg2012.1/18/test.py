from subprocess import call
from time import perf_counter

from os import chdir
from sys import argv
chdir("..\\18-build")

tested_solution = "voronoi_diagram.exe"
correct_solution = "voronoi_diagram_cgal.exe"
checker = "voronoi_diagram_checker.exe"

tmp_file1 = "res1.out"
tmp_file2 = "res2.out"

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
    t = run_solution(correct_solution, file, tmp_file1)
    print("    Cgal solution completed in {}.".format(t))
    t = run_solution(tested_solution, file, tmp_file2)
    print("    Solution completed in {}.".format(t))
    res = call([checker, tmp_file1, tmp_file2])
    if res == 0:
        print("    Correct.")
    else:
        print("    Wrong answer.")

def run_all_tests():
    print("Correctness tests (0):")
    for i in range(3):
        print("test {}:".format(i))
        test("correctness_tests_0\\{}.in".format(str(i).zfill(3)))
    print("Correctness tests (1):")
    for i in range(48):
        print("test {}:".format(i))
        test("correctness_tests_1\\{}.in".format(str(i).zfill(3)))
    print("Performance tests:")
    for i in range(3):
        print("test {}:".format(i))
        test("performance_tests\\{}.in".format(str(i).zfill(3)))
 
if __name__ == "__main__":
    if len(argv) >= 2:
        prefix = "correctness_tests_1"
        if len(argv) == 2:
            num = int(argv[1])
        else:
            if argv[1] == "p":
                prefix = "performance_tests"
            elif argv[1] == "c0":
                prefix = "correctness_tests_0"
            num = int(argv[2])
        test("{}\\{}.in".format(prefix, str(num).zfill(3)))
    else:
        run_all_tests()
    
from subprocess import call
from time import perf_counter

from os import chdir
from sys import argv
chdir("..\\15-build")

tested_solution = "segment_set_intersection.exe"
checker = "segment_set_intersection_checker.exe"

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
    t = run_solution(tested_solution, file, tmp_file)
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
    for i in range(1, 51):
        print("test {}:".format(i))
        test("correctness_tests\\{}.in".format(str(i).zfill(2)))
    # Performance test takes too much time.
    # print("Performance tests:")
    # for i in range(1, 2):
        # print("test {}:".format(i))
        # test("performance_tests\\{}.in".format(str(i).zfill(2)))
 
if __name__ == "__main__":
    if len(argv) >= 2:
        prefix = "correctness_tests"
        if len(argv) == 2:
            num = int(argv[1])
        else:
            if argv[1] == "p":
                prefix = "performance_tests"
            elif argv[1] == "c":
                prefix = "correctness_tests"
            num = int(argv[2])
        test("{}\\{}.in".format(prefix, str(num).zfill(2)))
    else:
        run_all_tests()
    
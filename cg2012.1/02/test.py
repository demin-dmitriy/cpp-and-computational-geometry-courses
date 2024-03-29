from subprocess import call
from time import perf_counter

from os import chdir
chdir("..\\02-build")

solution = "segment_rasterization.exe"
checker = "segment_rasterization-checker.exe"

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
    print("Correctness tests:")
    for i in range(25):
        print("test {}:".format(i))
        test("correctness_tests\\{}.in".format(str(i).zfill(3)))
    # Note: Perfomance tests take many time because of a lot of io.
    print("Perfomance tests:")
    for i in range(2):
        print("test {}:".format(i))
        test("performance_tests\\{}.in".format(str(i).zfill(3)))
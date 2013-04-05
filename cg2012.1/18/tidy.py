# Удаляет точки-дубликаты из тестов.

from collections import OrderedDict

input_dir = "..\\testgen-build\\tests"
output_dir = "..\\testgen-build\\tests-unique-points"

def tidy(in_file, out_file):
    lines = list(filter(None, in_file.read().split('\n')))
    dict = OrderedDict.fromkeys(lines[1:])
    unique_lines = "\n".join(dict)
    n = len(dict)
    out_file.write(str(n) + '\n' + unique_lines)
    return len(lines[1:]) != n
    
if __name__ == "__main__":
    for i in range(78):
        name = "{}.in".format(i)
        if tidy(open("{}\\{}".format(input_dir, name)),
                open("{}\\{}".format(output_dir, name), 'w')):
            print(name)
             
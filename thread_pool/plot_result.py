import matplotlib.pyplot as plt
import sys

def read_result_file(filename):
    res = [[], []]
    with open(filename) as res_file:
        for line in res_file:
            space = line.find(' ')
            m = line.find('m')
            s = line.find('s')

            i = int(line[:space])
            res[0].append(i)

            minutes = float(line[space + 1:m])
            seconds = float(line[m + 1:s])
            seconds += minutes * 60
            res[1].append(seconds)

    return res

if __name__ == '__main__':
    result = read_result_file(sys.argv[1])
    plt.xlabel('cores/array size')
    plt.ylabel('time')
    plt.plot(result[0], result[1])
    plt.grid(True)
    plt.show()


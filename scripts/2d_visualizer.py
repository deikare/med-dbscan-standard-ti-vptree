import numpy as np
import argparse
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
from typing import Tuple


def parser() -> Tuple[str, int, bool]:
    parser = argparse.ArgumentParser(description='Description of your program')
    parser.add_argument(
        '-f', '--filename', help='provide filename', required=True)
    parser.add_argument(
        '-i', '--ignore-lines', help='ignore n initial lines of file in reading points', required=True, type=int)
    parser.add_argument('--eps', help='if epsilon value is in file - it should be first line', required=False, action='store_true', default=False)
    args = vars(parser.parse_args())
    filename = args['filename']
    ignore_rows = args['ignore_lines']
    epsilon_defined = args['eps']
    return filename, ignore_rows, epsilon_defined


def read_data(filename: str, ignore_lines: int, epsilon_defined: bool) -> Tuple[pd.DataFrame, int, float]:
    file = open(filename)
    if epsilon_defined:
        eps_value = float(file.readline())
    else:
        eps_value = None
    file.close()

    file = open(filename)
    data = pd.read_csv(file, skiprows=ignore_lines, header=None)
    classes_num = data.nunique()[len(data.columns)-1]
    print(type(data))
    return data, classes_num, eps_value


def make_plot(data: pd.DataFrame, classes_num: int, eps_value: float) -> None:
    colors = cm.rainbow(np.linspace(0, 1, classes_num))
    for i in range(len(data)):
        plt.plot(data[0][i], data[1][i], 'o', color=get_color(data, colors, i))

    if eps_value:
        for i in range(len(data)):
            circle = plt.Circle((data[0][i], data[1][i]), eps_value, color=get_color(data, colors, i), alpha=0.1)
            plt.gca().add_patch(circle)

    plt.xlabel('x coordinates')
    plt.ylabel('y coordinates')
    plt.show()

def get_color(data: pd.DataFrame, colors, i: int) -> Tuple[float, float, float]:
    if data[2][i] < 0:
        color = (0, 0, 0) #print black circle
    else:
        color = colors[data[2][i]]
    
    return color

def main():
    filename, ignore_rows, epsilon_defined = parser()
    data, classes_num, eps_value = read_data(filename, ignore_rows, epsilon_defined)
    make_plot(data, classes_num, eps_value)


if __name__ == "__main__":
    main()

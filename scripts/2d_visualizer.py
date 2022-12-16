import numpy as np
import argparse
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
from typing import Tuple


def parser() -> Tuple[str, int]:
    parser = argparse.ArgumentParser(description='Description of your program')
    parser.add_argument(
        '-f', '--filename', help='provide filename', required=True)
    parser.add_argument(
        '-i', '--ignore-lines', help='ignore n initial lines of file', required=True, type=int)
    args = vars(parser.parse_args())
    filename = args['filename']
    ignore_rows = args['ignore_lines']
    return filename, ignore_rows


def read_data(filename: str, ignore_lines: int) -> Tuple[pd.DataFrame, int]:
    file = open(filename)
    data = pd.read_csv(file, skiprows=ignore_lines, header=None)
    classes_num = data.nunique()[len(data.columns)-1]
    print(type(data))
    return data, classes_num


def make_plot(data: pd.DataFrame, classes_num: int) -> None:
    colors = cm.rainbow(np.linspace(0, 1, classes_num))
    for i in range(len(data)):
        plt.plot(data[0][i], data[1][i], 'o', color=colors[data[2][i]])
    plt.xlabel('x coordinates')
    plt.ylabel('y coordinates')
    plt.show()


def main():
    filename, ignore_rows = parser()
    data, classes_num = read_data(filename, ignore_rows)
    make_plot(data, classes_num)


if __name__ == "__main__":
    main()

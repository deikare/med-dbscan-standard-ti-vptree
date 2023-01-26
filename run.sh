#!/bin/bash

if [ $# -gt 1 ]; then
  echo "wrong number of arguments"
  exit 4
elif [ $# -eq 1 ]; then
  (cd build && make && ./DBSCAN -f ../data/DS-850.arff -i 12 && cd ../scripts && python3 2d_visualizer.py -f ../data/dbscan-result-new.csv -i 1 --eps)
else
  (cd build && make && ./DBSCAN -f ../data/letter.arff -i 12 && cd ../scripts && python3 2d_visualizer.py -f ../data/dbscan-result-new.csv -i 1)
fi

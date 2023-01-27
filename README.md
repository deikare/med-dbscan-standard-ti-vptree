# Informacja o projekcie
Implementacja TI-DBSCAN i VP-tree-DBSCAN.

# Budowa
Pierwsze uruchomienie:
``` console
mkdir build
cmake -B build .
cd build
make
./DBSCAN
```

Jeśli folder `build` istnieje i pliki cmake zostały już wygenerowane:
``` console
cd build
make
./DBSCAN
```

# Testowanie
Podczas testów warto zwrócić uwagę aby zmienić parametr `CMAKE_BUILD_TYPE` z `Debug` na `Release` w pliku `CMakeLists.txt`.

Program należy wywołać z poprawnymi parametrami. Aby sprawdzić jakie opcje są dostępne można użyć flagi -h. Wtedy zostaną wyświetlone wszystkie dostępne opcje:
```console
foo@bar:~/MED/build$ ./DBSCAN -h
Program options: 
-h get help 

Obligatory: 
-f provide filename 
-i ignore n initial lines of file 

Optional: 
-a [std|ti|vp] - algorithm version, default = std
-e epsilon, default = 1 
-m minPts, default = 2 
-rv [min|max|point=0,0,...,0|refOpts=min,max,...,max] - specify reference point
type for TI, default = max 
-p minkowskiParameter - specify minkowskiParameter for distance calculation, default = 2
```

Flaga -f pozwala na dodanie pliku z danymi, które mają być wczytane do programu. Natomiast flaga -i pozwala wyłączyć z wczytywania n pierwszych linii tego pliku, które mogą stanowić nagłówki lub metadane. Przykładowe wywołanie programu na danych testowych dla wersji TI-DBSCAN można wykonać jak poniżej:
```console
foo@bar:~/MED/build$ ./DBSCAN -f ../data/simple2D.csv -i 2 -a ti -e 1 -m 3
```

Parametry dla konkretnych zbiorów danych, dla których wygenerowano pliki wynikowe (parametr ):
- simple2D.arff
    - i = 2
    - epsilon = 1
    - minPts = 3
- complex9.arff
    - i = 9
    - epsilon = 10
    - minPts = 5
- DS-850.arff
    - i 12
    - epsilon = 0.45
    - minPts = 5
- letter.arff
    - i 191
    - epsilon = 5
    - minPts = 150

# Wizualizacja wyników
Wykresy testowe generowano za pomocą Pythona 3.8.10 i środowiska wirtualnego zbudowanego w oparciu o plik requirements.txt. Aby wygenerować wykres należy przejść do folderu
scripts i wykonać poniższe instrukcje.
``` console
foo@bar:~/MED/scripts$ python3 -m venv venv
foo@bar:~/MED/scripts$ source venv/bin/activate
foo@bar:~/MED/scripts$ python -m pip install -r requirements.txt
foo@bar:~/MED/scripts$ python 2d_visualizer.py -f ../data/simple2D.csv -i 2 --eps
```
W przypadku skryptu do generowania wykresów (podobnie jak dla właściwego programu) jest dostępna instrukcja użytkownika. Można ją wyświetlić za pomocą flagi -h.

``` console
foo@bar:~/MED/scripts$ python 2d_visualizer.py -h
usage: 2d_visualizer.py [-h] -f FILENAME -i IGNORE_LINES [--eps]

Description of your program

optional arguments:
  -h, --help            show this help message and exit
  -f FILENAME, --filename FILENAME
                        provide filename
  -i IGNORE_LINES, --ignore-lines IGNORE_LINES
                        ignore n initial lines of file in reading points
  --eps                 if epsilon value is in file - it should be first
                        line
```

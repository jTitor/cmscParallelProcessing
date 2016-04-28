#!/bin/bash

# Define a timestamp function
timestamp() {
  date +"%y-%d-%m_%H-%M-%S"
}

# compile code
make clean
make
rm temp.txt
rm times.txt

echo "Starting at $(timestamp)" >> times.txt

# run with varying number of processes
procs=( 1, 2, 4, 8, 16 )

# do it for cameraman (easy task)
for i in "${procs[@]}"
do
	echo "Time for cameraman256->cameraman250 np=$i" >> times.txt
	{ time ./Proj6 ../img/cameraman256.bmp ../img/cameraman250.bmp 250 250 $i ; } 2> temp.txt
	grep -i 'real' temp.txt >> times.txt
	grep -i 'user' temp.txt >> times.txt
done

# do it for lena (difficult task)
for i in "${procs[@]}"
do
	echo "Time for lena512->lena300 np=$i" >> times.txt
	{ time ./Proj6 ../img/lena512.bmp ../img/lena300.bmp 250 250 $i ; } 2> temp.txt
	grep -i 'real' temp.txt >> times.txt
	grep -i 'user' temp.txt >> times.txt
done

echo "Ending at $(timestamp)" >> times.txt

# finish up
mv times.txt "times_$(timestamp).txt"
rm temp.txt
rm times.txt
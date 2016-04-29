#!/bin/bash

# Define a timestamp function
timestamp() {
  date +"%y-%d-%m_%H-%M-%S"
}

tmpfile="temp$(timestamp)"
timefile="temp-times$(timestamp)"
logfile="log-times$(timestamp)"

# compile code
# make clean
# make

echo "Starting at $(timestamp)" >> $timefile

# run with varying number of processes
procs=( 1, 2, 4, 8, 16 )

# do it for cameraman (easy task)
for i in "${procs[@]}"
do
	echo "Time for cameraman256->cameraman250 np=$i" >> $timefile
	{ time ./Proj6 ../img/cameraman256.bmp ../img/cameraman250.bmp 250 250 $i ; } 2> $tmpfile
	# convert time ouput to seconds
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'user' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile 
	# keep original time output to make sure we arent doing stupid parsing
	cat $tmpfile >> $logfile
done

# do it for lena (difficult task)
for i in "${procs[@]}"
do
	echo "Time for lena512->lena300 np=$i" >> $timefile
	{ time ./Proj6 ../img/lena512.bmp ../img/lena300.bmp 250 250 $i ; } 2> $tmpfile
	# convert time ouput to seconds
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'user' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	cat $tmpfile >> $logfile
done

echo "Ending at $(timestamp)" >> $timefile

# finish up
mv "$timefile" "times_$(timestamp).txt"
rm "$tmpfile"

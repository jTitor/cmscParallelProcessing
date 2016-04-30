#!/bin/bash

# Define a timestamp function
timestamp() {
  date +"%y-%d-%m_%H-%M-%S"
}

host=$(hostname)
tmpfile="temp-$host-$(timestamp)"  # temp file for each run
timefile="times-$host-$(timestamp)"  # temp file with parsed data
logfile="log-$host-$(timestamp)"  # file with raw data
realtime="real-$host-$(timestamp)"  # file with parsed data

# compile code
# make clean
# make

# start new time data file
echo "Hostname: $host" > $timefile
echo "Hostname: $host" > $realtime
echo "Starting at $(timestamp)" >> $timefile 
echo "Starting at $(timestamp)" >> $realtime

# run with varying number of processes
procs=( 1 2 4 8 12 )
#procs=( 12 )

printf "%s" "${procs[@]}" >> $realtime
printf "\n" >> $realtime

echo "cameraman256->cameraman250" >> $realtime
# do it for cameraman (easy task)
for i in "${procs[@]}"
do
	echo "Time for cameraman256->cameraman250 np=$i" >> $timefile
	{ time ./Proj6 ../img/cameraman256.bmp "../img/new/cameraman250-$host-$i.bmp" 250 250 $i ; } 2> $tmpfile
	# convert time ouput to seconds
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'user' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile 
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print (($2*60)+ $3);' >> $realtime
	# keep origcanal time output to make sure we arent doing stupid parsing
	cat "$tmpfile" >> $logfile
done

echo "lena512->lena300" >> $realtime
# do it for lena (difficult task)
for i in "${procs[@]}"
do
	echo "Time for lena512->lena300 np=$i" >> $timefile
	{ time ./Proj6 ../img/lena512.bmp "../img/new/lena300$host-$i.bmp" 250 250 $i ; } 2> $tmpfile
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'user' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print (($2*60)+ $3);' >> $realtime
	cat "$tmpfile" >> $logfile
done

echo "tux800x950->tux400x400" >> $realtime
# do it for tux (very difficult task)
for i in "${procs[@]}"
do
	echo "Time for tux800x950->tux400x400 np=$i" >> $timefile
	 { time ./Proj6 ../img/tux800x950.bmp "../img/new/tux400x400-$host-$i.bmp" 400 400 $i ; } 2> $tmpfile
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'user' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print $1, (($2*60)+ $3);' >> $timefile
	grep -i 'real' $tmpfile | perl -lne '/([a-z\t ]*)([0-9]+)m([0-9\.]+)s/; print (($2*60)+ $3);' >> $realtime
	cat "$tmpfile" >> $logfile
done

echo "Ending at $(timestamp)" >> $timefile 
echo "Ending at $(timestamp)" >> $realtime

# finish up
mv "$timefile" "$timefile.txt"
mv "$logfile" "$logfile.txt"
mv "$logfile" "$logfile.txt"
mv profileData.csv "data/profileData$(timestamp).csv"

rm "$tmpfile"

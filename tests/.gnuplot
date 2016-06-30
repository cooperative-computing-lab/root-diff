set output ".eps"
set terminal postscript eps enhanced
set title "Root diff on "
set xlabel "Size/M"
set ylabel "Time/S"
set yrange [0:6]
unset key
plot ".dat" using 1:3

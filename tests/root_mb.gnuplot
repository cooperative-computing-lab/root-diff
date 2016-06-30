set output "root_mb.eps"
set terminal postscript eps enhanced
set title "Root diff on root_mb"
set xlabel "Size/M"
set ylabel "Time/S"
set yrange [0:6]
unset key
plot "root_mb.dat" using 1:3

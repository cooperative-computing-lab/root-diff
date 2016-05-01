set output "minus.eps"
set terminal postscript eps enhanced
set title "Root diff on minus"
unset key
set xlabel "Size/G"
set ylabel "Time/S"
set yrange [0:45]
set xrange [0:5]
plot "minus.dat" using 1:3

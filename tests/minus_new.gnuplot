set output "minus_new.eps"
set terminal postscript eps enhanced
set title "Root diff on minus_new"
set xlabel "Size/G"
set ylabel "Time/S"
set yrange [0:14]
unset key
plot "minus_new.dat" using 1:3

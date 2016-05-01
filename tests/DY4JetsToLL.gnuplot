set output "DY4JetsToLL.eps"
set terminal postscript eps enhanced
set title "Root diff on DY4JetsToLL"
set xlabel "Size/M"
set ylabel "Time/S"
set yrange [0:4]
unset key
plot "DY4JetsToLL.dat" using 1:3

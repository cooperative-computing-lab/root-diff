set output "root_MB.eps"
set terminal postscript eps enhanced
set title "Root diff on root_MB"
set xlabel "Size/M"
set xrange [0:830]
set ylabel "Time/S"
set yrange [0:5]
unset key
plot "root_MB.dat" using 1:3 smooth csplines, "root_MB.dat" using 1:3 with points

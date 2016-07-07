set output "root_GB.eps"
set terminal postscript eps enhanced
set title "Root diff on root_GB"
set xlabel "Size/G"
set xrange [0:8]
set ylabel "Time/S"
set yrange [0:80]
unset key
plot "root_GB.dat" using 1:3 smooth csplines, "root_GB.dat" using 1:3 with points

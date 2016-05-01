set output "BEAN_GTV7G_V01_CV03.eps"
set terminal postscript eps enhanced
set title "Root diff on BEAN_GTV7G_V01_CV03"
set xlabel "Size/M"
set ylabel "Time/S"
set yrange [0:4]
unset key
plot "BEAN_GTV7G_V01_CV03.dat" using 1:3

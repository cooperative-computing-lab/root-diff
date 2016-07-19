set style data histograms
set style fill solid
set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'tth_ttjets_UC.eps'
set xlabel "Size/G"
set xtics right offset 6,0
set ylabel "Time/S"
set yrange [0:125]
plot "tth_ttjets_UC.dat" using 2:xtic(1) title "tth" lt rgb "#406090",\
     "" using 3 title "ttjets" lt rgb "#40FF00"

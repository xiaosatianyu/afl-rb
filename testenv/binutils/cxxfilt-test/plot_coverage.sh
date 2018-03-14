
#--------------------------------
GNUPLOT=`which gnuplot 2>/dev/null`

if [ "$GNUPLOT" = "" ]; then

  echo "[-] Error: can't find 'gnuplot' in your \$PATH." 1>&2
  exit 1

fi

rm -r './path-coverage.png'
echo "[*] Generating plots..."

(
cat <<_EOF_
set terminal png truecolor enhanced size 1000,350 font "Times New Roman,16" #butt 
set output './mini_distance.png' 

#设置x轴
set xdata time 		#设置x轴为时间
set timefmt '%s'    #时间输入格式设置为seconds since the Unix epoch (1970-01-01, 00:00 UTC)
#set format x "%b %d\n%H:%M" #x轴时间格式
set format x   "%H:%M" #时间格式
#set grid xtics linetype 0 linecolor rgb '#e0e0e0'
#set xtics font  "Times New Roman,16" 
set autoscale xfixmin
set autoscale xfixmax
set xlabel "Time(Hours)" font "Times New Roman,16" 

#设置y轴
#set ytics 80 font  "Times New Roman,16" 
#set grid ytics linetype 0 linecolor rgb '#e0e0e0'
#set ylabel "Number of paths"


#set tics font  "100000" 
#set tics textcolor rgb '#000000'
#unset mxtics
#unset mytics


#设置边
set border #linecolor rgb '#50c0f0'
set grid 

#设置图例
set key outside  Right font "DejaVu Sans,18"  #bottom
#set label "ddd" at 0.5,0.5

#set title "the path number "

plot '/tmp/out-fairfuzz-cxxfilt/master/plot_data'  using 1:4 with lines title 'farifuzz'    linewidth 4 linetype 8,\\
     '/tmp/out-para-cxxfilt/master/plot_data'  using 1:4 with lines title 'para'    linewidth 4 linetype 6 ,\\
######end the high_freq.png


_EOF_

) | gnuplot 


echo "[+] All done - enjoy your charts!"

exit 0

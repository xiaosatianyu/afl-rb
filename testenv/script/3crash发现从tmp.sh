

GNUPLOT=`which gnuplot 2>/dev/null`

if [ "$GNUPLOT" = "" ]; then

  echo "[-] Error: can't find 'gnuplot' in your \$PATH." 1>&2
  exit 1

fi

rm -r './crash发现速度.png' 
echo "[*] Generating plots..."

(

cat <<_EOF_
set terminal png truecolor enhanced size 1000,350 font "Times New Roman,16" #butt 

set output './crash发现速度.png' 


#设置x轴
set xdata time 		#设置x轴为时间
set timefmt '%s'    #时间输入格式设置为seconds since the Unix epoch (1970-01-01, 00:00 UTC)
#set format x "%b %d\n%H:%M" #x轴时间格式
set format x "%H:%M" #时间格式
set xlabel "Time(Hours)" font "Times New Roman,16" 
unset mxtics
#set grid xtics linetype 0 linecolor rgb '#e0e0e0'
set autoscale xfixmin
set autoscale xfixmax


#设置y轴
set ytics 25
#set tics font 'small'
#set ylabel "Number of crashes"
unset mytics
#set grid ytics linetype 0 linecolor rgb '#e0e0e0'


#设置边
set border #linecolor rgb '#50c0f0'
#set tics textcolor rgb '#000000'
set grid 

#设置图例
#set key inside  bottom Right font "DejaVu Sans,18" 
set key inside   Right font "DejaVu Sans,15" 

#设置label
#set label "ddd" at 0.5,0.5


#设置题目
#set title "the number of unique crashes"



plot  '/tmp/1npndnr/plot_data'   using 1:8 with lines title 'npndnr'   linewidth 4 linetype 3 ,\\
	  '/tmp/2p/plot_data'   using 1:8 with lines title 'p'   linewidth 4 linetype 3 ,\\
	  '/tmp/3pr/plot_data'   using 1:8 with lines title 'pd'   linewidth 4 linetype 3 ,\\
	  '/tmp/4pd/plot_data'   using 1:8 with lines title 'rdfuzz'   linewidth 4 linetype 3 ,\\
	  '/tmp/5pdr/plot_data'   using 1:8 with lines title 'pdr'   linewidth 4 linetype 3 
_EOF_

) | gnuplot 




echo "[+] All done - enjoy your charts!"

exit 0

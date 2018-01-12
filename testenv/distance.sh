target="HTML_filter_INTOverflow_eip_1"

#--------------------------------
GNUPLOT=`which gnuplot 2>/dev/null`

if [ "$GNUPLOT" = "" ]; then

  echo "[-] Error: can't find 'gnuplot' in your \$PATH." 1>&2
  exit 1

fi

rm -r './距离变化.png'
echo "[*] Generating plots..."

(
cat <<_EOF_
set terminal png truecolor enhanced size 1000,350 font "Times New Roman,16" #butt 
set output './距离变化.png' 

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
set key inside  bottom Right font "DejaVu Sans,18" 
#set label "ddd" at 0.5,0.5

#set title "the min_distance "

  
plot './output/plot_data'   using 1:13 with lines title 'rdfuzz'   linewidth 4 linetype 3, \\
	 
	  
#plot  '/tmp/output-yyy-cgc-more/plot_data' using 1:7 with lines title 'aflfast-cgc'   linewidth 4 linetype 5  ,\\
      #'/tmp/output-yyy-cgc/plot_data'  using 1:7 with lines title 'aflyyy-cgc'    linewidth 4 linetype 6

######end the high_freq.png


_EOF_

) | gnuplot 


echo "[+] All done - enjoy your charts!"

exit 0



GNUPLOT=`which gnuplot 2>/dev/null`

if [ "$GNUPLOT" = "" ]; then

  echo "[-] Error: can't find 'gnuplot' in your \$PATH." 1>&2
  exit 1

fi

rm -r '/home/xiaosatianyu/Desktop/driller/crash发现速度.png' 
echo "[*] Generating plots..."

(

cat <<_EOF_
set terminal png truecolor enhanced size 1000,350 font "Times New Roman,16" #butt 

set output '/home/xiaosatianyu/Desktop/driller/crash发现速度.png' 


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
set key inside  bottom Right font "DejaVu Sans,18" 

#设置label
#set label "ddd" at 0.5,0.5


#设置题目
#set title "the number of unique crashes"



plot './plot_data_shelfish' using 1:8 with lines title 'AFL-shelfish'   linewidth 2, \\
     './plot_data_yyy' using 1:8 with lines title 'AFL-yyy'  linewidth 8 #linetype 2

_EOF_

) | gnuplot 




echo "[+] All done - enjoy your charts!"

exit 0

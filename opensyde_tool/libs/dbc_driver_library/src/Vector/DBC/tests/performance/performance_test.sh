#!/bin/sh

# set cpus to max
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

ID="1"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "lookup time per message"
set xlabel "number of messages"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

ID="2"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "time to loop over all signals within a message"
set xlabel "number of signals"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

ID="3lu"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "time to decode a signal (little endian, unsigned)"
set xlabel "bit size of signal"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

ID="3ls"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "time to decode a signal (little endian, signed)"
set xlabel "bit size of signal"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

ID="3bu"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "time to decode a signal (big endian, unsigned)"
set xlabel "bit size of signal"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

ID="3bs"
echo ${ID}
./performance_test ${ID} > table_${ID}.csv
gnuplot << END
set title "time to decode a signal (big endian, signed)"
set xlabel "bit size of signal"
set ylabel "lookup time (ns)"
set yrange [0:1000]
set terminal pdf
set output "table_${ID}.pdf"
plot 'table_${ID}.csv' using 1:2
END

echo "Generating report"
pdftk table_*.pdf cat output - > performance_measurement.pdf

# set cpus to powersave
echo powersave | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# print cpu info
egrep "processor|model name" /proc/cpuinfo

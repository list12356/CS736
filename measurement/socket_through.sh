
NUM_ITERATIONS=100
SLEEP_TIME=0.25

# SIZE_LIST=( 64 512 1024 4096 16384 65536 524288 1048576 2048000 4096000)
SIZE_LIST=( 16 256 )
# SIZE_LIST=(524288 1048576 2048000 4096000 8192000 16384000)
for SIZE in "${SIZE_LIST[@]}"
do 
    echo $SIZE
    rm -f tcp_through_$SIZE.txt
    for((c=1; c<=$NUM_ITERATIONS; c++)) 
    do
        ./server $SIZE 16384 & ./client $SIZE 16384 >> tcp_through_$SIZE.txt
        sleep $SLEEP_TIME
    done
done

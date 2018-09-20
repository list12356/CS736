NUM_ITERATIONS=100
SLEEP_TIME=0.25

SIZE_LIST=( 4 16 64 512 1024 4096 16384 65536 524288 )
for SIZE in "${SIZE_LIST[@]}"
do 
    echo $SIZE
    for((c=1; c<=$NUM_ITERATIONS; c++)) 
    do
        ./server & ./client $SIZE >> tcp_$SIZE.txt
        sleep $SLEEP_TIME
    done
done
# ./server & ./client 16 >> tcp_16.txt
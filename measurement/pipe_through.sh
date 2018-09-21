
NUM_ITERATIONS=100
SLEEP_TIME=0.25

# SIZE_LIST=( 16 64 256 512 1024)
SIZE_LIST=( 1024)
# SIZE_LIST=( 4096 16384 65536 524288 1048576 2048000 4096000)
for SIZE in "${SIZE_LIST[@]}"
do 
    echo $SIZE
    rm -f pipe_through_$SIZE.txt
    for((c=1; c<=$NUM_ITERATIONS; c++)) 
    do
        ./pipe $SIZE 134217728 >> pipe_through_$SIZE.txt
        sleep $SLEEP_TIME
    done
done

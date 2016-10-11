#!/bin/bash 
DATADIR="../data"
OUTPUTDIR="../output"

function get_data_nonthreaded {
    IMAGE=$1
    EXECTYPE=$2
    DATAFILE="$OUTPUTDIR/$IMAGE/$IMAGE_$EXECTYPE-data.txt"
    IMAGEPREFIX="$OUTPUTDIR/$IMAGE/$IMAGE_$EXECTYPE"

    if [ ! -d "$OUTPUTDIR/$IMAGE" ]; then
        mkdir $OUTPUTDIR/$IMAGE
    fi

    echo "Non-threaded\n" >> $DATAFILE
    for i in {1..10}
    do
        echo "Non-threaded\n" >> $DATAFILE
        ../src/readWrite-bmp $DATADIR/$IMAGE.bmp $IMAGEPREFIX-$i.bmp 100 >> $DATAFILE
    done
}

function get_data_threaded {
    IMAGE=$1
    EXECTYPE=$2
    DATAFILE="$OUTPUTDIR/$IMAGE/$IMAGE_$EXECTYPE-data.txt"
    IMAGEPREFIX="$OUTPUTDIR/$IMAGE/$IMAGE_$EXECTYPE"

    if [ ! -d "$OUTPUTDIR/$IMAGE" ]; then
        mkdir $OUTPUTDIR/$IMAGE
    fi

    echo "Non-threaded\n" >> $DATAFILE
    for i in {1..10}
    do
        echo "Non-threaded\n" >> $DATAFILE
        ../src/readWrite-bmp-threaded $DATADIR/$IMAGE.bmp $IMAGEPREFIX-$i.bmp 100 $EXECTYPE >> $DATAFILE
    done
}

echo "Getting data for image yeast"
echo "Non-threaded"
get_data_nonthreaded yeast nonthreaded
echo "2 threads"
get_data_threaded yeast 2
echo "4 threads"
get_data_threaded yeast 4
echo "6 threads"
get_data_threaded yeast 6
echo "8 threads"
get_data_threaded yeast 8
echo "12 threads"
get_data_threaded yeast 12
echo "24 threads"
get_data_threaded yeast 24

echo "\nGetting data for image chryseobacterium"
echo "Non-threaded"
get_data_nonthreaded chryseobacterium nonthreaded
echo "2 threads"
get_data_threaded chryseobacterium 2
echo "4 threads"
get_data_threaded chryseobacterium 4
echo "6 threads"
get_data_threaded chryseobacterium 6
echo "8 threads"
get_data_threaded chryseobacterium 8
echo "12 threads"
get_data_threaded chryseobacterium 12
echo "24 threads"
get_data_threaded chryseobacterium 24
echo "Finished!"

exit

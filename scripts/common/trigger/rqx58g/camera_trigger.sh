#!/bin/bash -e
help()
{
    echo "Run GPIO triggering on all four deserializers on ROScube X." 
    echo
    echo "Syntax: scriptTemplate [-r|h]"
    echo "Options:"
    echo "r     Set the frame rate. Default is 10Hz. Maximum is 25Hz, minimum is 1Hz"
    echo "h     Print this Help."
    echo
}
cleanup()
{
    echo "Cleaning up before exit..."    
    echo 408 > /sys/class/gpio/unexport
    echo 350 > /sys/class/gpio/unexport
    echo 446 > /sys/class/gpio/unexport
    echo 445 > /sys/class/gpio/unexport
}
trap cleanup EXIT
fps=10
while getopts ":hr:" option; do
    case $option in
        h) # Display help
            help
            exit;;
        r) # Get frame rate
            fps=$OPTARG
            if [ $fps -gt 25 ]
            then
                echo "Frame rate must be less than 25Hz"
                exit
            fi
            if [ $fps -lt 1 ]
            then
                echo "Frame rate must be more than 1Hz"
                exit
            fi;;
        \?) # Invalid option
            echo "Error: Invalid option"
            exit;;
    esac
done
on_time_milli=10
let "off_time_milli = 1000 / $fps - $on_time_milli"
echo 408 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/gpio408/direction
sleep $(echo "scale=10; 0.5" | bc)
echo 350 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/gpio350/direction
sleep $(echo "scale=10; 0.5" | bc)
echo 446 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/gpio446/direction
sleep $(echo "scale=10; 0.5" | bc)
echo 445 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/gpio445/direction
sleep $(echo "scale=10; 0.5" | bc)

while true
do
    echo 1 > /sys/class/gpio/gpio408/value
    echo 1 > /sys/class/gpio/gpio350/value
    echo 1 > /sys/class/gpio/gpio445/value
    echo 1 > /sys/class/gpio/gpio446/value
    sleep $(echo "scale=10; $on_time_milli/1000" | bc)
    echo 0 > /sys/class/gpio/gpio408/value
    echo 0 > /sys/class/gpio/gpio350/value  
    echo 0 > /sys/class/gpio/gpio446/value
    echo 0 > /sys/class/gpio/gpio445/value
    sleep $(echo "scale=10; $off_time_milli/1000" | bc)
done
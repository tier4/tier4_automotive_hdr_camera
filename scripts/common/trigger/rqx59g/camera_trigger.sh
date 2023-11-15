#!/bin/bash -e

# get pin numders and names from `sudo cat /sys/kernel/debug/gpio`
PIN_NAME1=PP.00
PIN_NAME2=PH.06
PIN_NAME3=PAC.01
PIN_NAME4=PAC.00
PIN_NUM1=440
PIN_NUM2=397
PIN_NUM3=487
PIN_NUM4=486

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
    echo 440 > /sys/class/gpio/unexport
    echo 397 > /sys/class/gpio/unexport
    echo 487 > /sys/class/gpio/unexport
    echo 486 > /sys/class/gpio/unexport
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
echo $PIN_NUM1 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/$PIN_NAME1/direction
sleep $(echo "scale=10; 0.5" | bc)
echo $PIN_NUM2 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/$PIN_NAME2/direction
sleep $(echo "scale=10; 0.5" | bc)
echo $PIN_NUM3 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/$PIN_NAME3/direction
sleep $(echo "scale=10; 0.5" | bc)
echo $PIN_NUM4 > /sys/class/gpio/export
sleep $(echo "scale=10; 0.5" | bc)
echo out > /sys/class/gpio/$PIN_NAME4/direction
sleep $(echo "scale=10; 0.5" | bc)

while true
do
    echo 1 > /sys/class/gpio/$PIN_NAME1/value
    echo 1 > /sys/class/gpio/$PIN_NAME2/value
    echo 1 > /sys/class/gpio/$PIN_NAME3/value
    echo 1 > /sys/class/gpio/$PIN_NAME4/value
    sleep $(echo "scale=10; $on_time_milli/1000" | bc)
    echo 0 > /sys/class/gpio/$PIN_NAME1/value
    echo 0 > /sys/class/gpio/$PIN_NAME2/value  
    echo 0 > /sys/class/gpio/$PIN_NAME3/value
    echo 0 > /sys/class/gpio/$PIN_NAME4/value
    sleep $(echo "scale=10; $off_time_milli/1000" | bc)
done

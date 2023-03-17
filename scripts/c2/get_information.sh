#!/bin/bash

PRE=`i2ctransfer -f -y 30 w10@0x6d 0x33 0x47 0x03 0x00 0x00 0x00 0x42 0x00 0x80 0x3f r6 | cut -d' ' -f 4`

CSUM=`echo $(((0x33+0x51+0x07+$PRE+0xec+0x01)&0xff)) | xargs printf '0x%x'` 

echo "---raw---"
DATA=`i2ctransfer -f -y 30 w11@0x6d 0x33 0x51 0x07 $PRE 0x00 0x00 0xec 0x01 0x00 0x00 $CSUM r508`
echo $DATA | fold -b80 | sed -r "s/([^0x00])/\x1b[38;5;9m\1\x1b[0m/g"

echo "====================================="
echo ""
echo ""
echo ""
echo ""
echo ""
echo ""

echo -n "total gain:" 
echo $DATA | cut -d' ' -f16-31

echo -n "integration:" 
echo $DATA | cut -d' ' -f100-107


echo -n "vts:"
echo $DATA | cut -d' ' -f108-109
echo -n "fps:" 
echo $DATA | cut -d' ' -f112-115
#| fold -b65 | sed -r "s/(0x00)/\x1b[38;5;9m\1\x1b[0m/g"

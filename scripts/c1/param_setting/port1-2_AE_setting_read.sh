#!/bin/bash
sleep_time=0.5

echo "AE current settign read."

show_register_value() {
	slave_addr=$1
	reg_addr=$2
 	let u_byte="$reg_addr>>8"
	let l_byte="$reg_addr&0x0ff"
	u_byte_hex=`printf "0x%02X" $u_byte`
	l_byte_hex=`printf "0x%02X" $l_byte`

	printf "[%s] = " $reg_addr
	i2cset -f -y 30 $slave_addr $u_byte_hex $l_byte_hex i
	i2cget -f -y 30 $slave_addr
}

echo "--- Curent AE mode ---"
show_register_value 0x1a 0xABC0

echo "--- min exposure---"
show_register_value 0x1a 0xAC4C

show_register_value 0x1a 0xAC40
show_register_value 0x1a 0xAC41

echo "--- min gain---"
show_register_value 0x1a 0xAC5A
show_register_value 0x1a 0xAC26
show_register_value 0x1a 0xAE46
show_register_value 0x1a 0xAE94

echo "--- exposure setting ---"
show_register_value 0x1a 0xAC4D

show_register_value 0x1a 0xAC44
show_register_value 0x1a 0xAC45

show_register_value 0x1a 0xAC4E

show_register_value 0x1a 0xAC48
show_register_value 0x1a 0xAC49

echo "--- analog gain setting ---"
show_register_value 0x1a 0xAC5C
show_register_value 0x1a 0xAC5D
show_register_value 0x1a 0xAC5E
show_register_value 0x1a 0xAC5F
show_register_value 0x1a 0xAC60
show_register_value 0x1a 0xAC61

echo -n -e "\n"
sleep $sleep_time

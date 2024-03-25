#!/bin/bash
show_register_value() {

    port_num=$1
	slave_addr=$2
	tmp3=$3
	reg_addr=${tmp3#0[xX]}
	out_fp=$4

	reg_addr_tmp=`echo "ibase=16; $reg_addr" | bc`

 	let u_byte=$((reg_addr_tmp >> 8))
	let l_byte=$((reg_addr_tmp & 0x0ff))
	let l_byte="$reg_addr_tmp&0x0ff"
	u_byte_hex=`printf "0x%02X" $u_byte`
	l_byte_hex=`printf "0x%02X" $l_byte`

	case "$port_num" in
		1 | 2) bus_num=30 ;;
		3 | 4) bus_num=31 ;;
		5 | 6) bus_num=32 ;;
		7 | 8) bus_num=33 ;;
	esac

	i2cset -f -y $bus_num $slave_addr $u_byte_hex $l_byte_hex i
	{
		echo -n "[$reg_addr] = "
		echo `i2cget -f -y $bus_num  $slave_addr`
	} >> $out_fp
}

## Main

if [ $# -ne 1 ]; then
	echo  " **************************************************"
	echo  " ***** Error: Number of Argument should be 1. *****"
	echo  " **************************************************"
	exit 1
fi

port_number=$1

#echo "arg1= $port_number"

case "$port_number" in
 1 | 3 | 5 | 7) ser_slave_addr=0x42 ;;
 2 | 4 | 6 | 8) ser_slave_addr=0x60 ;;
 		     *) 
				echo  " ******************************************************" 
				echo  " ***** Error : Argument #1 should be from 1 to 8. *****" 
				echo  " ******************************************************" 
				exit 1 ;;
esac

output_file=port${port_number}_ser_des_registers_$(date "+%Y%m%d_%H%M%S").txt

echo "--------------------------------------------------------------" 
echo "---- Output_File = $output_file" 
echo "--------------------------------------------------------------" 

echo -e "-------- Port $port_number : Max9295 Registers at $ser_slave_addr --------\n" > $output_file

#echo "command : show_register_value $port_number $ser_slave_addr 0x0002 $output_file"

show_register_value $port_number $ser_slave_addr 0x0002 $output_file
show_register_value $port_number $ser_slave_addr 0x0308 $output_file
show_register_value $port_number $ser_slave_addr 0x0311 $output_file
show_register_value $port_number $ser_slave_addr 0x0314 $output_file
show_register_value $port_number $ser_slave_addr 0x0316 $output_file
show_register_value $port_number $ser_slave_addr 0x0318 $output_file
show_register_value $port_number $ser_slave_addr 0x031A $output_file
show_register_value $port_number $ser_slave_addr 0x0330 $output_file
show_register_value $port_number $ser_slave_addr 0x0331 $output_file
show_register_value $port_number $ser_slave_addr 0x0332 $output_file
show_register_value $port_number $ser_slave_addr 0x0333 $output_file
show_register_value $port_number $ser_slave_addr 0x0334 $output_file
show_register_value $port_number $ser_slave_addr 0x0010 $output_file
show_register_value $port_number $ser_slave_addr 0x0000 $output_file
show_register_value $port_number $ser_slave_addr 0x0044 $output_file
show_register_value $port_number $ser_slave_addr 0x0045 $output_file
show_register_value $port_number $ser_slave_addr 0x006A $output_file
show_register_value $port_number $ser_slave_addr 0x006B $output_file
show_register_value $port_number $ser_slave_addr 0x0073 $output_file
show_register_value $port_number $ser_slave_addr 0x007B $output_file
show_register_value $port_number $ser_slave_addr 0x0083 $output_file
show_register_value $port_number $ser_slave_addr 0x008B $output_file
show_register_value $port_number $ser_slave_addr 0x0093 $output_file
show_register_value $port_number $ser_slave_addr 0x009B $output_file
show_register_value $port_number $ser_slave_addr 0x00A3 $output_file
show_register_value $port_number $ser_slave_addr 0x00AB $output_file
show_register_value $port_number $ser_slave_addr 0x02BE $output_file
show_register_value $port_number $ser_slave_addr 0x02BF $output_file
show_register_value $port_number $ser_slave_addr 0x03F1 $output_file
show_register_value $port_number $ser_slave_addr 0x02CA $output_file
show_register_value $port_number $ser_slave_addr 0x02CD $output_file
show_register_value $port_number $ser_slave_addr 0x02D6 $output_file

echo -n -e "\n" >> $output_file
#echo "----------------------------------------------" >> $output_file

deser_slave_addr=0x48

echo -e "-------- Port $port_number: Max9296 Registers at $deser_slave_addr -------\n" >> $output_file

show_register_value $port_number $deser_slave_addr 0x0005 $output_file
show_register_value $port_number $deser_slave_addr 0x0010 $output_file

show_register_value $port_number $deser_slave_addr 0x0050 $output_file
show_register_value $port_number $deser_slave_addr 0x0051 $output_file
show_register_value $port_number $deser_slave_addr 0x0052 $output_file
show_register_value $port_number $deser_slave_addr 0x0053 $output_file

show_register_value $port_number $deser_slave_addr 0x02B0 $output_file
show_register_value $port_number $deser_slave_addr 0x02B1 $output_file

show_register_value $port_number $deser_slave_addr 0x0320 $output_file

show_register_value $port_number $deser_slave_addr 0x0330 $output_file
show_register_value $port_number $deser_slave_addr 0x0331 $output_file
show_register_value $port_number $deser_slave_addr 0x0332 $output_file
show_register_value $port_number $deser_slave_addr 0x0333 $output_file
show_register_value $port_number $deser_slave_addr 0x0334 $output_file


show_register_value $port_number $deser_slave_addr 0x040B $output_file
show_register_value $port_number $deser_slave_addr 0x040D $output_file
show_register_value $port_number $deser_slave_addr 0x040E $output_file
show_register_value $port_number $deser_slave_addr 0x040F $output_file

show_register_value $port_number $deser_slave_addr 0x0410 $output_file
show_register_value $port_number $deser_slave_addr 0x0411 $output_file
show_register_value $port_number $deser_slave_addr 0x0412 $output_file

show_register_value $port_number $deser_slave_addr 0x042D $output_file

show_register_value $port_number $deser_slave_addr 0x044A $output_file
show_register_value $port_number $deser_slave_addr 0x044B $output_file
show_register_value $port_number $deser_slave_addr 0x044D $output_file
show_register_value $port_number $deser_slave_addr 0x044E $output_file
show_register_value $port_number $deser_slave_addr 0x044F $output_file

show_register_value $port_number $deser_slave_addr 0x0450 $output_file
show_register_value $port_number $deser_slave_addr 0x0451 $output_file
show_register_value $port_number $deser_slave_addr 0x0452 $output_file

show_register_value $port_number $deser_slave_addr 0x046D $output_file

show_register_value $port_number $deser_slave_addr 0x048A $output_file
show_register_value $port_number $deser_slave_addr 0x04CA $output_file

echo -n -e "\n" >> $output_file
echo "----------------------------------------------" >> $output_file

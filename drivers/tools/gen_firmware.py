#!/usr/bin/env python3

import sys
import os
import shutil
import datetime

from sys import byteorder

args = sys.argv

num = len(args)

if num != 2:
    print("***********************************************************************************")
    print("***   Usage :                                                                    ***")
    print("***           gen_firmware.py  C_SOURCE_FILE                                     ***")
    print("***                                                                              ***")
    print("***           C_SOURCE_FILE : Original C source code file                        ***")
    print("***                                                                              ***")
    print("***   Function :                                                                 ***")
    print("***      1. Generating the firmware binary file for ISX021 Sensor.               ***")
    print("***      2. Converting C SOURCE FILE to use the firmware binary file.            ***")
    print("***      3. Generating the sed script file to revert converted C source file.    ***")
    print("***                                                                              ***")
    print("************************************************************************************")
    sys.exit(1)

tm_delta         = datetime.timedelta(hours=9)
JST              = datetime.timezone(tm_delta, 'JST')
now              = datetime.datetime.now(JST)


str_current_time = now.strftime('%Y-%m-%d_%H_%M_%S')

source_file              = args[1]

firmware_bin_file        = "tier4-isx021.bin_" + str_current_time

sed_revert_reg_addr_file = "revert-reg-addr.sed_" + str_current_time

source_file_without_ext  = os.path.splitext(os.path.basename(source_file))[0]

temp_source_file         = source_file_without_ext + "_tmp.c"

backup_org_source_file   = source_file_without_ext + ".c_org_backup_" + str_current_time

converted_source_file    = source_file_without_ext + ".c_out_" + str_current_time

print("----------------------------------------------------------------------------------------")
print("--- Input  [ Source file                      ] : ", source_file)
print("--- Output [ Backup file of Source file       ] : ", backup_org_source_file)
print("--- Output [ Converted  Source file           ] : ", converted_source_file)
print("--- Output [ Sed revert register address file ] : ", sed_revert_reg_addr_file)
print("--- Output [ Firmware binary file             ] : ", firmware_bin_file)
print("----------------------------------------------------------------------------------------")


shutil.copy2(source_file, backup_org_source_file)

length = 2
count = 0

tier4_isx021_reg_0_addr = "TIER4_ISX021_REG_0"

reg_addr_list = []

with open(source_file,"r") as ft1:
    whole_text = ft1.read()
    if tier4_isx021_reg_0_addr in whole_text:
        print("--------------------------------------------------------------------")
        print("-- Error !! : The source file you specified is already converted ! -")
        print("--------------------------------------------------------------------")
        exit(1)

    lines = whole_text.split("\n")

with  open(firmware_bin_file,'wb') as fb, open(temp_source_file,'w') as ftw, open(sed_revert_reg_addr_file, 'w') as ftc:
    for line in lines:
        word_list = line.split()
        if len(word_list) > 2:
            if '#define' in word_list[0]:
                if '_ADDR' in word_list[1]:
                    n = int(word_list[2],16)
                    addr16 = n.to_bytes(length,byteorder = "little")
                    fb.write(addr16)
                    reg_addr_list.append(word_list[1])
                    w_line = "#define TIER4_ISX021_REG_" + str(count) + "_ADDR    " + str(count)
                    revert_define_reg_addr_line = "s/" + w_line + "/#define " + word_list[1] + "  " + word_list[2]+"/g"
                    revert_reg_addr_line = "s/TIER4_ISX021_REG_" + str(count) + "_ADDR/" + word_list[1]+"/g"
                    ftc.write(revert_define_reg_addr_line + "\n")
                    ftc.write(revert_reg_addr_line + "\n")
                    count += 1
                else:
                    w_line = line
            else:
                w_line = line
        else:
            w_line = line

        ftw.write(w_line+"\n")

with open(temp_source_file,'r') as ftw2, open(converted_source_file,'w') as ftw3:

    output_text = ftw2.read()

    rlines = output_text.split("\n")

    w_rline = ["none"] * 200

    for rline in rlines:
        count = 0
        w_rline[count] = rline

        for old_word in reg_addr_list:
            new_word = 'TIER4_ISX021_REG_' + str(count) + '_ADDR'
            w_rline[count+1] = w_rline[count].replace(old_word, new_word)
            count += 1
        ftw3.write(w_rline[count] + "\n")

os.remove(temp_source_file)


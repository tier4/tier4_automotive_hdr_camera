#!/bin/bash

function Usage
{
    echo "*********************************************************************************"
    echo "***   Usage:                                                                  ***"
    echo "***            revert_source.sh C_SOURCE_CODE_FILE                            ***"
    echo "***                                                                           ***"
    echo "***     SOURCE_CODE_FILE :                                                    ***"
    echo "***                                                                           ***"
    echo "***            tier4-isx021.c                                                 ***"
    echo "***            tier4-isx021.c_out_20YY-MN-DD-HH_MM_SS                         ***"
    echo "***                                                                           ***"
    echo "***            YY:Year  MN:Month DD:Day MM:Minutes SS:Seconds                 ***"
    echo "***                                                                           ***"
    echo "***    If "tier4-isx021.c" is specified,                                      ***"
    echo "***       The file "revert-reg-addr.sed" is required.                         ***"
    echo "***                                                                           ***"
    echo "***    if "tier4-isx021.c_out_20YY-MN-DD_HH_MM_SS" is specified,              ***"
    echo "***       The file "revert-reg-addr.sed_20YY-MN-DD_HH_MM_SS " is required.    ***"
    echo "***                                                                           ***"
    echo "***   Function:                                                               ***"
    echo "***       Generating the Orignal source code file from C_SOURCE_CODE_FILE.    ***"
    echo "***                                                                           ***"
    echo "***                                                                           ***"
    echo "*********************************************************************************"
}

if [ $# != 1 ]; then
    echo "====================================================================*"
    echo "===   Error !! Total number of argument should be 1.              ==="
    echo "====================================================================="
    Usage
    exit -1
fi

source_file=$1

if [ -n $1 ]; then

    file_base_name=${source_file%.*}
    extension=${source_file##*.}
    ext_date_time="c_out_*"

    case $extension in

        "C" | "c" )
            time_stamp=""
            rev_file_ext=".c_rev"
            rev_reg_addr_sed_file="revert-reg-addr.sed"
            ;;
        $ext_date_time )
            time_stamp=${extension##c_out_}
            rev_file_ext=".c_rev_"${time_stamp}
            rev_reg_addr_sed_file="revert-reg-addr.sed_"${time_stamp}
            ;;
        * )
            echo " SOURCE CODE FILE is wrong"
            Usage
            exit -1
            ;;
    esac

  output_rev_file=${file_base_name}${rev_file_ext}

  echo "--------------------------------------------------------------"
  echo "---  Input  [ Source File     ] : $source_file"
  echo "---  Input  [ Sed Script File ] : $rev_reg_addr_sed_file"
  echo "---  Output [ Reverted File   ] : $output_rev_file"
  echo "--------------------------------------------------------------"

  sed -f ${rev_reg_addr_sed_file} ${source_file} > ${output_rev_file}

fi


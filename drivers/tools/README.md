~~~~~~~~~~~~~~~~
gen_firmware.py ( python 3 is required)
----------------

( Python 3 is required.)

This generates the followings from original C source code file written with the actual register address and name.

1.  Converted C source code file for the register address numbers to be translated with  firmware binary file.
2.  firmware binary file containing the register address.
3.  sed script file reverting the converted C source code file to the original C source code file.

Refer to he Usage section in gen_firmware.py in detail.

~~~~~~~~~~~~~~~~
revert_source.sh
~~~~~~~~~~~~~~~~

This generates the original C source code file from the C source code file genereted by gen_firmware.py.

Refer to he Usage section in revert_source.sh in detail.

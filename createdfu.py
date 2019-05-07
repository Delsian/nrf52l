#!/usr/bin/python
#from firebase import firebase
import os, subprocess
    
KEY_PEM="../config/private.pem"

if __name__ == '__main__':
    tag = subprocess.check_output(['git', 'describe', '--tags', '--abbrev=0']).rstrip("\r\n")
    print tag
    with open("version.txt", 'w') as fver:
        fver.write(tag)
        fver.close()
    os.system("nrfutil pkg generate --hw-version 52 --application Debug/nrf52l.hex --key-file %s --sd-req 0x9D --application-version-string %s dfu.zip"%(KEY_PEM,tag))    
    #

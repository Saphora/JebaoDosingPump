@echo off
scp *.cpp pi@192.168.2.2:/home/pi/DosingPump/
scp *.h pi@192.168.2.2:/home/pi/DosingPump/
scp Makefile pi@192.168.2.2:/home/pi/DosingPump/Makefile
ssh pi@192.168.2.2 "cd /home/pi/DosingPump ; make" 
if errorlevel 1 goto somethingbad
    echo Build Success!
exit
    :somethingbad
    echo There are build errors, please fix before procceed.
exit 0
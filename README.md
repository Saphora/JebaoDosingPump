# JebaoDosingPump
This driver is used to driving Jebao dosing pumps. Please remove the control board from the Jebao DP4 Master and replace it with a Raspberry PI or whatever.
In my case, i have connected the storage clock (ST) to GPIO pin 29 and shift clock (CH) to pin 28. The Serial Data line is connected to pin 27.

I have completly removed the controller board and replaced it with the Raspberry PI, i have mounted the Pi to back of the casing and connected +5V and GND from the Raspberry PI 
to the power supply of the pump.

#How to install?

sudo apt update

sudo apt install wiringPi

make

./dosingpump


This project is just for hobby and i am not a C++ developer! this project is for learning purposes and i just needed a dosing pump for my garden... 

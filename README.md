[![Build status](https://ci.appveyor.com/api/projects/status/github/NXPmicro/bcu?svg=true)]

# to build and run BCU in Windows:  

first we need to have FTDI D2XX library installed:  
  
1. go to https://www.ftdichip.com/Drivers/D2XX.htm  
2. click "setup executable" and download the zip file  
3. unzip the file and install the library  
  
then we can download and build bcu:  
  
$ git clone https://github.com/NXPmicro/bcu.git  
  
open Board_Control_Utilities.sln with windows visual studio  
  
ctr-shift+B to build the solution  
  
ctr-f5 to run  
  
  
____________________________________________________________________  
  # to build and run BCU in Linux:  
  
first we need to obtain libftdi1-dev library:  
$ sudo apt-get update  
$ sudo apt-get install libftdi1-dev  
  
then we can obtain and build bcu:  
  
$ git clone https://github.com/NXPmicro/bcu.git 
  
$ cd bcu  
  
$ cmake .  
  
$ make  
  
$ sudo ./bcu command  
  
linux user: please remember to run bcu with sudo to avoid permission issue  
  
_____________________________________________________________________  
  
  
# list of available commands & their functions:  
  
|commands                  |descriptions|  
|----|                     ----------------|
reset                       |reset the board  
monitor                    |switch into a tui windows that monitors voltage, current and power consumption inside the board  
lsgpio                    |show a list of available gpio pin  
set_gpio [GPIO_NAME] [1/0]        |set a GPIO_NAME pin to be high(1) or low(0). all GPIO_NAMEs are shown in lsgpio command  
lsftdi                    |show the list of connected boards and the their location IDs  
lsboard                    |list all supported board models  
set_boot_mode [BOOTMODE]        |set the boot mode as BOOTMODE  
help                    |show list of available commands and their functions, as well as options
version                 |show version number
  
### typical usage of bcu:  
  
$ bcu command [-options]   
  
#### some example:  
  
1.set sd_wp gpio pin as high:  
$ bcu set_gpio sd_wp 1  
   
2.reset the i.MX8QMEVK board after 1 second:  
$ bcu reset -board=iMX8QMEVK -delay=1000  
  
3.hold sd_pwr pin low for 3 second:  
$ bcu set_gpio sd_pwr 0 -hold=3000  
  
4.set emmc as the booting device:  
$ bcu set_boot_mode emmc  
  
___________________________________________________________________________________________________ 
  
## general options for all commands:
|options|descriptions|
|----------------|-------|
|[-board=MODEL]|set the board model, list of avaliable models is shown in lsboards command  
| [-id=ID]    |select the target board with location id(when there are multiple boards), the location id can be viewed by using command lsftdi  
  
## options for specfic command:  
### reset:  
|options|descriptions|
|----------------|-------|
 [-delay=DURATION]    |reset after DURATION ms  
 [BOOTMODE]        |setting BOOTMODE as the booting method  
  
### monitor:  
|options|descriptions|
|----------------|-------|
[-dump]            |dump data into a file, which can be imported to excel  
  
### set_gpio: 
|options||
|----------------|-------|
 [-delay=DURATION]    |set after DURATION ms  
 [-hold=DURATION]    |hold the output state for DURATION ms  
 [GPIO_NAME]        |specify which gpio pin should be set, list of available gpio can be found using list gpio command  
 [0]            |set the pin output low  
 [1]            |set the pin output high  
 [-path=PATH]        |for testing purpose only: set the gpio pin specified by path  
  
### set_boot_mode
|options|descriptions|
|----------------|-------|  
[BOOTMODE]        |booting from emmc device  
_______________________________________________________________________________________________________

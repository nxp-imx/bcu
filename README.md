# bcu (Board Control Utilities)

[![Build status](https://ci.appveyor.com/api/projects/status/github/NXPmicro/bcu?svg=true)](https://ci.appveyor.com/project/nxpfrankli/bcu)
![GitHub](https://img.shields.io/github/license/NXPmicro/bcu.svg)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/18825/badge.svg)](https://scan.coverity.com/projects/nxpmicro-bcu)

# to build and run BCU in Windows:  

first we need to have FTDI D2XX library installed:  

1. go to https://www.ftdichip.com/Drivers/D2XX.htm  
2. click "setup executable" and download the zip file  
3. unzip the file and install the library
4. **Make sure no application open remote control port. Generally is first two uart ports. otherwise you will get fail to open ftdi device.** 

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

$ sudo ./bcu command [-options]  

linux user: please remember to run bcu with sudo to avoid permission issue  

_____________________________________________________________________


# list of available commands & their functions:  

| commands                                       | descriptions                                                 |
| ---------------------------------------------- | ------------------------------------------------------------ |
| reset  [BOOTMODE_NAME] [-board=] [-id=]        | reset the board                                              |
| init   [BOOTMODE_NAME] [-board=] [-id=]        | enable the remote control with a boot mode                   |
| deinit [BOOTMODE_NAME] [-board=] [-id=]        | disable the remote control                                   |
| monitor                                        | switch into a tui windows that monitors voltage, current and power consumption inside the board |
| set_gpio [GPIO_NAME] [1/0] [-board=] [-id=]    | set pin GPIO_NAME to be high(1) or low(0)                    |
| set_boot_mode [BOOTMODE_NAME] [-board=] [-id=] | set BOOTMODE_NAME as boot mode                               |
| lsftdi                                         | show the list of connected boards and the their location IDs |
| lsboard                                        | list all supported board models                              |
| lsbootmode [-board=]                           | show a list of available boot mode of a board                |
| lsgpio [-board=]                               | show a list of available gpio pin of a board                 |
| help                                           | show list of available commands and their functions, as well as options |
| version                                        | show version number                                          |

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
| options        | descriptions                                                 |
| -------------- | ------------------------------------------------------------ |
| [-board=MODEL] | set the board model, list of avaliable models is shown in lsboards command |
| [-id=ID]       | select the target board with location id(when there are multiple boards), the location id can be viewed by using command lsftdi |

## options for specfic command:  
### reset:  
| options           | descriptions                           |
| ----------------- | -------------------------------------- |
| [-delay=DURATION] | reset after DURATION ms                |
| [BOOTMODE]        | setting BOOTMODE as the booting method |

### monitor:  
| options | descriptions                                          |
| ------- | ----------------------------------------------------- |
| [-dump] | dump data into a file, which can be imported to excel |

### set_gpio: 
| options           | descriptions                                                 |
| ----------------- | ------------------------------------------------------------ |
| [-delay=DURATION] | set after DURATION ms                                        |
| [-hold=DURATION]  | hold the output state for DURATION ms                        |
| [GPIO_NAME]       | specify which gpio pin should be set, list of available gpio can be found using list gpio command |
| [0]               | set the pin output low                                       |
| [1]               | set the pin output high                                      |
| [-path=PATH]      | for testing purpose only: set the gpio pin specified by path |

### set_boot_mode
| options    | descriptions             |
| ---------- | ------------------------ |
| [BOOTMODE] | booting from emmc device |

_______________________________________________________________________________________________________

# **Board support**

## i.MX8MP-EVK

#### initialization usage:

$ sudo ./bcu init [boot mode] -board=imx8mpevk  

#### reset usage:

$ sudo ./bcu reset [boot mode] -board=imx8mpevk  
**RESET function does NOT dependent on initialization. Can be use alone.**  

#### multi-board usage:

If there are more than one board plug-in your host, you can use [-id=] to operate the specific board.  

1. get the id number  
$ sudo ./bcu lsftdi  
We can get the board id here:  
board[0] location_id=**1-1**  

2. if we want to reset id=1-1 board  
$ sudo ./bcu reset [sd] -board=imx8mpevk -id=1-1  

#### boot mode select for imx8mpevk:

| SW4[1/2/3/4] | cmd string      | bootmode                          |
| ------------ | --------------- | --------------------------------- |
| 0000         | fuse            | Internal Fuses                    |
| 0001         | usb             | USB Serial Download               |
| 0010         | emmc            | EMMC/USDHC3                       |
| 0011         | sd              | USDHC2                            |
| 0100         | nand_256        | NAND 8-bit single device 256 page |
| 0101         | nand_512        | NAND 8-bit single device 512 page |
| 0110         | qspi_3b_read    | QSPI 3B Read                      |
| 0111         | qspi_hyperflash | QSPI Hyperflash 3.3V              |
| 1000         | ecspi           | ecSPI Boot                        |
| 1110         | infinite_loop   | Infinite Loop Mode                |

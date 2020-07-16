# bcu (Board Control Utilities)

[![Build status](https://ci.appveyor.com/api/projects/status/github/NXPmicro/bcu?svg=true)](https://ci.appveyor.com/project/nxpfrankli/bcu)
![GitHub](https://img.shields.io/github/license/NXPmicro/bcu.svg)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/18825/badge.svg)](https://scan.coverity.com/projects/nxpmicro-bcu)

# to build and run BCU in Windows:  

first we need to have FTDI D2XX library installed:  

1. go to https://www.ftdichip.com/Drivers/D2XX.htm  
2. click "setup executable" and download the zip file  
3. unzip the file and install the library

then we can load and build bcu:  

> git clone https://github.com/NXPmicro/bcu.git  

> git submodule update --init  

open Board_Control_Utilities.sln with windows visual studio  


ctr-shift+B to build the solution  

ctr-f5 to run  

You can download prebuild image from https://github.com/NXPmicro/bcu/releases

**Make sure no application open remote control port. Generally is first two uart ports. otherwise you will get fail to open ftdi device.** 

____________________________________________________________________
  # to build and run BCU in Linux:  

first we need to obtain libftdi1-dev library:  
$ sudo apt-get update  
$ sudo apt-get install libftdi1-dev  

then we can obtain and build bcu:  

$ git clone https://github.com/NXPmicro/bcu.git 

$ cd bcu  

$ git submodule update --init  

$ cmake .  

$ make  

$ sudo ./bcu command [-options]  

linux user: please remember to run bcu with sudo to avoid permission issue  

_____________________________________________________________________


# list of available commands & their functions:  

| commands                                                     | descriptions                                                 |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| reset  [BOOTMODE_NAME] [-board=] [-id=]                      | reset the board                                              |
| init   [BOOTMODE_NAME] [-board=] [-id=]                      | enable the remote control with a boot mode                   |
| deinit [BOOTMODE_NAME] [-board=] [-id=]                      | disable the remote control                                   |
| monitor [-board=] [-id=] [-dump/-dump=] [-nodisplay] [-hz=] [-rms] [-hwfilter] [-unipolar] | switch into a tui windows that monitors voltage, current and power consumption inside the board |
| set_gpio [GPIO_NAME] [1/0] [-board=] [-id=]                  | set pin GPIO_NAME to be high(1) or low(0)                    |
| set_boot_mode [BOOTMODE_NAME] [-board=] [-id=]               | set BOOTMODE_NAME as boot mode                               |
| lsftdi                                                       | show the list of connected boards and the their location IDs |
| lsboard                                                      | list all supported board models                              |
| lsbootmode [-board=]                                         | show a list of available boot mode of a board                |
| lsgpio [-board=]                                             | show a list of available gpio pin of a board                 |
| help                                                         | show list of available commands and their functions, as well as options |
| version                                                      | show version number                                          |

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
| options          | descriptions                                                 |
| ---------------- | ------------------------------------------------------------ |
| [-dump]          | dump data into a file, which can be imported to excel        |
| [-dump=FILENAME] | dump data into a “FILENAME.csv” file, which can be imported to excel. |
| [-nodisplay]     | Will not show data value in real time, just dump data to file in the background. This will help to improve sample speed.<br/>It will auto select -dump option. |
| [-hz=]           | Unit: Hz, support float number.<br/>It can change the display refresh frequency on terminal. Low refresh frequency will help to improve sample speed on Windows.<br/>usage:<br/>•	Windows: > .\bcu.exe monitor [-board=xxx] -hz=”0.5”<br/>•	Linux: $ sudo ./bcu monitor [-board=xxx] -hz=0.5 |
| [-rms]           | Show Root-Mean-Square instead of normal Average of all Current data. |
| [-hwfilter]      | 8x Hardware Filter<br/>Voltage and current values will be read from avg register which contain a rolling average of the eight most recent VBUS/VSENSE results. |
| [-unipolar]      | Change the current measurement range from (-100mA~100mA) to (0mA~100mA). |
| HOT-KEYs         | Please refer to release note.                                |

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

- imx8mpevk: do not have power measurement function

- imx8mpevkpwr: have power measurement function

  **!!!NOTE!!!**

  Please make sure your imx8mp power CPU board has done this rework:

  - R208 from 1.0Ohm to 0.4Ohm

  - R76  from 0.4Ohm to 1.0Ohm

  - R225 from 2.0Ohm to 1.0Ohm

  - R227 from 2.0Ohm to 1.0Ohm

  - R129 from 0.1Ohm to 0.05Ohm

  If your board is not reworked, **please modify rsense1/rsense2 to the old values in config.yaml file by yourself**.

  - vdd_pll_ana_0v8 resense1/resense2 both set to 400(Milliohm)

  - lpd4_vdd2 resense1/resense2 both set to 100

  - vdd_usb_0v8 resense1/resense2 both set to 1000

  - vdd_lvds_1V8 resense1/resense2 both set to 2000

  - vdd_pci_1v8 resense1/resense2 both set to 2000


- imx8mpddr3l: VDB

- imx8mpddr4: VDB

## i.MX8DXL-EVK

- imx8dxlevk
- imx8dxl_ddr3_evk

_______________________________________________________________________________________________________

#### **For more details, please check the BCU release note in the latest release.**


# bcu (Board Control Utilities)

[![Build status](https://ci.appveyor.com/api/projects/status/github/NXPmicro/bcu?svg=true)](https://ci.appveyor.com/project/nxpfrankli/bcu)
![GitHub](https://img.shields.io/github/license/NXPmicro/bcu.svg)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/18825/badge.svg)](https://scan.coverity.com/projects/nxpmicro-bcu)

# **Running environment**

 - Windows 10, 64bit
    - Need install FTDI Driver for Windows.
    - Early Windows version(below 1.2.0) need install vs2017 redistribute package

 - Ubuntu 16.04 or above, 64bit
    - Need install libftdi, libyaml-dev and openssl
      - $ sudo apt-get install libftdi1-2 libyaml-dev openssl
    - If you do not want to run BCU as root in Linux, please follow the steps below to set the rules.
      - $ cd /etc/udev/rules.d/
      - $ sudo vim xx-ftdi.rules
      - Add the line to this file: 
      - SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6011", GROUP="dialout", MODE="0660"
      - Save the file
      - Re-plug the debug wire

 - Mac OS 11.x Big Sur or above, x64 (ARM is not support for now)
    - Need install Homebrew first
      - $ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    - Then install other libs by Homebrew
      - $ brew install libftdi libyaml
 - Mac OS 11.x Big Sur or above, arm64
    - Apple Silicon can run x64 binary, but all libs should be installed to the path which is same as x64.
      - $ arch -x86_64 $SHELL
      - $ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    - Install other libs by Homebrew to default path(/usr/local/Cellar)
      - $ brew install libftdi libyaml

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

## Methods to get support:

 - From NXP internal issue: Create Jira ticket in MLK project, component TOOLS
 
 - From Customers: Create new Issue in this repo https://github.com/NXPmicro/bcu


_______________________________________________________________________________________________________

## **For more details about BCU, please check the BCU release note(BCU.pdf) in the latest release.**


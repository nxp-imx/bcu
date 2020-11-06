# bcu (Board Control Utilities)

[![Build status](https://ci.appveyor.com/api/projects/status/github/NXPmicro/bcu?svg=true)](https://ci.appveyor.com/project/nxpfrankli/bcu)
![GitHub](https://img.shields.io/github/license/NXPmicro/bcu.svg)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/18825/badge.svg)](https://scan.coverity.com/projects/nxpmicro-bcu)

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


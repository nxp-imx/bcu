/*
* Copyright 2019 NXP.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of the NXP Semiconductor nor the names of its
* contributors may be used to endorse or promote products derived from this
* software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS //in order to use strcpy without error
#include <windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include "board.h"

//x select channel of pca9548 chip, y select which sensor it is measuring 
//rsense use mOhm
#define IMX8XXL_POWER_PATH(channel, sensor, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/pca9548{channel="#channel";addr=0x70}/pac1934{group="#channel";sensor="#sensor";addr=0x10;rsense1="#rsense1";rsense2="#rsense2"}"

#define IMX8XXL_EXP_PATH(port,bitmask) "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/pca9548{channel=0;addr=0x70}/pca6416a{addr=0x20;port="#port";pin_bitmask="#bitmask";opendrain=1;}"
struct mapping imx8xxl[] = {
	{"on_board_5v0",power,IMX8XXL_POWER_PATH(1,1,10,10), 0x00},
	{"vdd_usb_3v3",power,IMX8XXL_POWER_PATH(1,2,1650,1650), 0x00},
	{"3v3_io",power,IMX8XXL_POWER_PATH(1,3, 1650, 1650), 0x00},
	{"3v3_enet",power,IMX8XXL_POWER_PATH(1,4, 3300,3300), 0x00},
	{"3v3_pmic_in",power,IMX8XXL_POWER_PATH(2,1,10,10), 0x00},
	{"on_board_3v3", power, IMX8XXL_POWER_PATH(2,2, 10, 10), 0x00},
	{"vdd_snvs_4p2",power,IMX8XXL_POWER_PATH(2,3,2000,2000), 0x00},
	{"vdd_main",power,IMX8XXL_POWER_PATH(2,4, 60, 10000), 0x00},
	{"vdd_memc",power,IMX8XXL_POWER_PATH(3,1, 50, 10000), 0x00},
	{"vdd_ddr_vddq",power,IMX8XXL_POWER_PATH(3,2, 120, 10000), 0x00},
	{"ddr_vdd2",power,IMX8XXL_POWER_PATH(3,3, 10, 10), 0x00},
	{"vdd_enet0_1p8_3p3",power,IMX8XXL_POWER_PATH(3,4, 3320, 3320), 0x00},
	{"vdd_ana",power,IMX8XXL_POWER_PATH(4,1, 10000, 10000), 0x00},
	{"ddr_vdd1",power,IMX8XXL_POWER_PATH(4,2, 10, 10), 0x00},
	{"1v8_1",power,IMX8XXL_POWER_PATH(4,3, 600, 600), 0x00},
	{"1v8_2",power,IMX8XXL_POWER_PATH(4,4, 1000, 1000), 0x00},
	{"1v8_3",power,IMX8XXL_POWER_PATH(5,1, 1000, 1000), 0x00},
	{"1v8_4",power,IMX8XXL_POWER_PATH(5,2, 680, 680), 0x00},
	{"1v8_5",power,IMX8XXL_POWER_PATH(5,3, 440, 440), 0x00},
	{"on_board_1v8",power,IMX8XXL_POWER_PATH(5,4,10, 10), 0x00},
	{"1v8_6",power,IMX8XXL_POWER_PATH(6,1, 10, 10), 0x00},
	{"ana1",power,IMX8XXL_POWER_PATH(6,2, 500, 500), 0x00},
	{"emmc0",power,IMX8XXL_POWER_PATH(6,3,3000,3000), 0x00},
	{"on_board_12v0",power,IMX8XXL_POWER_PATH(6,4, 10, 10), 0x00},

	{"boot_mode",gpio,IMX8XXL_EXP_PATH(0,0x07), 0x80},
	{"sd_pwr",gpio, IMX8XXL_EXP_PATH(0,0x08), 0x51},
	{"sd_wp",gpio, IMX8XXL_EXP_PATH(0,0x10), 0x61},
	{"sd_cd",gpio, IMX8XXL_EXP_PATH(0,0x20), 0x71},
	{"exp5_p06", gpio, IMX8XXL_EXP_PATH(0, 0x40), 0x00},
	{"exp5_p07", gpio, IMX8XXL_EXP_PATH(0, 0x80), 0x00},
	{"SR_vdd_main",gpio, IMX8XXL_EXP_PATH(1,0x01), 0x00},
	{"SR_vdd_memc",gpio, IMX8XXL_EXP_PATH(1,0x02), 0x00},
	{"SR_vdd_ddr_vddq",gpio, IMX8XXL_EXP_PATH(1,0x04), 0x00},
	{"SR_vdd_ana",gpio, IMX8XXL_EXP_PATH(1,0x08), 0x00},
	{"exp5_p14", gpio, IMX8XXL_EXP_PATH(1,0x10), 0x00},
	{"exp5_p15", gpio, IMX8XXL_EXP_PATH(1,0x20), 0x00},
	{"testmod_sel",gpio, IMX8XXL_EXP_PATH(,0x40), 0x00},
	{"bootmode_sel",gpio, IMX8XXL_EXP_PATH(1,0x80), 0x90},

	{"reset",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x11},
	{"jtag_sel",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x01},
	{"onoff", gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x21},
	{"remote_en",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x40},
	{"ft_reset", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x31},

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/at24cxx{addr=0x57;}", 0x00},
	{NULL, 0, NULL, 0}//null terminated
};

struct mapping imx8dxl_ddr3[] = {
	{"on_board_12v0",power,IMX8XXL_POWER_PATH(1,1,10,10), 0x00},
	{"3v3_MICRO",power,IMX8XXL_POWER_PATH(1,2,10,10), 0x00},
	{"vdd_main",power,IMX8XXL_POWER_PATH(1,3, 50, 50), 0x00},
	{"vdd_memc",power,IMX8XXL_POWER_PATH(1,4, 50,50), 0x00},

	{"vdd_ddr_vddq",power,IMX8XXL_POWER_PATH(2,1,120,120), 0x00},
	{"vdd_ddr", power, IMX8XXL_POWER_PATH(2,2, 10, 10), 0x00},
	{"vdd_vtt",power,IMX8XXL_POWER_PATH(2,3,10,10), 0x00},
	{"vdd_ddr_pll",power,IMX8XXL_POWER_PATH(2,4, 1000, 1000), 0x00},

	{"boot_mode",gpio,IMX8XXL_EXP_PATH(0,0x07), 0x80},
	{"sd_pwr",gpio, IMX8XXL_EXP_PATH(0,0x08), 0x51},
	{"sd_wp",gpio, IMX8XXL_EXP_PATH(0,0x10), 0x61},
	{"sd_cd",gpio, IMX8XXL_EXP_PATH(0,0x20), 0x71},
	{"exp5_p06", gpio, IMX8XXL_EXP_PATH(0, 0x40), 0x00},
	{"exp5_p07", gpio, IMX8XXL_EXP_PATH(0, 0x80), 0x00},
	{"exp5_p14", gpio, IMX8XXL_EXP_PATH(1,0x10), 0x00},
	{"exp5_p15", gpio, IMX8XXL_EXP_PATH(1,0x20), 0x00},
	{"testmod_sel",gpio, IMX8XXL_EXP_PATH(,0x40), 0x00},
	{"bootmode_sel",gpio, IMX8XXL_EXP_PATH(1,0x80), 0x90},

	{"reset",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x11},
	{"jtag_sel",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x01},
	{"onoff", gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x21},
	{"remote_en",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x40},
	{"ft_reset", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x31},

	// you put all the pin in the imx8xxl board here
	//"sd_wp", gpio, "..."
	{NULL, 0, NULL, 0}//null terminated
};

struct boot_mode imx8xxl_boot_modes[] = {
	{"efuse", 0x00},
	{"usb", 0x01},
	{"emmc", 0x02},
	{"sd",0x03},
	{"nand", 0x04},
	{"m4_infinite_loop",0x05},
	{"spi", 0x06},
	{"dft_burnin_mode",0x07},
	NULL, 0
};

struct board_links imx8xxlevk_board_links[] = {
	"",
	""
};

#define IMX8MPEVK_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX8MPEVK_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX8MPEVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}"
struct mapping imx8mpevkpwr_board[] = {
	{"vdd_arm", power, IMX8MPEVK_POWER_SWITCH_PATH(1, 1, 0x11, 20, 2, 5010), 0x00},
	{"nvcc_dram_1v1", power, IMX8MPEVK_POWER_SWITCH_PATH(1, 3, 0x11, 50, 4, 10050), 0x00},
	{"vsys_5v", power, IMX8MPEVK_POWER_SWITCH_PATH(2, 1, 0x12, 20, 2, 2020), 0x00},
	{"vdd_soc", power, IMX8MPEVK_POWER_SWITCH_PATH(2, 3, 0x12, 10, 4, 2010), 0x00},
	{"lpd4_vddq", power, IMX8MPEVK_POWER_SWITCH_PATH(3, 1, 0x13, 50, 2, 10050), 0x00},
	{"lpd4_vdd2", power, IMX8MPEVK_POWER_PATH(3, 3, 0x13, 50, 50), 0x00},
	{"nvcc_sd2", power, IMX8MPEVK_POWER_PATH(3, 4, 0x13, 1000, 1000), 0x00},
	{"vdd_lvds_1V8", power, IMX8MPEVK_POWER_PATH(4, 1, 0x14, 1000, 1000), 0x00},
	{"vdd_hdmi_1v8", power, IMX8MPEVK_POWER_PATH(4, 2, 0x14, 2000, 2000), 0x00},
	{"nvcc_snvs_1v8", power, IMX8MPEVK_POWER_PATH(4, 3, 0x14, 10000, 509000), 0x00},
	{"vdd_earc_1v8", power, IMX8MPEVK_POWER_PATH(4, 4, 0x14, 2000, 2000), 0x00},
	{"vdd_usb_1v8", power, IMX8MPEVK_POWER_PATH(5, 1, 0x15, 2000, 2000), 0x00},
	{"vdd_pci_1v8", power, IMX8MPEVK_POWER_PATH(5, 2, 0x15, 1000, 1000), 0x00},
	{"vdd_mipi_1v8", power, IMX8MPEVK_POWER_PATH(5, 3, 0x15, 2000, 2000), 0x00},
	{"vdd_pll_ana_1v8", power, IMX8MPEVK_POWER_PATH(5, 4, 0x15, 258, 10250), 0x00},
	{"nvcc_sd1", power, IMX8MPEVK_POWER_PATH(6, 1, 0x16, 100, 100), 0x00},
	{"lpd4_vdd1", power, IMX8MPEVK_POWER_PATH(6, 2, 0x16, 4990, 4990), 0x00},
	{"cpu_vdd_1v8", power, IMX8MPEVK_POWER_PATH(6, 3, 0x16, 100, 100), 0x00},
	{"bb_vdd_1v8", power, IMX8MPEVK_POWER_PATH(6, 4, 0x16, 50, 50), 0x00},
	{"vdd_pll_ana_0v8", power, IMX8MPEVK_POWER_PATH(7, 1, 0x17, 1000, 1000), 0x00},
	{"vdd_pci_0v8", power, IMX8MPEVK_POWER_PATH(7, 2, 0x17, 1000, 1000), 0x00},
	{"vdd_mipi_0v8", power, IMX8MPEVK_POWER_PATH(7, 3, 0x17, 1000, 1000), 0x00},
	{"vdd_hdmi_0v8", power, IMX8MPEVK_POWER_PATH(7, 4, 0x17, 1000, 1000), 0x00},
	{"vdd_usb_3v3", power, IMX8MPEVK_POWER_PATH(8, 1, 0x18, 1000, 1000), 0x00},
	{"vdd_usb_0v8", power, IMX8MPEVK_POWER_PATH(8, 2, 0x18, 400, 400), 0x00},
	{"vdd_sd1_3v3", power, IMX8MPEVK_POWER_PATH(8, 3, 0x18, 250, 250), 0x00},
	{"bb_vdd_3v3", power, IMX8MPEVK_POWER_PATH(8, 4, 0x18, 50, 50), 0x00},

	{"boot_mode", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x30},
	{"SR_vsys_5v", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_nvcc_snvs_1v8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_vdd_soc", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_vdd_arm", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"remote_en", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"SR_vdd_pll_ana_1v8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_nvcc_dram_1v1", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_lpd4_vddq", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"ft_io8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_io9", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_io10", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_io11", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"reset_b",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"reset",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x21},
	{"io_nrst",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x11},
	{"onoff",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},
	{"io_nint",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},

	{"ft_gpio1",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_gpio2",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_gpio3",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_gpio4",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x80}", 0x00},

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/at24cxx{addr=0x50;}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct mapping imx8mpevk_board[] = {
	{"boot_mode", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x30},
	{"SR_vsys_5v", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_nvcc_snvs_1v8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_vdd_soc", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_vdd_arm", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"remote_en", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"SR_vdd_pll_ana_1v8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_nvcc_dram_1v1", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_lpd4_vddq", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"ft_io8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_io9", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_io10", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_io11", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"reset_b",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"reset",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x21},
	{"io_nrst",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x11},
	{"onoff",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},
	{"io_nint",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},

	{"ft_gpio1",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_gpio2",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_gpio3",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_gpio4",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x80}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

#define IMX8MPVBD_POWER_SWITCH_PATH(channel, group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel="#channel";dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX8MPVBD_POWER_PATH(channel, group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel="#channel";dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX8MPVBD_GPIO_EXTENDER_PATH(channel, addr, port, pin_bitmask) "/ft4232h_i2c{channel="#channel";dir_bitmask=0xF0;val_bitmask=0x00}/pcal6524h{addr="#addr";port="#port";pin_bitmask="#pin_bitmask";opendrain=0;}"
struct mapping imx8mpddr3l_board[] = {
	{"vdd_avpll_1p8", power, IMX8MPVBD_POWER_PATH(1, 1, 1, 0x10, 1000, 1000), 0x00},
	{"nvcc_dram_1v35", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 1, 2, 0x10, 50, 3, 1050), 0x00},
	{"vdd_dram_pll_1p8", power, IMX8MPVBD_POWER_PATH(1, 1, 4, 0x10, 1000, 11000), 0x00},
	{"vdd_arm", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 2, 1, 0x11, 20, 2, 2020), 0x00},
	{"vdd_soc", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 2, 4, 0x11, 10, 3, 1010), 0x00},
	{"vdd_pci_1p8", power, IMX8MPVBD_POWER_PATH(1, 3, 1, 0x12, 1000, 11000), 0x00},
	{"vdd_ana_1p8", power, IMX8MPVBD_POWER_PATH(1, 3, 2, 0x12, 1000, 11000), 0x00},
	{"pmic_dcdc_5v", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 3, 3, 0x12, 10, 4, 1010), 0x00},
	{"vdd_sai_pll_1p8", power, IMX8MPVBD_POWER_PATH(1, 4, 1, 0x13, 1000, 1000), 0x00},
	{"vdd_hdmi_1p8", power, IMX8MPVBD_POWER_PATH(1, 4, 2, 0x13, 1000, 1000), 0x00},
	{"vdd_earc_1p8", power, IMX8MPVBD_POWER_PATH(1, 4, 3, 0x13, 1000, 11000), 0x00},
	{"vdd_mipi_1p8", power, IMX8MPVBD_POWER_PATH(1, 4, 4, 0x13, 1000, 11000), 0x00},
	{"vdd_arm_pll_1p8", power, IMX8MPVBD_POWER_PATH(1, 5, 1, 0x14, 1000, 11000), 0x00},
	{"vdd_usb_1p8", power, IMX8MPVBD_POWER_PATH(1, 5, 2, 0x14, 1000, 11000), 0x00},
	{"vdd_24m_xtal_1p8", power, IMX8MPVBD_POWER_PATH(1, 5, 3, 0x14, 2000, 2000), 0x00},
	{"vdd_lvds_1p8", power, IMX8MPVBD_POWER_PATH(1, 5, 4, 0x14, 1000, 11000), 0x00},
	{"vdd_ana1_pll_0p8", power, IMX8MPVBD_POWER_PATH(0, 6, 1, 0x15, 1000, 11000), 0x00},
	{"vdd_arm_pll_0p8", power, IMX8MPVBD_POWER_PATH(0, 6, 2, 0x15, 1000, 11000), 0x00},
	{"vdd_dram_pll_0p8", power, IMX8MPVBD_POWER_PATH(0, 6, 3, 0x15, 1000, 11000), 0x00},
	{"vdd_pci_0p8", power, IMX8MPVBD_POWER_PATH(0, 6, 4, 0x15, 1000, 11000), 0x00},
	{"vdd_1v8", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 7, 1, 0x16, 100, 2, 600), 0x00},
	{"nvcc_snvs_1v8_cpu", power, IMX8MPVBD_POWER_PATH(0, 7, 3, 0x16, 2500, 2500), 0x00},
	{"nvcc_sd2", power, IMX8MPVBD_POWER_PATH(0, 7, 4, 0x16, 500, 2500), 0x00},
	{"vdd_3v3", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 8, 2, 0x17, 100, 1, 600), 0x00},
	{"vdd_3v3_cpu_1", power, IMX8MPVBD_POWER_PATH(0, 8, 3, 0x17, 1000, 11000), 0x00},
	{"vdd_3v3_cpu_0", power, IMX8MPVBD_POWER_PATH(0, 8, 4, 0x17, 1000, 11000), 0x00},
	{"vdd_usb_0p8", power, IMX8MPVBD_POWER_PATH(0, 9, 1, 0x18, 1000, 11000), 0x00},
	{"vdd_sai_pll_0p8", power, IMX8MPVBD_POWER_PATH(0, 9, 2, 0x18, 1000, 1000), 0x00},
	{"vdd_mipi_0p8", power, IMX8MPVBD_POWER_PATH(0, 9, 3, 0x18, 1000, 11000), 0x00},
	{"vdd_hdmi_0p8", power, IMX8MPVBD_POWER_PATH(0, 9, 4, 0x18, 1000, 11000), 0x00},
	{"vdd_1v8_cpu_0", power, IMX8MPVBD_POWER_PATH(0, 10, 1, 0x19, 1000, 11000), 0x00},
	{"vdd_1v8_cpu_1", power, IMX8MPVBD_POWER_PATH(0, 10, 2, 0x19, 1000, 11000), 0x00},

	{"SR_vdd_soc", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x01), 0x00},
	{"SR_vdd_pci_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x02), 0x00},
	{"SR_vdd_dram_pll_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x04), 0x00},
	{"SR_vdd_arm_pll_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x08), 0x00},
	{"SR_vdd_ana1_pll_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x10), 0x00},
	{"SR_vdd_hdmi_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x20), 0x00},
	{"SR_vdd_mipi_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x40), 0x00},
	{"SR_vdd_usb_0p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 1, 0x80), 0x00},
	{"SR_vdd_3v3_cpu_0", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x01), 0x00},
	{"SR_vdd_3v3_cpu_1", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x02), 0x00},
	{"SR_vdd_3v3", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x04), 0x00},
	{"SR_vdd_1v8_cpu_0", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x08), 0x00},
	{"SR_vdd_1v8_cpu_1", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x10), 0x00},
	{"SR_vdd_1v8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x20), 0x00},
	{"SR_nvcc_sd2", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x40), 0x00},
	{"SR_vdd_dram_pll_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x23, 2, 0x80), 0x00},

	{"SR_vdd_earc_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 0, 0x08), 0x00},
	{"SR_vdd_mipi_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 0, 0x10), 0x00},
	{"SR_vdd_arm_pll_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 0, 0x20), 0x00},
	{"SR_vdd_usb_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 0, 0x40), 0x00},
	{"SR_pmic_dcdc_5v", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 0, 0x80), 0x00},
	{"dcdc_5v_en", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x04), 0x00},
	{"SR_vdd_pci_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x08), 0x00},
	{"SR_vdd_ana_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x10), 0x00},
	{"SR_vdd_lvds_1p8", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x20), 0x00},
	{"SR_vdd_arm", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x40), 0x00},
	{"SR_nvcc_dram_1v35", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(1, 0x22, 1, 0x80), 0x00},

	// {"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/at24cxx{addr=0x50;}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct mapping imx8mpddr4_board[] = {
	{"nvcc_snvs_1v8_cpu", power, IMX8MPVBD_POWER_PATH(0, 1, 1, 0x10, 100, 100), 0x00},
	{"vdda_1v8_3", power, IMX8MPVBD_POWER_PATH(0, 1, 2, 0x10, 100, 100), 0x00},
	{"nvcc_sd2", power, IMX8MPVBD_POWER_PATH(0, 1, 3, 0x10, 100, 100), 0x00},
	{"vdda_1v8_4", power, IMX8MPVBD_POWER_PATH(0, 1, 4, 0x10, 100, 100), 0x00},
	{"vdd_arm", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 2, 1, 0x11, 20, 2, 2020), 0x00},
	{"vpp_2v5", power, IMX8MPVBD_POWER_PATH(0, 2, 3, 0x11, 100, 100), 0x00},
	{"vdda_1v8_2", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 3, 2, 0x14, 1000, 1, 11000), 0x00},
	{"vdda_1v8_0", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 3, 4, 0x14, 1000, 3, 11000), 0x00},
	{"vdda_1v8_1", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 4, 2, 0x16, 1000, 1, 11000), 0x00},
	{"nvcc_dram_1v2", power, IMX8MPVBD_POWER_SWITCH_PATH(0, 4, 4, 0x16, 50, 3, 2000), 0x00},
	{"vdd08_phy_pll_1", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 5, 2, 0x12, 1000, 1, 11000), 0x00},
	{"vdd_3v3", power, IMX8MPVBD_POWER_PATH(1, 5, 3, 0x12, 100, 100), 0x00},
	{"vdd_3v3_cpu", power, IMX8MPVBD_POWER_PATH(1, 5, 4, 0x12, 100, 100), 0x00},
	{"vdd_1v8_cpu_0", power, IMX8MPVBD_POWER_PATH(1, 6, 1, 0x13, 100, 100), 0x00},
	{"vdd_1v8_cpu_1", power, IMX8MPVBD_POWER_PATH(1, 6, 2, 0x13, 100, 100), 0x00},
	{"vdd_1v8", power, IMX8MPVBD_POWER_PATH(1, 6, 3, 0x13, 100, 100), 0x00},
	{"dcdc_5v_pmic", power, IMX8MPVBD_POWER_PATH(1, 6, 4, 0x13, 10, 10), 0x00},
	{"vdd08_phy_pll_0", power, IMX8MPVBD_POWER_PATH(1, 7, 1, 0x15, 100, 100), 0x00},
	{"vdd_nand_cpu", power, IMX8MPVBD_POWER_PATH(1, 7, 2, 0x15, 100, 100), 0x00},
	{"vdd_soc", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 8, 3, 0x17, 10, 1, 1010), 0x00},
	{"vdd08_phy_pll_2", power, IMX8MPVBD_POWER_SWITCH_PATH(1, 8, 2, 0x12, 1000, 4, 11000), 0x00},

	{"SR_vdd_arm", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x01), 0x00},
	{"SR_nvcc_dram_1v2", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x02), 0x00},
	{"SR_vdda_1v8_1", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x04), 0x00},
	{"SR_vdda_1v8_2", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x08), 0x00},
	{"SR_vdda_1v8_0", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x10), 0x00},
	{"SR_vdd_soc", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x20), 0x00},
	{"SR_vdd08_phy_pll_2", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x40), 0x00},
	{"SR_vdd08_phy_pll_1", gpio, IMX8MPVBD_GPIO_EXTENDER_PATH(0, 0x22, 1, 0x80), 0x00},

	// {"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/at24cxx{addr=0x50;}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct boot_mode imx8mpevk_board_boot_modes[] = {
	{"fuse", 0x00},
	{"usb", 0x01},
	{"emmc", 0x02},
	{"sd",0x03},
	{"nand_256", 0x04},
	{"nand_512",0x05},
	{"qspi_3b_read", 0x06},
	{"qspi_hyperflash",0x07},
	{"ecspi",0x08},
	{"infinite_loop",0x0E},
	{NULL, 0}
};

struct board_links imx8mpevk_board_links[] = {
	"http://shlinux22.ap.freescale.net/internal-only/Linux_IMX_Core/latest/common_bsp/imx-boot/imx-boot-imx8mpevk-sd.bin-flash_evk",
	""
};

struct boot_mode null_boot_mode[] = {
	{NULL, 0}
};

struct board_links null_board_links[] = {
	"",
	""
};

struct board_info board_list[] =
{
	{"imx8dxlevk", imx8xxl, imx8xxl_boot_modes,// NULL,
		"[chip_power:vdd_main,3v3_io,vdd_usb_3v3,3v3_enet,vdd_snvs_4p2,vdd_memc,vdd_ddr_vddq,vdd_enet0_1p8_3p3,vdd_ana,1v8_1,1v8_2,1v8_3,1v8_4,1v8_5]", 
		imx8xxlevk_board_links},
	{"imx8dxl_ddr3_evk", imx8dxl_ddr3, imx8xxl_boot_modes, NULL, imx8xxlevk_board_links},
	{"imx8mpevkpwr", imx8mpevkpwr_board, imx8mpevk_board_boot_modes, NULL, imx8mpevk_board_links},
	{"imx8mpevk", imx8mpevk_board, imx8mpevk_board_boot_modes, NULL, imx8mpevk_board_links},
	{"imx8mpddr3l", imx8mpddr3l_board, null_boot_mode, NULL, null_board_links},
	{"imx8mpddr4", imx8mpddr4_board, null_boot_mode, NULL, null_board_links},
	//"imx9xxl",&imx9xxl_pins,
};
int num_of_boards = sizeof(board_list) / sizeof(struct board_info);

int have_gpio(char* gpio_name, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (strcmp(gpio_name, board->mappings[i].name) == 0)
		{
			return 0;
		}
		i++;
	}
	return -1;
}

struct board_info* get_board(char* board_name)
{
	for (int i = 0; i < num_of_boards; i++)
	{

		if (strcmp(board_name, board_list[i].name) == 0)//board found
		{
			return &board_list[i];
		}
	}
	printf("board model %s is not supported\n", board_name);
	return NULL;
}

struct board_info* get_board_by_id(int id)
{
	if (id >= num_of_boards)
		return NULL;

	return &board_list[id];
}

int get_board_numer(void)
{
	return num_of_boards;
}

int get_item_location(char* item_name, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (strcmp(item_name, board->mappings[i].name) == 0)
		{
			return i;
		}
		i++;
	}
	//printf("path not found\n");
	return -1;
}

int get_path(char* path, char* item_name, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (strcmp(item_name, board->mappings[i].name) == 0)
		{
			strcpy(path, board->mappings[i].path);
			return 0;
		}
		i++;
	}
	//printf("path not found\n");
	return -1;
}

int set_path(char* path, char* item_name, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (strcmp(item_name, board->mappings[i].name) == 0)
		{
			board->mappings[i].path = malloc(sizeof(char) * MAX_PATH_LENGTH);
			strcpy(board->mappings[i].path, path);
			return 0;
		}
		i++;
	}
	//printf("gpio/power not found\n");
	return -1;
}

int get_gpio_info_by_initid(char* gpio_name, char* path, int initid, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (board->mappings[i].type == gpio && (board->mappings[i].initinfo >> 4) == initid)
		{
			strcpy(gpio_name, board->mappings[i].name);
			strcpy(path, board->mappings[i].path);
			return (board->mappings[i].initinfo) & 0x0F;
		}
		i++;
	}

	return -1;
}

int get_power_index_by_showid(int showid, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (board->mappings[i].type == power && (board->mappings[i].initinfo >> 2) == showid)
		{
			return i;
		}
		i++;
	}

	return -1;
}

/*
 * get the maximum length of the power related variable name
 */
int get_max_power_name_length(struct board_info* board)
{
	size_t max = 0;
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (board->mappings[i].type == power && strlen(board->mappings[i].name) > max)
		{
			max = (int)strlen(board->mappings[i].name);
		}
		i++;
	}
	return max;
}

int get_boot_mode_offset(unsigned char boot_mode_pin_bitmask)
{
	int offset = 0;
	unsigned char hex = boot_mode_pin_bitmask;
	while (hex % 2 == 0)
	{
		offset++;
		hex = hex >> 1;
	}
	if (offset > 8)
	{
		printf("get_boot_mode_offset: something is wrong with the pin bitmask %x\n", boot_mode_pin_bitmask);
		return -1;
	}
	//printf("offset is %d\n", offset);
	return offset;
}

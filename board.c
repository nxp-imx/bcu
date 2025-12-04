/*
* Copyright 2019-2021 NXP.
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
#define IMX8XXL_EXP8_PATH(bitmask) "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/pca9548{channel=0;addr=0x70}/pca6408a{addr=0x20;pin_bitmask="#bitmask";opendrain=1;}"

struct mapping imx8xxl[] = {
	{"on_board_5v0",power,IMX8XXL_POWER_PATH(1,1,10,10), 0x00},
	{"vdd_usb_3v3",power,IMX8XXL_POWER_PATH(1,2,1650,1650), 0x00},
	{"3v3_io",power,IMX8XXL_POWER_PATH(1,3, 1650, 1650), 0x00},
	{"3v3_enet",power,IMX8XXL_POWER_PATH(1,4, 3300,3300), 0x00},
	{"3v3_pmic_in",power,IMX8XXL_POWER_PATH(2,1,10,10), 0x00},
	{"on_board_3v3", power, IMX8XXL_POWER_PATH(2,2, 10, 10), 0x00},
	{"vdd_snvs_4p2",power,IMX8XXL_POWER_PATH(2,3,2000,2000), 0x00},
	{"vdd_main",power,IMX8XXL_POWER_PATH(2,4, 64, 69800), 0x00},
	{"vdd_memc",power,IMX8XXL_POWER_PATH(3,1, 54, 17400), 0x00},
	{"vdd_ddr_vddq",power,IMX8XXL_POWER_PATH(3,2, 514, 1430000), 0x00},
	{"ddr_vdd2",power,IMX8XXL_POWER_PATH(3,3, 10, 10), 0x00},
	{"vdd_enet0_1p8_3p3",power,IMX8XXL_POWER_PATH(3,4, 3320, 3320), 0x00},
	{"vdd_ana",power,IMX8XXL_POWER_PATH(4,1, 4, 97600), 0x00},
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
	{"testmod_sel",gpio, IMX8XXL_EXP_PATH(1,0x40), 0x00},
	{"bootmode_sel",gpio, IMX8XXL_EXP_PATH(1,0x80), 0x90},

	{"reset",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x11},
	{"jtag_sel",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x01},
	{"onoff", gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x21},
	{"remote_en",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x40},
	{"ft_reset", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x31},
	{"pmic_stby", gpio, "/ft4232h_gpio{channel=3;pin_bitmask=0x20}", 0x00},

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/at24cxx{addr=0x57;type=0x0;}", 0x00},
	// {"93lcx6", ftdi_eeprom , "/ft4232h_eeprom{uasize=0xFF}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

// DXL Orange Box
struct mapping imx8dxl_obx[] = {

	{"boot_mode",gpio,IMX8XXL_EXP8_PATH(0x07), 0x30},
	{"testmod_sel",gpio, IMX8XXL_EXP8_PATH(0x8), 0x00},
	{"bootmode_sel",gpio, IMX8XXL_EXP8_PATH(0x10), 0x10},
	{"sd_pwr",gpio, IMX8XXL_EXP8_PATH(0x20), 0x00},
	{"smi_sel",gpio, IMX8XXL_EXP8_PATH(0xc0), 0x00},

	{"reset",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x21},
	{"jtag_sel",gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x11},
	{"onoff", gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x31},
	{"remote_en",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x50},
	{"ft_reset", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x41},
	{"sys_ps_pg", gpio, "/ft4232h_gpio{channel=3;pin_bitmask=0x08}", 0x00},
	{"pmic_stby", gpio, "/ft4232h_gpio{channel=3;pin_bitmask=0x20}", 0x00},

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/at24cxx{addr=0x57;type=0x0;}", 0x00},
	{"93lcx6", ftdi_eeprom , "/ft4232h_eeprom{uasize=0xFF}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct mapping imx8xxl_board_c1[] = {
	{"on_board_5v0",	power, IMX8XXL_POWER_PATH(1, 1, 10, 10), 0x00},
	{"vdd_usb_3v3",		power, IMX8XXL_POWER_PATH(1, 2, 1650, 1650), 0x00},
	{"3v3_io",		power, IMX8XXL_POWER_PATH(1, 3, 1650, 1650), 0x00},
	{"3v3_enet",		power, IMX8XXL_POWER_PATH(1, 4, 3300, 3300), 0x00},
	{"3v3_pmic_in",		power, IMX8XXL_POWER_PATH(2, 1, 10, 10), 0x00},
	{"on_board_3v3",	power, IMX8XXL_POWER_PATH(2, 2, 10, 10), 0x00},
	{"vdd_snvs_4p2",	power, IMX8XXL_POWER_PATH(2, 3, 10000, 10000), 0x00},
	{"vdd_main",		power, IMX8XXL_POWER_PATH(2, 4, 64, 2000), 0x00},
	{"vdd_memc",		power, IMX8XXL_POWER_PATH(3, 1, 54, 2000), 0x00},
	{"vdd_ddr_vddq",	power, IMX8XXL_POWER_PATH(3, 2, 54, 2000), 0x00},
	{"ddr_vdd2",		power, IMX8XXL_POWER_PATH(3, 3, 50, 50), 0x00},
	{"vdd_enet0_1p8_3p3",	power, IMX8XXL_POWER_PATH(3, 4, 3320, 3320), 0x00},
	{"vdd_ana",		power, IMX8XXL_POWER_PATH(4, 1, 4, 2000), 0x00},
	{"ddr_vdd1",		power, IMX8XXL_POWER_PATH(4, 2, 50, 50), 0x00},
	{"1v8_1",		power, IMX8XXL_POWER_PATH(4, 3, 600, 600), 0x00},
	{"1v8_2",		power, IMX8XXL_POWER_PATH(4, 4, 1000, 1000), 0x00},
	{"1v8_3",		power, IMX8XXL_POWER_PATH(5, 1, 1000, 1000), 0x00},
	{"1v8_4",		power, IMX8XXL_POWER_PATH(5, 2, 680, 680), 0x00},
	{"1v8_5",		power, IMX8XXL_POWER_PATH(5, 3, 440, 440), 0x00},
	{"on_board_1v8",	power, IMX8XXL_POWER_PATH(5, 4, 10, 10), 0x00},
	{"1v8_6",		power, IMX8XXL_POWER_PATH(6, 1, 10, 10), 0x00},
	{"ana1",		power, IMX8XXL_POWER_PATH(6, 2, 500, 500), 0x00},
	{"emmc0",		power, IMX8XXL_POWER_PATH(6, 3, 3000, 3000), 0x00},
	{"on_board_12v0",	power, IMX8XXL_POWER_PATH(6, 4, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX8XXL_EXP_PATH(0, 0x07), 0x80},
	{"sd_pwr",		gpio, IMX8XXL_EXP_PATH(0, 0x08), 0x51},
	{"sd_wp",		gpio, IMX8XXL_EXP_PATH(0, 0x10), 0x61},
	{"sd_cd",		gpio, IMX8XXL_EXP_PATH(0, 0x20), 0x71},
	{"exp5_p06",		gpio, IMX8XXL_EXP_PATH(0, 0x40), 0x00},
	{"exp5_p07",		gpio, IMX8XXL_EXP_PATH(0, 0x80), 0x00},
	{"SR_vdd_main",		gpio, IMX8XXL_EXP_PATH(1, 0x01), 0x00},
	{"SR_vdd_memc",		gpio, IMX8XXL_EXP_PATH(1, 0x02), 0x00},
	{"SR_vdd_ddr_vddq",	gpio, IMX8XXL_EXP_PATH(1, 0x04), 0x00},
	{"SR_vdd_ana",		gpio, IMX8XXL_EXP_PATH(1, 0x08), 0x00},
	{"exp5_p14",		gpio, IMX8XXL_EXP_PATH(1, 0x10), 0x00},
	{"exp5_p15",		gpio, IMX8XXL_EXP_PATH(1, 0x20), 0x00},
	{"testmod_sel",		gpio, IMX8XXL_EXP_PATH(1, 0x40), 0x00},
	{"bootmode_sel",	gpio, IMX8XXL_EXP_PATH(1, 0x80), 0x90},

	{"reset",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x11},
	{"jtag_sel",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x01},
	{"onoff",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x21},
	{"remote_en",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x40},
	{"ft_reset",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x31},
	{"pmic_stby",		gpio, "/ft4232h_gpio{channel=3;pin_bitmask=0x20}", 0x00},

	{"at24cxx",		bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0x60;val_bitmask=0x40}/at24cxx{addr=0x57;type=0x0;}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group imx8xxl_power_groups[] = {
	{"GROUP_SOC", "vdd_main,vdd_usb_3v3,3v3_enet,vdd_snvs_4p2,vdd_memc,vdd_ddr_vddq,vdd_ana,1v8_1,1v8_4,1v8_5"},
	{"GROUP_SOC_FULL", "vdd_main,vdd_usb_3v3,3v3_enet,vdd_snvs_4p2,vdd_memc,vdd_ddr_vddq,vdd_ana,1v8_1,1v8_4,1v8_5,3v3_io,vdd_enet0_1p8_3p3,1v8_2,1v8_3"},
	{"GROUP_DRAM", "ddr_vdd1,ddr_vdd2"},
	{"GROUP_PLATFORM", "on_board_12v0"},
	{"GROUP_CUSTOM", "3v3_pmic_in,3v3_io,3v3_enet,vdd_usb_3v3"},
	{"GROUP_on_board", "on_board_1v8,on_board_3v3,on_board_5v0"},
	{"GROUP_1v8_6", "1v8_6"},
	{NULL, 0}
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
struct mapping imx8mpevkpwr_board_a0[] = {
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

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/at24cxx{addr=0x50;type=0x1;}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct mapping imx8mpevkpwr_board_a1[] = {
	{"vdd_arm", power, IMX8MPEVK_POWER_SWITCH_PATH(1, 1, 0x11, 20, 2, 5010), 0x00},
	{"nvcc_dram_1v1", power, IMX8MPEVK_POWER_SWITCH_PATH(1, 3, 0x11, 50, 4, 2050), 0x00},
	{"vsys_5v", power, IMX8MPEVK_POWER_SWITCH_PATH(2, 1, 0x12, 20, 2, 420), 0x00},
	{"vdd_soc", power, IMX8MPEVK_POWER_SWITCH_PATH(2, 3, 0x12, 10, 4, 1010), 0x00},
	{"lpd4_vddq", power, IMX8MPEVK_POWER_SWITCH_PATH(3, 1, 0x13, 50, 2, 2050), 0x00},
	{"lpd4_vdd2", power, IMX8MPEVK_POWER_PATH(3, 3, 0x13, 50, 50), 0x00},
	{"nvcc_sd2", power, IMX8MPEVK_POWER_PATH(3, 4, 0x13, 1000, 1000), 0x00},
	{"vdd_lvds_1V8", power, IMX8MPEVK_POWER_PATH(4, 1, 0x14, 1000, 1000), 0x00},
	{"vdd_hdmi_1v8", power, IMX8MPEVK_POWER_PATH(4, 2, 0x14, 2000, 2000), 0x00},
	{"nvcc_snvs_1v8", power, IMX8MPEVK_POWER_PATH(4, 3, 0x14, 10000, 509000), 0x00},
	{"vdd_earc_1v8", power, IMX8MPEVK_POWER_PATH(4, 4, 0x14, 2000, 2000), 0x00},
	{"vdd_usb_1v8", power, IMX8MPEVK_POWER_PATH(5, 1, 0x15, 2000, 2000), 0x00},
	{"vdd_pci_1v8", power, IMX8MPEVK_POWER_PATH(5, 2, 0x15, 1000, 1000), 0x00},
	{"vdd_mipi_1v8", power, IMX8MPEVK_POWER_PATH(5, 3, 0x15, 2000, 2000), 0x00},
	{"vdd_pll_ana_1v8", power, IMX8MPEVK_POWER_PATH(5, 4, 0x15, 1008, 11000), 0x00},
	{"nvcc_sd1", power, IMX8MPEVK_POWER_PATH(6, 1, 0x16, 100, 100), 0x00},
	{"lpd4_vdd1", power, IMX8MPEVK_POWER_PATH(6, 2, 0x16, 4990, 4990), 0x00},
	{"cpu_vdd_1v8", power, IMX8MPEVK_POWER_PATH(6, 3, 0x16, 100, 100), 0x00},
	{"bb_vdd_1v8", power, IMX8MPEVK_POWER_PATH(6, 4, 0x16, 50, 50), 0x00},
	{"vdd_pll_ana_0v8", power, IMX8MPEVK_POWER_PATH(7, 1, 0x17, 1000, 1000), 0x00},
	{"vdd_pci_0v8", power, IMX8MPEVK_POWER_PATH(7, 2, 0x17, 400, 400), 0x00},
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

	{"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/at24cxx{addr=0x50;type=0x1;}", 0x00},

	{NULL, 0, NULL, 0}//null terminated
};

struct board_power_group imx8mpevkpwr_power_groups[] = {
	{"GROUP_SOC", "vdd_arm,vdd_soc,nvcc_snvs_1v8,vdd_pll_ana_0v8,vdd_pll_ana_1v8,nvcc_dram_1v1,vdd_hdmi_0v8,vdd_hdmi_1v8,vdd_mipi_0v8,vdd_mipi_1v8,vdd_pci_0v8,vdd_pci_1v8,vdd_usb_0v8,vdd_usb_1v8,vdd_usb_3v3,vdd_lvds_1V8,vdd_earc_1v8"},
	{"GROUP_SOC_FULL", "vdd_arm,vdd_soc,nvcc_snvs_1v8,vdd_pll_ana_0v8,vdd_pll_ana_1v8,nvcc_dram_1v1,vdd_hdmi_0v8,vdd_hdmi_1v8,vdd_mipi_0v8,vdd_mipi_1v8,vdd_pci_0v8,vdd_pci_1v8,vdd_usb_0v8,vdd_usb_1v8,vdd_usb_3v3,vdd_lvds_1V8,vdd_earc_1v8,cpu_vdd_1v8,nvcc_sd1,nvcc_sd2"},
	{"GROUP_DRAM", "lpd4_vdd1,lpd4_vdd2,lpd4_vddq"},
	{"GROUP_PLATFORM", "vsys_5v"},
	{NULL, 0}
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

#define IMX8ULPEVK_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX8ULPEVK_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX8ULPEVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"
struct mapping imx8ulpevkb2_board[] = {
	{"buck4_cpu_1v1", power, IMX8ULPEVK_POWER_PATH(1, 1, 0x10, 2000, 2000), 0x00},
	{"buck1_cpu_1v8", power, IMX8ULPEVK_POWER_SWITCH_PATH(1, 3, 0x10, 100, 4, 10000), 0x00},
	{"buck2_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(2, 1, 0x11, 50, 2, 10000), 0x00},
	{"buck3_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(2, 3, 0x11, 20, 4, 10000), 0x00},
	{"ldo5_cpu_3v0", power, IMX8ULPEVK_POWER_PATH(3, 1, 0x12, 200000, 200000), 0x00},
	{"ldo1_cpu_1v1", power, IMX8ULPEVK_POWER_PATH(3, 2, 0x12, 500, 500), 0x00},
	{"buck1_lsw1_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(3, 3, 0x12, 1000, 1000), 0x00},
	{"buck1_lsw4_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(3, 4, 0x12, 1000, 1000), 0x00},
	{"buck1_lsw2_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 1, 0x13, 1000, 1000), 0x00},
	{"buck1_lsw3_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 2, 0x13, 2000, 2000), 0x00},
	{"ldo4_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 3, 0x13, 1000, 1000), 0x00},
	{"ldo2_cpu_3v3", power, IMX8ULPEVK_POWER_PATH(4, 4, 0x13, 1000, 1000), 0x00},
	{"ldo1_cpu_1v1_0v6", power, IMX8ULPEVK_POWER_PATH(5, 3, 0x14, 50, 50), 0x00},
	{"buck4_dram_1v1", power, IMX8ULPEVK_POWER_PATH(5, 4, 0x14, 50, 50), 0x00},

	{"boot_mode", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0xC0;opendrain=0;}", 0x30},
	{"boot_cfg0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x3F;opendrain=0;}", 0x00},
	{"remote_en", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"reset", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x02;opendrain=0;}", 0x10},
	{"onoff", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x04;opendrain=0;}", 0x20},
	{"reset0_b", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"reset1_b", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"cpu_power_mode0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"cpu_power_mode1", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"cpu_power_mode2", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"SR_buck1_cpu_1v8", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_buck4_cpu_1v1", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_buck2_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_buck3_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},

	{"ft_int_b", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_reset_boot_mode", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},

	// {"at24cxx", bcu_eeprom, "/ft4232h_i2c{channel=1;dir_bitmask=0xF8;val_bitmask=0x00}/at24cxx{addr=0x53;type=0x1;}", 0x00},
	{"93lcx6", ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx8ulpevk9_board[] = {
	{"buck4_cpu_1v1", power, IMX8ULPEVK_POWER_SWITCH_PATH(1, 1, 0x10, 2000, 2, 2000), 0x00},
	{"buck1_cpu_1v8", power, IMX8ULPEVK_POWER_SWITCH_PATH(1, 3, 0x10, 100, 4, 10000), 0x00},
	{"buck2_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(2, 1, 0x11, 50, 2, 10000), 0x00},
	{"buck3_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(2, 3, 0x11, 20, 4, 10000), 0x00},
	{"ldo5_cpu_3v0", power, IMX8ULPEVK_POWER_PATH(3, 1, 0x12, 200000, 200000), 0x00},
	{"ldo1_cpu_1v1", power, IMX8ULPEVK_POWER_PATH(3, 2, 0x12, 500, 500), 0x00},
	{"buck1_lsw1_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(3, 3, 0x12, 1000, 1000), 0x00},
	{"buck1_lsw4_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(3, 4, 0x12, 1000, 1000), 0x00},
	{"buck1_lsw2_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 1, 0x13, 1000, 1000), 0x00},
	{"buck1_lsw3_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 2, 0x13, 2000, 2000), 0x00},
	{"ldo4_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 3, 0x13, 1000, 1000), 0x00},
	{"ldo2_cpu_3v3", power, IMX8ULPEVK_POWER_PATH(4, 4, 0x13, 1000, 1000), 0x00},
	{"vsys_5v0_4v2", power, IMX8ULPEVK_POWER_SWITCH_PATH(5, 1, 0x14, 20, 2, 10000), 0x00},
	{"ldo1_cpu_1v1_0v6", power, IMX8ULPEVK_POWER_PATH(5, 3, 0x14, 50, 50), 0x00},
	{"buck4_dram_1v1", power, IMX8ULPEVK_POWER_PATH(5, 4, 0x14, 50, 50), 0x00},

	{"boot_mode", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0xC0;opendrain=0;}", 0x30},
	{"boot_cfg0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x3F;opendrain=0;}", 0x00},
	{"remote_en", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"reset", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x02;opendrain=0;}", 0x10},
	{"onoff", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x04;opendrain=0;}", 0x20},
	{"reset0_b", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"reset1_b", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"cpu_power_mode0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"cpu_power_mode1", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"cpu_power_mode2", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"SR_buck1_cpu_1v8", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_buck4_cpu_1v1", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_buck2_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_buck3_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_vsys_5v0_4v2", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x21;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},

	{"ft_int_b", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_reset_boot_mode", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},

	{"93lcx6", ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp", temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx8ulpwatchval_board[] = {
	{"buck1_cpu_1v8", power, IMX8ULPEVK_POWER_SWITCH_PATH(1, 1, 0x10, 100, 2, 10000), 0x00},
	{"buck1_io_1v8", power, IMX8ULPEVK_POWER_SWITCH_PATH(1, 3, 0x10, 100, 4, 10000), 0x00},
	{"buck2_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(2, 1, 0x11, 100, 2, 10000), 0x00},
	{"ldo5_cpu_3v0", power, IMX8ULPEVK_POWER_PATH(2, 3, 0x11, 200000, 200000), 0x00},
	{"ldo2_cpu_3v3", power, IMX8ULPEVK_POWER_PATH(2, 4, 0x11, 1000, 1000), 0x00},
	{"buck3_cpu_1v0", power, IMX8ULPEVK_POWER_SWITCH_PATH(3, 1, 0x12, 20, 2, 10000), 0x00},
	{"buck1_lsw2_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(3, 3, 0x12, 1000, 1000), 0x00},
	{"buck4_cpu_1v1", power, IMX8ULPEVK_POWER_PATH(3, 4, 0x12, 2000, 2000), 0x00},
	{"buck4_lsw4_cpu_1v1", power, IMX8ULPEVK_POWER_PATH(4, 1, 0x13, 500, 500), 0x00},
	{"buck1_lsw3_cpu_1v8", power, IMX8ULPEVK_POWER_PATH(4, 2, 0x13, 1000, 1000), 0x00},
	{"ldo1_cpu_0v6", power, IMX8ULPEVK_POWER_PATH(4, 3, 0x13, 50, 50), 0x00},
	{"pmic_4v2", power, IMX8ULPEVK_POWER_SWITCH_PATH(5, 1, 0x14, 20, 2, 10000), 0x00},
	{"ldo1_dram_0v6", power, IMX8ULPEVK_POWER_PATH(5, 3, 0x14, 50, 50), 0x00},
	{"buck4_dram_1v1", power, IMX8ULPEVK_POWER_PATH(5, 4, 0x14, 50, 50), 0x00},
	{"bat_4v2", power, IMX8ULPEVK_POWER_PATH(6, 1, 0x15, 50, 50), 0x00},
	{"buck1_emmc_1v8", power, IMX8ULPEVK_POWER_PATH(6, 2, 0x15, 100, 100), 0x00},
	{"buck1_dram_1v8", power, IMX8ULPEVK_POWER_PATH(6, 3, 0x15, 100, 100), 0x00},
	{"ldo3_emmc_3v3", power, IMX8ULPEVK_POWER_PATH(6, 4, 0x15, 100, 100), 0x00},
	{"per_1v8", power, IMX8ULPEVK_POWER_SWITCH_PATH(7, 1, 0x1F, 100, 2, 10000), 0x00},
	{"per_3v3", power, IMX8ULPEVK_POWER_SWITCH_PATH(7, 3, 0x1F, 100, 4, 10000), 0x00},

	{"SR_buck1_cpu_1v8", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_buck1_io_1v8", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_buck2_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_buck3_cpu_1v0", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_ldo1_cpu_0v6", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_pmic_4v2", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_per_1v8", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_per_3v3", gpio, IMX8ULPEVK_GPIO_EXTENDER_PATH"/pca6408a{addr=0x20;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"93lcx6", ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx8ulpwatchuwb_board[] = {
	{"vdd_pmic_4v2",	power, IMX8ULPEVK_POWER_PATH(1, 1, 0x10, 50, 50), 0x00},
	{"vdd_per_1v8",		power, IMX8ULPEVK_POWER_PATH(1, 2, 0x10, 100, 100), 0x00},
	{"vdd_per_3v3",		power, IMX8ULPEVK_POWER_PATH(1, 3, 0x10, 50, 50), 0x00},
	{"vdd_bat_4v2",		power, IMX8ULPEVK_POWER_PATH(1, 4, 0x10, 50, 50), 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group imx8ulpwatchuwb_power_groups[] = {
	{"GROUP_PMIC", "vdd_pmic_4v2"},
	{"GROUP_PERIPHERALS", "vdd_per_1v8,vdd_per_3v3"},
	{"GROUP_TOTAL", "vdd_bat_4v2"},
	{NULL, 0}
};

struct boot_mode imx8ulpevk_board_boot_modes[] = {
	{"fuse",	0x00},
	{"usb",		0x01},
	{"emmc_s",	0x02},
	{"emmc_nor_lp",	0x02},
	{"emmc_nor",	0x02},
	{"nand_nor",	0x02},
	{"nor_s",	0x02},
	{"nor_nor_lp",	0x02},
	{"nor_nor",	0x02},
	{NULL,		0}
};

struct boot_config imx8ulpevk_board_boot_config[] = {
	{"fuse",	{0x00}},
	{"usb",		{0x00}},
	{"emmc_s",	{0x00}},
	{"emmc_nor_lp",	{0x01}},
	{"emmc_nor",	{0x02}},
	{"nand_nor",	{0x12}},
	{"nor_s",	{0x20}},
	{"nor_nor_lp",	{0x21}},
	{"nor_nor",	{0x22}},
	{NULL,		{0}}
};

struct board_power_group imx8ulpevkpwr_power_groups[] = {
	{"GROUP_SOC_FULL", "buck1_cpu_1v8,buck2_cpu_1v0,buck3_cpu_1v0,buck4_cpu_1v1,ldo1_cpu_1v1,ldo1_cpu_1v1_0v6,buck1_lsw2_cpu_1v8,buck1_lsw3_cpu_1v8,ldo5_cpu_3v0,ldo2_cpu_3v3,buck1_lsw1_cpu_1v8,ldo4_cpu_1v8,buck1_lsw4_cpu_1v8"},
	{NULL, 0}
};

struct board_power_group imx8ulpwatchval_power_groups[] = {
	{"GROUP_SOC", "buck1_cpu_1v8,buck1_io_1v8,buck2_cpu_1v0,ldo5_cpu_3v0,ldo2_cpu_3v3,buck3_cpu_1v0,buck1_lsw2_cpu_1v8,buck4_cpu_1v1,buck4_lsw4_cpu_1v1,buck1_lsw3_cpu_1v8,ldo1_cpu_0v6"},
	{"GROUP_DRAM", "buck1_dram_1v8,buck4_dram_1v1,ldo1_dram_0v6"},
	{"GROUP_EMMC", "buck1_emmc_1v8,ldo3_emmc_3v3"},
	{"GROUP_PLATFORM", "bat_4v2"},
	{NULL, 0}
};

#define VAL_BOARD_1_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_1_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_1_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}"
struct mapping val_board_1[] = {
	{"vbus_in_dcdc",	power, VAL_BOARD_1_POWER_PATH(1, 1, 0x10, 10, 10), 0x00},
	{"dcdc_5v",		power, VAL_BOARD_1_POWER_PATH(1, 2, 0x10, 10, 10), 0x00},
	{"vdd_5v",		power, VAL_BOARD_1_POWER_PATH(1, 3, 0x10, 20, 20), 0x00},
	{"vddext_3v3",		power, VAL_BOARD_1_POWER_PATH(1, 4, 0x10, 20, 20), 0x00},
	{"vdd_arm",		power, VAL_BOARD_1_POWER_SWITCH_PATH(2, 1, 0x11, 20, 2, 5010), 0x00},
	{"nvcc_dram_1v1",	power, VAL_BOARD_1_POWER_SWITCH_PATH(2, 3, 0x11, 50, 4, 2050), 0x00},
	{"vsys_5v",		power, VAL_BOARD_1_POWER_SWITCH_PATH(3, 1, 0x12, 20, 2, 420), 0x00},
	{"vdd_soc",		power, VAL_BOARD_1_POWER_SWITCH_PATH(3, 3, 0x12, 20, 4, 1020), 0x00},
	{"vqspi_1v8",		power, VAL_BOARD_1_POWER_PATH(4, 1, 0x13, 1000, 1000), 0x00},
	{"lpd4_vddq",		power, VAL_BOARD_1_POWER_PATH(4, 2, 0x13, 100, 100), 0x00},
	{"lpd4_vdd2",		power, VAL_BOARD_1_POWER_PATH(4, 3, 0x13, 50, 50), 0x00},
	{"nvcc_sd2",		power, VAL_BOARD_1_POWER_PATH(4, 4, 0x13, 1000, 1000), 0x00},
	{"vdd_phy_0v9",		power, VAL_BOARD_1_POWER_PATH(5, 1, 0x14, 1000, 1000), 0x00},
	{"vdd_phy_1v2",		power, VAL_BOARD_1_POWER_PATH(5, 2, 0x14, 1000, 1000), 0x00},
	{"nvcc_snvs_1v8",	power, VAL_BOARD_1_POWER_PATH(5, 3, 0x14, 10000, 509000), 0x00},
	{"vdd_snvs_0v8",	power, VAL_BOARD_1_POWER_PATH(5, 4, 0x14, 10000, 509000), 0x00},
	{"vdd_usb_1v8",		power, VAL_BOARD_1_POWER_PATH(6, 1, 0x15, 2000, 2000), 0x00},
	{"vdd_pci_1v8",		power, VAL_BOARD_1_POWER_PATH(6, 2, 0x15, 1000, 1000), 0x00},
	{"vdd_mipi_1v8",	power, VAL_BOARD_1_POWER_PATH(6, 3, 0x15, 2000, 2000), 0x00},
	{"vdd_pll_ana_1v8",	power, VAL_BOARD_1_POWER_PATH(6, 4, 0x15, 1000, 1000), 0x00},
	{"nvcc_sd1",		power, VAL_BOARD_1_POWER_PATH(7, 1, 0x16, 100, 100), 0x00},
	{"lpd4_vdd1",		power, VAL_BOARD_1_POWER_PATH(7, 2, 0x16, 4990, 4990), 0x00},
	{"cpu_vdd_1v8",		power, VAL_BOARD_1_POWER_PATH(7, 3, 0x16, 100, 100), 0x00},
	{"bb_vdd_1v8",		power, VAL_BOARD_1_POWER_PATH(7, 4, 0x16, 50, 50), 0x00},
	{"vdd_pll_ana_0v8",	power, VAL_BOARD_1_POWER_PATH(8, 1, 0x17, 1000, 1000), 0x00},
	{"vdd_pci_0v8",		power, VAL_BOARD_1_POWER_PATH(8, 2, 0x17, 400, 400), 0x00},
	{"cpu_vdd_3v3",		power, VAL_BOARD_1_POWER_PATH(8, 3, 0x17, 1000, 1000), 0x00},
	{"vccq_sd1",		power, VAL_BOARD_1_POWER_PATH(8, 4, 0x17, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 1, 0x18, 1000, 1000), 0x00},
	{"vdd_usb_0v8",		power, VAL_BOARD_1_POWER_PATH(9, 2, 0x18, 400, 400), 0x00},
	{"vdd_sd1_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 3, 0x18, 250, 250), 0x00},
	{"bb_vdd_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 4, 0x18, 50, 50), 0x00},
	{"vdd_gpu",		power, VAL_BOARD_1_POWER_SWITCH_PATH(10, 1, 0x19, 100, 3, 10100), 0x00},
	{"vdd_vpu",		power, VAL_BOARD_1_POWER_SWITCH_PATH(10, 2, 0x19, 50, 4, 10050), 0x00},
	{"vdd_dram",		power, VAL_BOARD_1_POWER_SWITCH_PATH(11, 1, 0x1A, 50, 2, 10050), 0x00},
	{"vdd_dram_pll_0v8",	power, VAL_BOARD_1_POWER_PATH(11, 3, 0x1A, 250, 250), 0x00},
	{"nvcc_enet",		power, VAL_BOARD_1_POWER_PATH(11, 4, 0x1A, 100, 100), 0x00},

	{"boot_mode",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x30},
	{"SR_vsys_5v",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_nvcc_snvs_1v8",	gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_vdd_snvs_0v8",     gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00}, // shares SR_ pin with SR_nvcc_snvs_1v8
	{"SR_vdd_soc",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_vdd_vpu",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"remote_en",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"SR_vdd_gpu",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_vdd_dram",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_vdd_arm",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_nvcc_dram_1v1",	gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_io9",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_io10",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_io11",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"boot_cfg0",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0xFF;opendrain=0;}", 0x00},
	{"boot_cfg1",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0xFF;opendrain=0;}", 0x00},

	{"reset_b",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"reset",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x21},
	{"io_nrst",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x11},
	{"onoff",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"ft_gpio1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_gpio2",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_gpio3",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_1_power_groups[] = {
	{"GROUP_SOC",		"nvcc_snvs_1v8,vdd_snvs_0v8,vdd_soc,vdd_pll_ana_0v8,vdd_pci_0v8,vdd_usb_0v8,vdd_vpu,vdd_gpu,vdd_dram,vdd_dram_pll_0v8,vdd_phy_0v9,vdd_arm,vdd_pll_ana_1v8,vdd_usb_1v8,vdd_pci_1v8,vdd_mipi_1v8,nvcc_dram_1v1,vdd_usb_3v3,vdd_phy_1v2"},
	{"GROUP_SOC_FULL",	"nvcc_snvs_1v8,vdd_snvs_0v8,vdd_soc,vdd_pll_ana_0v8,vdd_pci_0v8,vdd_usb_0v8,vdd_vpu,vdd_gpu,vdd_dram,vdd_dram_pll_0v8,vdd_phy_0v9,vdd_arm,vdd_pll_ana_1v8,vdd_usb_1v8,vdd_pci_1v8,vdd_mipi_1v8,nvcc_dram_1v1,vdd_usb_3v3,vdd_phy_1v2,cpu_vdd_1v8,nvcc_sd1,nvcc_sd2,nvcc_enet,cpu_vdd_3v3"},
	{"GROUP_DRAM",		"lpd4_vdd1,lpd4_vdd2,lpd4_vddq"},
	{"GROUP_PLATFORM",	"vbus_in_dcdc"},
	{NULL, 0}
};

struct boot_mode val_board_1_boot_modes[] = {
	{"usb",		0x05},
	{"emmc",	0x06},
	{"sd",		0x06},
	{NULL,		0}
};

struct boot_config val_board_1_boot_config[] = {
	{"usb",		{0x00, 0x00}},
	{"emmc",	{0x20, 0x28}},
	{"sd",		{0x10, 0x14}},
	{NULL,		{0}}
};

struct mapping val_board_2[] = {
	{"vbus_in_dcdc",	power, VAL_BOARD_1_POWER_PATH(1, 1, 0x10, 10, 10), 0x00},
	{"dcdc_5v",		power, VAL_BOARD_1_POWER_PATH(1, 2, 0x10, 10, 10), 0x00},
	{"vdd_5v",		power, VAL_BOARD_1_POWER_PATH(1, 3, 0x10, 20, 20), 0x00},
	{"vddext_3v3",		power, VAL_BOARD_1_POWER_PATH(1, 4, 0x10, 20, 20), 0x00},
	{"vdd_arm",		power, VAL_BOARD_1_POWER_SWITCH_PATH(2, 1, 0x11, 20, 2, 5010), 0x00},
	{"nvcc_dram_1v1",	power, VAL_BOARD_1_POWER_SWITCH_PATH(2, 3, 0x11, 50, 4, 2050), 0x00},
	{"vsys_5v",		power, VAL_BOARD_1_POWER_SWITCH_PATH(3, 1, 0x12, 20, 2, 420), 0x00},
	{"vdd_soc",		power, VAL_BOARD_1_POWER_SWITCH_PATH(3, 3, 0x12, 20, 4, 1020), 0x00},
	{"vqspi_1v8",		power, VAL_BOARD_1_POWER_PATH(4, 1, 0x13, 1000, 1000), 0x00},
	{"lpd4_vddq",		power, VAL_BOARD_1_POWER_PATH(4, 2, 0x13, 100, 100), 0x00},
	{"lpd4_vdd2",		power, VAL_BOARD_1_POWER_PATH(4, 3, 0x13, 50, 50), 0x00},
	{"nvcc_sd2",		power, VAL_BOARD_1_POWER_PATH(4, 4, 0x13, 1000, 1000), 0x00},
	{"vdd_phy_0v9",		power, VAL_BOARD_1_POWER_PATH(5, 1, 0x14, 1000, 1000), 0x00},
	{"vdd_phy_1v2",		power, VAL_BOARD_1_POWER_PATH(5, 2, 0x14, 1000, 1000), 0x00},
	{"nvcc_snvs_1v8",	power, VAL_BOARD_1_POWER_PATH(5, 3, 0x14, 10000, 509000), 0x00},
	{"vdd_snvs_0v8",	power, VAL_BOARD_1_POWER_PATH(5, 4, 0x14, 10000, 509000), 0x00},
	{"vdd_gpu",		power, VAL_BOARD_1_POWER_SWITCH_PATH(6, 1, 0x15, 50, 2, 10050), 0x00},
	{"vdd_mipi_1v8",	power, VAL_BOARD_1_POWER_PATH(6, 3, 0x15, 2000, 2000), 0x00},
	{"vdd_pll_ana_1v8",	power, VAL_BOARD_1_POWER_PATH(6, 4, 0x15, 200, 200), 0x00},
	{"nvcc_sd1",		power, VAL_BOARD_1_POWER_PATH(7, 1, 0x16, 100, 100), 0x00},
	{"lpd4_vdd1",		power, VAL_BOARD_1_POWER_PATH(7, 2, 0x16, 4990, 4990), 0x00},
	{"cpu_vdd_1v8",		power, VAL_BOARD_1_POWER_PATH(7, 3, 0x16, 100, 100), 0x00},
	{"bb_vdd_1v8",		power, VAL_BOARD_1_POWER_PATH(7, 4, 0x16, 50, 50), 0x00},
	{"vdd_pll_ana_0v8",	power, VAL_BOARD_1_POWER_PATH(8, 1, 0x17, 1000, 1000), 0x00},
	{"vdd_usb_1v8",		power, VAL_BOARD_1_POWER_PATH(8, 2, 0x17, 2000, 2000), 0x00},
	{"cpu_vdd_3v3",		power, VAL_BOARD_1_POWER_PATH(8, 3, 0x17, 100, 100), 0x00},
	{"vccq_sd1",		power, VAL_BOARD_1_POWER_PATH(8, 4, 0x17, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 1, 0x18, 1000, 1000), 0x00},
	{"vdd_usb_0v8",		power, VAL_BOARD_1_POWER_PATH(9, 2, 0x18, 400, 400), 0x00},
	{"vdd_sd1_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 3, 0x18, 250, 250), 0x00},
	{"bb_vdd_3v3",		power, VAL_BOARD_1_POWER_PATH(9, 4, 0x18, 50, 50), 0x00},
	{"vdd_dram",		power, VAL_BOARD_1_POWER_SWITCH_PATH(10, 1, 0x1A, 50, 2, 10050), 0x00},
	{"vdd_dram_pll_0v8",	power, VAL_BOARD_1_POWER_PATH(10, 3, 0x1A, 250, 250), 0x00},
	{"nvcc_enet",		power, VAL_BOARD_1_POWER_PATH(10, 4, 0x1A, 100, 100), 0x00},

	{"boot_mode",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x30},
	{"SR_vsys_5v",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_nvcc_snvs_1v8",	gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_vdd_soc",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_vdd_snvs_0v8",	gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"remote_en",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x41},
	{"SR_vdd_gpu",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_vdd_dram",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_vdd_arm",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_nvcc_dram_1v1",	gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_io9",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_io10",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_io11",		gpio, VAL_BOARD_1_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"reset_b",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"reset",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x21},
	{"io_nrst",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x11},
	{"onoff",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"ft_gpio1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_gpio2",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_gpio3",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_2_power_groups[] = {
	{"GROUP_SOC",		"nvcc_snvs_1v8,vdd_snvs_0v8,vdd_soc,vdd_pll_ana_0v8,vdd_usb_0v8,vdd_gpu,vdd_dram,vdd_dram_pll_0v8,vdd_phy_0v9,vdd_arm,vdd_pll_ana_1v8,vdd_usb_1v8,vdd_mipi_1v8,nvcc_dram_1v1,vdd_usb_3v3,vdd_phy_1v2"},
	{"GROUP_SOC_FULL",	"nvcc_snvs_1v8,vdd_snvs_0v8,vdd_soc,vdd_pll_ana_0v8,vdd_usb_0v8,vdd_gpu,vdd_dram,vdd_dram_pll_0v8,vdd_phy_0v9,vdd_arm,vdd_pll_ana_1v8,vdd_usb_1v8,vdd_mipi_1v8,nvcc_dram_1v1,vdd_usb_3v3,vdd_phy_1v2,cpu_vdd_1v8,nvcc_sd1,nvcc_sd2,nvcc_enet,cpu_vdd_3v3"},
	{"GROUP_DRAM",		"lpd4_vdd1,lpd4_vdd2,lpd4_vddq"},
	{"GROUP_PLATFORM",	"vbus_in_dcdc"},
	{NULL, 0}
};

struct boot_mode val_board_2_boot_modes[] = {
	{"fuse",	0x0},
	{"usb",		0x1},
	{"emmc",	0x2},
	{"sd",		0x3},
	{"nand_256",	0x4},
	{"nand_512",	0x5},
	{"qspi_3b",	0x6},
	{"qspi_hyper",	0x7},
	{"ecspi",	0x8},
	{"loop",	0xe},
	{"test",	0xf},
	{NULL,		0}
};

#define IMX93EVK_POWER_SWITCH_GROUP_PATH(group, sensor1, addr, rsense1, group2, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";group2="#group2";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX93EVK_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX93EVK_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX93EVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping imx91qsb_board[] = {
	{"vdd_soc",		power, IMX93EVK_POWER_SWITCH_PATH(1, 1, 0x11, 20, 2, 1020), 0x00},
	{"nvcc_bbsm_1v8",	power, IMX93EVK_POWER_PATH(1, 3, 0x11, 10000, 10000), 0x00},
	{"vdd_ana_0v8",		power, IMX93EVK_POWER_PATH(4, 4, 0x11, 250, 250), 0x00},
	{"vdd_1v8",		power, IMX93EVK_POWER_PATH(2, 1, 0x12, 20, 20), 0x00},
	{"lpd4_vdd2",		power, IMX93EVK_POWER_SWITCH_PATH(2, 3, 0x12, 50, 4, 2050), 0x00},
	{"vdd_3v3",		power, IMX93EVK_POWER_PATH(3, 1, 0x13, 20, 20), 0x00},
	{"nvcc_3v3",		power, IMX93EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd_ana_1v8",		power, IMX93EVK_POWER_PATH(3, 3, 0x13, 250, 250), 0x00},
	{"vdd_usb_3v3",		power, IMX93EVK_POWER_PATH(3, 4, 0x13, 4990, 4990), 0x00},
	{"nvcc_1v8",		power, IMX93EVK_POWER_PATH(4, 1, 0x14, 100, 100), 0x00},
	{"lpd4_vdd1",		power, IMX93EVK_POWER_PATH(4, 2, 0x14, 250, 250), 0x00},
	{"vsys_5v",		power, IMX93EVK_POWER_PATH(4, 3, 0x14, 50, 50), 0x00},
	{"nvcc_sd2",		power, IMX93EVK_POWER_PATH(4, 4, 0x14, 1000, 1000), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"SR_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_lpd4_vdd2",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx93evk11_board[] = {
	{"vdd_1v8",		power, IMX93EVK_POWER_PATH(1, 1, 0x11, 20, 20), 0x00},
	{"vdd_3v3",		power, IMX93EVK_POWER_PATH(1, 3, 0x11, 20, 20), 0x00},
	{"nvcc_bbsm_1p8",	power, IMX93EVK_POWER_PATH(1, 4, 0x11, 10000, 10000), 0x00},
	{"lpd4x_vddq",		power, IMX93EVK_POWER_PATH(2, 1, 0x12, 250, 250), 0x00},
	{"vdd_soc",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 2, 0x12, 20, 1, 2, 1020), 0x00},
	{"nvcc_sd2",		power, IMX93EVK_POWER_PATH(2, 3, 0x12, 1000, 1000), 0x00},
	{"vdd2_ddr",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 4, 0x12, 50, 3, 3, 2050), 0x00},
	{"vddq_ddr",		power, IMX93EVK_POWER_SWITCH_PATH(3, 2, 0x13, 50, 1, 2050), 0x00},
	{"lpd4x_vdd2",		power, IMX93EVK_POWER_PATH(3, 4, 0x13, 100, 100), 0x00},
	{"vsys_5v",		power, IMX93EVK_POWER_PATH(4, 1, 0x14, 20, 20), 0x00},
	{"nvcc_3p3",		power, IMX93EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"vdd_usb_3p3",		power, IMX93EVK_POWER_PATH(4, 3, 0x14, 4990, 4990), 0x00},
	{"vdd_ana_0p8",		power, IMX93EVK_POWER_PATH(4, 4, 0x14, 400, 400), 0x00},
	{"lpd4x_vdd1",		power, IMX93EVK_POWER_PATH(5, 1, 0x15, 250, 250), 0x00},
	{"vdd_ana_1p8",		power, IMX93EVK_POWER_PATH(5, 2, 0x15, 250, 250), 0x00},
	{"vsd2_3v3",		power, IMX93EVK_POWER_PATH(5, 3, 0x15, 100, 100), 0x00},
	{"nvcc_1p8",		power, IMX93EVK_POWER_PATH(5, 4, 0x15, 100, 100), 0x00},
	{"vpcie_3v3",		power, IMX93EVK_POWER_PATH(6, 1, 0x16, 10, 10), 0x00},
	{"dcdc_5v",		power, IMX93EVK_POWER_PATH(6, 2, 0x16, 10, 10), 0x00},
	{"vsys_in",		power, IMX93EVK_POWER_PATH(6, 3, 0x16, 10, 10), 0x00},
	{"vdd_5v_in",		power, IMX93EVK_POWER_PATH(6, 4, 0x16, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"SR_vsys_5v",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x01;opendrain=0;active_level=0;}", 0x00},
	{"SR_nvcc_bbsm_1p8",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x02;opendrain=0;active_level=0;}", 0x00},
	{"SR_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x04;opendrain=0;active_level=0;}", 0x00},
	{"SR_vdd2_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=0;}", 0x00},
	{"SR_vddq_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=0;}", 0x00},
	{"SRD_vsys_5v",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=1;active_level=0;}", 0x00},
	{"SRD_nvcc_bbsm_1p8",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=1;active_level=0;}", 0x00},
	{"SRD_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=1;active_level=0;}", 0x00},
	{"SRD_vdd2_ddr",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=1;active_level=0;}", 0x00},
	{"SRD_vddq_ddr",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=1;active_level=0;}", 0x00},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx93evk11b1_board[] = {
	{"vdd_1v8",		power, IMX93EVK_POWER_PATH(1, 1, 0x11, 20, 20), 0x00},
	{"vdd_3v3",		power, IMX93EVK_POWER_PATH(1, 3, 0x11, 20, 20), 0x00},
	{"nvcc_bbsm_1p8",	power, IMX93EVK_POWER_PATH(1, 4, 0x11, 10000, 10000), 0x00},
	{"lpd4x_vddq",		power, IMX93EVK_POWER_PATH(2, 1, 0x12, 250, 250), 0x00},
	{"vdd_soc",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 2, 0x12, 5, 1, 2, 1005), 0x00},//
	{"nvcc_sd2",		power, IMX93EVK_POWER_PATH(2, 3, 0x12, 1000, 1000), 0x00},
	{"vdd2_ddr",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 4, 0x12, 50, 3, 3, 2050), 0x00},
	{"vddq_ddr",		power, IMX93EVK_POWER_SWITCH_PATH(3, 2, 0x13, 50, 1, 2050), 0x00},
	{"lpd4x_vdd2",		power, IMX93EVK_POWER_PATH(3, 4, 0x13, 100, 100), 0x00},
	{"vsys_5v",		power, IMX93EVK_POWER_PATH(4, 1, 0x14, 20, 20), 0x00},
	{"nvcc_3p3",		power, IMX93EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"vdd_usb_3p3",		power, IMX93EVK_POWER_PATH(4, 3, 0x14, 4990, 4990), 0x00},
	{"vdd_ana_0p8",		power, IMX93EVK_POWER_PATH(4, 4, 0x14, 400, 400), 0x00},
	{"lpd4x_vdd1",		power, IMX93EVK_POWER_PATH(5, 1, 0x15, 250, 250), 0x00},
	{"vdd_ana_1p8",		power, IMX93EVK_POWER_PATH(5, 2, 0x15, 250, 250), 0x00},
	{"vsd2_3v3",		power, IMX93EVK_POWER_PATH(5, 3, 0x15, 100, 100), 0x00},
	{"nvcc_1p8",		power, IMX93EVK_POWER_PATH(5, 4, 0x15, 100, 100), 0x00},
	{"vpcie_3v3",		power, IMX93EVK_POWER_PATH(6, 1, 0x16, 10, 10), 0x00},
	{"dcdc_5v",		power, IMX93EVK_POWER_PATH(6, 2, 0x16, 10, 10), 0x00},
	{"vsys_in",		power, IMX93EVK_POWER_PATH(6, 3, 0x16, 10, 10), 0x00},
	{"vdd_5v_in",		power, IMX93EVK_POWER_PATH(6, 4, 0x16, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"SR_vsys_5v",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=1;active_level=0;}", 0x00},
	{"SR_nvcc_bbsm_1p8",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=1;active_level=0;}", 0x00},
	{"SR_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=1;active_level=0;}", 0x00},
	{"SR_vdd2_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=1;active_level=0;}", 0x00},
	{"SR_vddq_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=1;active_level=0;}", 0x00},
	{"SRD_vsys_5v",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x01;opendrain=0;active_level=0;}", 0x00},
	{"SRD_nvcc_bbsm_1p8",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x02;opendrain=0;active_level=0;}", 0x00},
	{"SRD_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x04;opendrain=0;active_level=0;}", 0x00},
	{"SRD_vdd2_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=0;}", 0x00},
	{"SRD_vddq_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=0;}", 0x00},

	{"ft_tst0",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"ft_tst1",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x00},
	{"ft_tst2",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_tst3",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},
	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx93evk14_board[] = {
	{"vcc_12v0",		power, IMX93EVK_POWER_PATH(1, 1, 0x16, 10, 10), 0x00},
	{"vsys_5v0",		power, IMX93EVK_POWER_PATH(1, 2, 0x16, 10, 10), 0x00},
	{"vcc_5v0",		power, IMX93EVK_POWER_PATH(1, 3, 0x16, 10, 10), 0x00},
	{"vcc_ext_3v3",		power, IMX93EVK_POWER_PATH(1, 4, 0x16, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"ft_tst0",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x10}", 0x00},
	{"ft_tst1",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x20}", 0x00},
	{"ft_tst2",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_tst3",		gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},
	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping val_board_3[] = {
	{"nvcc_bbsm_1p8",	power, IMX93EVK_POWER_SWITCH_PATH(1, 4, 0x11, 10000, 1, 509000), 0x00},
	{"vdd_soc",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 2, 0x12, 20, 1, 2, 1020), 0x00},
	{"lpd4x_vddq",		power, IMX93EVK_POWER_PATH(2, 1, 0x12, 250, 250), 0x00},
	{"nvcc_sd2",		power, IMX93EVK_POWER_PATH(2, 3, 0x12, 1000, 1000), 0x00},
	{"vdd2_ddr",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(2, 4, 0x12, 50, 3, 3, 2050), 0x00},
	{"vddq_ddr",		power, IMX93EVK_POWER_SWITCH_PATH(3, 2, 0x13, 50, 1, 2050), 0x00},
	{"lpd4x_vdd2",		power, IMX93EVK_POWER_PATH(3, 4, 0x13, 100, 100), 0x00},
	{"vsys_5v",		power, IMX93EVK_POWER_SWITCH_GROUP_PATH(4, 1, 0x14, 20, 1, 3, 420), 0x00},
	{"nvcc_3p3",		power, IMX93EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"vdd_usb_3p3",		power, IMX93EVK_POWER_PATH(4, 3, 0x14, 4990, 4990), 0x00},
	{"vdd_ana_0p8",		power, IMX93EVK_POWER_PATH(4, 4, 0x14, 400, 400), 0x00},
	{"lpd4x_vdd1",		power, IMX93EVK_POWER_PATH(5, 1, 0x15, 250, 250), 0x00},
	{"vdd_ana_1p8",		power, IMX93EVK_POWER_PATH(5, 2, 0x15, 250, 250), 0x00},
	{"vsd2_3v3",		power, IMX93EVK_POWER_PATH(5, 3, 0x15, 100, 100), 0x00},
	{"nvcc_1p8",		power, IMX93EVK_POWER_PATH(5, 4, 0x15, 100, 100), 0x00},
	{"vpcie_3v3",		power, IMX93EVK_POWER_PATH(6, 1, 0x16, 10, 10), 0x00},
	{"dcdc_5v",		power, IMX93EVK_POWER_PATH(6, 2, 0x16, 10, 10), 0x00},
	{"vsys_in",		power, IMX93EVK_POWER_PATH(6, 3, 0x16, 10, 10), 0x00},
	{"vdd_5v_in",		power, IMX93EVK_POWER_PATH(6, 4, 0x16, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"SR_vsys_5v",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x01;opendrain=0;active_level=0;}", 0x00},
	{"SR_nvcc_bbsm_1p8",	gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x02;opendrain=0;active_level=0;}", 0x00},
	{"SR_vdd_soc",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x04;opendrain=0;active_level=0;}", 0x00},
	{"SR_vdd2_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=0;}", 0x00},
	{"SR_vddq_ddr",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=0;}", 0x00},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx93qsb_board[] = {
	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct boot_mode imx91qsb_board_boot_modes[] = {
	{"fuse", 0x00},
	{"usb", 0x01},
	{"emmc", 0x02},
	{"sd", 0x03},
	{"nor", 0x04},
	{"nand_2k", 0x05},
	{NULL, 0}
};

struct boot_mode imx93evk11_board_boot_modes[] = {
	{"fuse", 0x00},
	{"usb", 0x01},
	{"emmc", 0x02},
	{"sd", 0x03},
	{"nor", 0x04},
	{"nand_2k", 0x05},
	{"infinite_loop", 0x06},
	{"test_mode", 0x07},
	{"m_fuse", 0x08},
	{"m_usb", 0x09},
	{"m_emmc", 0x0a},
	{"m_sd", 0x0b},
	{"m_nor", 0x0c},
	{"m_nand_2k", 0x0d},
	{"m_infinite_loop", 0x0e},
	{"m_test_mode", 0x0f},
	{NULL, 0}
};

struct board_power_group imx93evk11_power_groups[] = {
	{"GROUP_SOC", "nvcc_bbsm_1p8,vdd_soc,vdd2_ddr,vddq_ddr,vdd_ana_0p8,vdd_ana_1p8,vdd_usb_3p3"},
	{"GROUP_SOC_FULL", "nvcc_bbsm_1p8,vdd_soc,vdd2_ddr,vddq_ddr,vdd_ana_0p8,vdd_ana_1p8,vdd_usb_3p3,nvcc_sd2,nvcc_3p3,nvcc_1p8"},
	{"GROUP_DRAM", "lpd4x_vdd1,lpd4x_vddq,lpd4x_vdd2"},
	{"GROUP_PLATFORM", "vsys_in"},
	{NULL, 0}
};

#define IMX95EVK_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX95EVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"
struct mapping imx95evk19_board[] = {
	{"vdd_arm",		power, IMX95EVK_POWER_PATH(1, 1, 0x11, 5, 5), 0x00},
	{"vdd_soc",		power, IMX95EVK_POWER_PATH(1, 2, 0x11, 5, 5), 0x00},
	{"lpd5_vddq",		power, IMX95EVK_POWER_PATH(1, 3, 0x11, 10, 10), 0x00},
	{"lpd5_vdd2",		power, IMX95EVK_POWER_PATH(1, 4, 0x11, 10, 10), 0x00},
	{"lpd5_vdd1",		power, IMX95EVK_POWER_PATH(2, 1, 0x12, 400, 400), 0x00},
	{"vdd_ddr",		power, IMX95EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},
	{"vddq_ddr",		power, IMX95EVK_POWER_PATH(2, 3, 0x12, 10, 10), 0x00},
	{"vdd2_ddr",		power, IMX95EVK_POWER_PATH(2, 4, 0x12, 250, 250), 0x00},
	{"nvcc_sdio2",		power, IMX95EVK_POWER_PATH(3, 1, 0x13, 100, 100), 0x00},
	{"nvcc_3v3",		power, IMX95EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, IMX95EVK_POWER_PATH(3, 3, 0x13, 400, 400), 0x00},
	{"vdd_ana_0v8",		power, IMX95EVK_POWER_PATH(3, 4, 0x13, 10, 10), 0x00},
	{"vdd_ana_1v8",		power, IMX95EVK_POWER_PATH(4, 1, 0x14, 50, 50), 0x00},
	{"nvcc_enet_ccm",	power, IMX95EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"nvcc_wakeup",		power, IMX95EVK_POWER_PATH(4, 3, 0x14, 100, 100), 0x00},
	{"nvcc_bbsm_1v8",	power, IMX95EVK_POWER_PATH(4, 4, 0x14, 10000, 10000), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x70},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x60},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x51},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x41},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},
	{"ft_fta_sel",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x31},
	{"onoff_ptc",	gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x01;opendrain=0;}", 0x91},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{"ptc",		ptc, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/ptc_revA{addr=0x57}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx95evk15_board[] = {
	{"vdd_arm",		power, IMX95EVK_POWER_PATH(1, 1, 0x11, 5, 5), 0x00},
	{"vdd_soc",		power, IMX95EVK_POWER_PATH(1, 2, 0x11, 5, 5), 0x00},
	{"lpd4x_vddq",		power, IMX95EVK_POWER_PATH(1, 3, 0x11, 10, 10), 0x00},
	{"lpd4x_vdd2",		power, IMX95EVK_POWER_PATH(1, 4, 0x11, 10, 10), 0x00},
	{"lpd4x_vdd1",		power, IMX95EVK_POWER_PATH(2, 1, 0x12, 400, 400), 0x00},
	{"vdd_ddr",		power, IMX95EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},
	{"vddq_ddr",		power, IMX95EVK_POWER_PATH(2, 3, 0x12, 10, 10), 0x00},
	{"vdd2_ddr",		power, IMX95EVK_POWER_PATH(2, 4, 0x12, 250, 250), 0x00},
	{"nvcc_sd2",		power, IMX95EVK_POWER_PATH(3, 1, 0x13, 100, 100), 0x00},
	{"nvcc_3v3",		power, IMX95EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, IMX95EVK_POWER_PATH(3, 3, 0x13, 400, 400), 0x00},
	{"vdd_ana_0v8",		power, IMX95EVK_POWER_PATH(3, 4, 0x13, 10, 10), 0x00},
	{"vdd_ana_1v8",		power, IMX95EVK_POWER_PATH(4, 1, 0x14, 50, 50), 0x00},
	{"nvcc_enet",		power, IMX95EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"nvcc_wakeup",		power, IMX95EVK_POWER_PATH(4, 3, 0x14, 100, 100), 0x00},
	{"nvcc_bbsm_1v8",	power, IMX95EVK_POWER_PATH(4, 4, 0x14, 10000, 10000), 0x00},

	{"boot_mode",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x70},
	{"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"reset",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x60},
	{"onoff",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x50},
	{"remote_en",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x41},
	{"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	{"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},
	{"ft_fta_sel",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x21},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct boot_mode imx95evk_board_boot_modes[] = {
	{"a_usb", 0x01},
	{"a_emmc", 0x02},
	{"a_sd", 0x03},
	{"a_nor", 0x04},
	{"a_nand_2k", 0x05},
	{"a_nand_4k", 0x06},
	{"usb", 0x09},
	{"emmc", 0x0a},
	{"sd", 0x0b},
	{"nor", 0x0c},
	{"nand_2k", 0x0d},
	{"nand_4k", 0x0e},
	{NULL, 0}
};

struct board_power_group imx95evk19_power_groups[] = {
	{"GROUP_SOC", "vdd_arm,vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr"},
	{"GROUP_SOC_FULL", "vdd_arm,vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr,nvcc_sdio2,nvcc_3v3,nvcc_wakeup,nvcc_enet_ccm"},
	{"GROUP_DRAM", "lpd5_vdd1,lpd5_vddq,lpd5_vdd2"},
	{NULL, 0}
};

struct board_power_group imx95evk15_power_groups[] = {
	{"GROUP_SOC", "vdd_arm,vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr"},
	{"GROUP_SOC_FULL", "vdd_arm,vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr,nvcc_sd2,nvcc_3v3,nvcc_wakeup,nvcc_enet"},
	{"GROUP_DRAM", "lpd4x_vdd1,lpd4x_vddq,lpd4x_vdd2"},
	{NULL, 0}
};

struct mapping imx952evk19_board[] = {
	{"vdd_sd2_3v3",		power, IMX95EVK_POWER_PATH(1, 1, 0x11, 100, 100), 0x00},
	{"vdd_soc",		power, IMX95EVK_POWER_PATH(1, 2, 0x11, 5, 5), 0x00},
	{"lpddr_vddq",		power, IMX95EVK_POWER_PATH(1, 3, 0x11, 10, 10), 0x00},
	{"lpddr_vdd2",		power, IMX95EVK_POWER_PATH(1, 4, 0x11, 10, 10), 0x00},
	{"lpddr_vdd1",		power, IMX95EVK_POWER_PATH(2, 1, 0x12, 400, 400), 0x00},
	{"vdd_ddr",		power, IMX95EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},  //95 Only
	{"vddq_ddr",		power, IMX95EVK_POWER_PATH(2, 3, 0x12, 10, 10), 0x00},
	{"vdd2_ddr",		power, IMX95EVK_POWER_PATH(2, 4, 0x12, 250, 250), 0x00},
	{"nvcc_sdio2",		power, IMX95EVK_POWER_PATH(3, 1, 0x13, 100, 100), 0x00},
	{"nvcc_3v3",		power, IMX95EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, IMX95EVK_POWER_PATH(3, 3, 0x13, 400, 400), 0x00},
	{"vdd_ana_0v8",		power, IMX95EVK_POWER_PATH(3, 4, 0x13, 10, 10), 0x00},
	{"vdd_ana_1v8",		power, IMX95EVK_POWER_PATH(4, 1, 0x14, 50, 50), 0x00},
	{"nvcc_enet_ccm",	power, IMX95EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"nvcc_wakeup",		power, IMX95EVK_POWER_PATH(4, 3, 0x14, 100, 100), 0x00},
	{"nvcc_bbsm_1v8",	power, IMX95EVK_POWER_PATH(4, 4, 0x14, 10000, 10000), 0x00},

	{"boot_mode",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"reset",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x70},
	{"onoff",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x60},
	{"remote_en",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x51},
	{"mode_dir",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x41},
	{"ft_sd_pwren",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},
	{"ft_fta_sel",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x31},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx952evk15_board[] = {
	{"vdd_sd2_3v3",		power, IMX95EVK_POWER_PATH(1, 1, 0x11, 100, 100), 0x00},
	{"vdd_soc",		power, IMX95EVK_POWER_PATH(1, 2, 0x11, 5, 5), 0x00},
	{"lpddr_vddq",		power, IMX95EVK_POWER_PATH(1, 3, 0x11, 10, 10), 0x00},
	{"lpddr_vdd2",		power, IMX95EVK_POWER_PATH(1, 4, 0x11, 10, 10), 0x00},
	{"lpddr_vdd1",		power, IMX95EVK_POWER_PATH(2, 1, 0x12, 400, 400), 0x00},
	{"nvcc_ld_3v3",		power, IMX95EVK_POWER_PATH(2, 2, 0x12, 100, 100), 0x00},
	{"vddq_ddr",		power, IMX95EVK_POWER_PATH(2, 3, 0x12, 10, 10), 0x00},
	{"vdd2_ddr",		power, IMX95EVK_POWER_PATH(2, 4, 0x12, 250, 250), 0x00},
	{"nvcc_sd2",		power, IMX95EVK_POWER_PATH(3, 1, 0x13, 100, 100), 0x00},
	{"nvcc_3v3",		power, IMX95EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd_usb_3v3",		power, IMX95EVK_POWER_PATH(3, 3, 0x13, 400, 400), 0x00},
	{"vdd_ana_0v8",		power, IMX95EVK_POWER_PATH(3, 4, 0x13, 10, 10), 0x00},
	{"vdd_ana_1v8",		power, IMX95EVK_POWER_PATH(4, 1, 0x14, 50, 50), 0x00},
	{"nvcc_enet",		power, IMX95EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"nvcc_wakeup",		power, IMX95EVK_POWER_PATH(4, 3, 0x14, 100, 100), 0x00},
	{"nvcc_bbsm_1v8",	power, IMX95EVK_POWER_PATH(4, 4, 0x14, 10000, 10000), 0x00},

	{"boot_mode",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_por_b",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"reset",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x70},
	{"onoff",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x60},
	{"remote_en",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x51},
	{"mode_dir",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x41},
	{"ft_sd_pwren",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd_cd",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},
	{"ft_fta_sel",		gpio, IMX95EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x31},

	{"ft_io_nrst1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nint1",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",		gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct boot_mode imx952evk_board_boot_modes[] = {
	{"usb", 0x09},
	{"emmc", 0x0a},
	{"sd", 0x0b},
	{"nor", 0x0c},
	{"nand_2k", 0x0d},
	{"nand_4k", 0x0e},
	{NULL, 0}
};

struct board_power_group imx952evk19_power_groups[] = {
	{"GROUP_SOC", "vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr"},
	{"GROUP_SOC_FULL", "vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vdd_ddr,vddq_ddr,vdd2_ddr,nvcc_sdio2,nvcc_3v3,nvcc_wakeup,nvcc_enet_ccm"},
	{"GROUP_DRAM", "lpddr_vdd1,lpddr_vddq,lpddr_vdd2"},
	{NULL, 0}
};

struct board_power_group imx952evk15_power_groups[] = {
	{"GROUP_SOC", "vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,vddq_ddr,vdd2_ddr"},
	{"GROUP_SOC_FULL", "vdd_soc,nvcc_bbsm_1v8,vdd_ana_1v8,vdd_ana_0v8,vdd_usb_3v3,nvcc_ld_3v3,vddq_ddr,vdd2_ddr,nvcc_sd2,nvcc_3v3,nvcc_wakeup,nvcc_enet"},
	{"GROUP_DRAM", "lpddr_vdd1,lpddr_vddq,lpddr_vdd2"},
	{NULL, 0}
};


struct boot_mode null_boot_mode[] = {
	{NULL, 0}
};

struct board_links null_board_links[] = {
	"",
	""
};

#define NXP_CUSTOM_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=0;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define NXP_CUSTOM_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=0;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping nxp_custom_board[] = {
	{"J1_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 180), 0x00},
	{"J2_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(2, 	2, 0x10, 20, 1, 180), 0x00},
	{"J3_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(3, 	2, 0x10, 20, 1, 180), 0x00},
	{"J4_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(4, 	2, 0x10, 20, 1, 180), 0x00},
	{"J5_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(5, 	2, 0x10, 20, 1, 180), 0x00},
	{"J6_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(6, 	2, 0x10, 20, 1, 180), 0x00},
	{"J7_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(7, 	2, 0x10, 20, 1, 180), 0x00},
	{"J8_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(8, 	2, 0x10, 20, 1, 180), 0x00},
	{"J9_Rail",		power, NXP_CUSTOM_POWER_SWITCH_PATH(9, 	2, 0x10, 20, 1, 180), 0x00},
	{"J10_Rail",	power, NXP_CUSTOM_POWER_SWITCH_PATH(10,	2, 0x10, 20, 1, 180), 0x00},

	{"SR_J1_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},
	{"SR_J2_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_J3_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_J4_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_J5_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},

	{"SR_J6_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},
	{"SR_J7_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_J8_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_J9_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_J10_Rail",	gpio, NXP_CUSTOM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group nxp_custom_power_groups[] = {
	// {"GROUP_SOC", "J1_Rail,J2_Rail,J3_Rail,J4_Rail,J5_Rail,J6_Rail,J7_Rail,J8_Rail,J9_Rail,J10_Rail"},
	{NULL, 0}
};

#define NXP_CUSTOM_revB_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define NXP_CUSTOM_revB_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define NXP_CUSTOM_revB_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping nxp_custom_revB_board[] = {
	{"J1_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 996), 0x00},
	{"J2_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},
	{"J3_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},
	{"J4_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},
	{"J5_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},
	{"J6_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 996), 0x00},
	{"J7_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 996), 0x00},
	{"J8_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(8, 	2, 0x17, 20, 1, 996), 0x00},
	{"J9_Rail",		power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(9, 	2, 0x18, 20, 1, 996), 0x00},
	{"J10_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(10,	2, 0x19, 20, 1, 996), 0x00},
	{"J11_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(11, 	2, 0x1a, 20, 1, 996), 0x00},
	// {"J12_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(12, 	2, 0x1b, 20, 1, 30120), 0x00},
	// {"J13_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(13, 	2, 0x1c, 20, 1, 30120), 0x00},
	// {"J14_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(14, 	2, 0x1d, 20, 1, 30120), 0x00},
	// {"J15_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(15, 	2, 0x1e, 20, 1, 30120), 0x00},
	// {"J16_Rail",	power, NXP_CUSTOM_revB_POWER_SWITCH_PATH(16, 	2, 0x1f, 20, 1, 30120), 0x00},

	{"SR_J1_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J2_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_J3_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_J4_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_J5_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J6_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_J7_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_J8_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_J9_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_J10_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_J11_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	// {"SR_J12_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	// {"SR_J13_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	// {"SR_J14_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},
	// {"SR_J15_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	// {"SR_J16_Rail",	gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},

	// {"boot_mode",		gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},
	// // {"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=1;}", 0x21},
	{"onoff",               gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=1;}", 0x31},
	{"ptc_onoff",		gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x31},
	// {"remote_en",		gpio, NXP_CUSTOM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},
	// {"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	// {"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	// {"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{"ptc",		ptc, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/ptc_revA{addr=0x32}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group nxp_custom_revB_power_groups[] = {
	// {"GROUP_SOC", "J1_Rail,J2_Rail,J3_Rail,J4_Rail,J5_Rail,J6_Rail,J7_Rail,J8_Rail,J9_Rail,J10_Rail"},
	{NULL, 0}
};

struct boot_mode nxp_custom_revB_boot_modes[] = {
	{"emmc", 0x08},
	{"sd", 0x0C},
	{"serial", 0x04},
	{NULL, 0}
};

#define VAL_BOARD_6_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_6_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_6_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_6_board[] = {
	{"J1_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(1, 	2, 0x10, 2000, 1, 502000), 0x00},
	{"J2_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(2, 	2, 0x11, 2000, 1, 502000), 0x00},
	{"J3_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(3, 	2, 0x12, 2000, 1, 502000), 0x00},
	{"J4_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(4, 	2, 0x13, 2000, 1, 502000), 0x00},
	{"J5_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(5, 	2, 0x14, 2000, 1, 502000), 0x00},
	{"J6_Rail",		power, VAL_BOARD_6_POWER_SWITCH_PATH(6, 	2, 0x15, 2000, 1, 502000), 0x00},

	{"SR_J1_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J2_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_J3_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_J4_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_J5_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J6_Rail",	gpio, VAL_BOARD_6_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	
	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_6_power_groups[] = {
	{"GROUP_SOC", "J1_Rail,J2_Rail,J3_Rail,J4_Rail,J5_Rail,J6_Rail"},
	{NULL, 0}
};


#define BENCH_IMX8QM_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_IMX8QM_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx8qm_board[] = {
	{"VCC_MEMC",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(1, 	2, 0x19, 20, 1, 996), 0x00},
	{"VCC_DDRIO0",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(2, 	2, 0x16, 20, 1, 519), 0x00},
	{"VCC_3V3",			power, BENCH_IMX8QM_POWER_SWITCH_PATH(3, 	2, 0x1B, 20, 1, 519), 0x00},
	{"VCC_GPU0",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(4, 	2, 0x12, 20, 1, 996), 0x00},
	{"VCC_GPU1",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(5, 	2, 0x13, 20, 1, 996), 0x00},
	{"VCC_MAIN",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(6, 	2, 0x14, 20, 1, 519), 0x00},
	{"VCC_1V8",			power, BENCH_IMX8QM_POWER_SWITCH_PATH(7, 	2, 0x15, 20, 1, 519), 0x00},
	{"VCC_DDRIO1",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(8, 	2, 0x17, 20, 1, 519), 0x00},
	{"VCC_CPU0",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(9, 	2, 0x18, 20, 1, 996), 0x00},
	{"VCC_CPU1",		power, BENCH_IMX8QM_POWER_SWITCH_PATH(10,	2, 0x1A, 20, 1, 996), 0x00},

	{"SR_VCC_MEMC",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_DDRIO0",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_3V3",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_GPU0",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_GPU1",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},

	{"SR_VCC_MAIN",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_1V8",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_DDRIO1",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_CPU0",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_CPU1",	gpio, BENCH_IMX8QM_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx8qm_power_groups[] = {
	{"GROUP_SOC", "VCC_CPU0,VCC_CPU1,VCC_DDRIO0,VCC_DDRIO1,VCC_GPU0,VCC_GPU1,VCC_MAIN,VCC_MEMC"},
	{"GROUP_SOC_FULL", "VCC_1V8,VCC_3V3,VCC_CPU0,VCC_CPU1,VCC_DDRIO0,VCC_DDRIO1,VCC_GPU0,VCC_GPU1,VCC_MAIN,VCC_MEMC"},
	{NULL, 0}
};


#define BENCH_IMX8QM_revB_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_IMX8QM_revB_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx8qm_revB_board[] = {
	{"VDD_PLATFORM",	power, BENCH_IMX8QM_revB_POWER_PATH(1, 	1, 0x10, 20, 0), 0x00},//J1
	{"VCC_CPU0",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},//J2//L7
	{"VCC_CPU1",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},//J3//L5+L6
	{"VCC_GPU0",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},//J4//L14+L15
	{"VCC_GPU1",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},//J5//L9+L11
	{"VCC_DDRIO0",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 519), 0x00},//J6//L12
	{"VCC_DDRIO1",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 519), 0x00},//J7//L4
	{"VCC_MAIN",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(8, 	2, 0x17, 20, 1, 519), 0x00},//J8//L1+L3
	{"VCC_MEMC",		power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(9, 	2, 0x18, 20, 1, 996), 0x00},//J9//L10
	{"VCC_1V8",			power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(10,	2, 0x19, 20, 1, 519), 0x00},//J10//L2
	{"VCC_3V3",			power, BENCH_IMX8QM_revB_POWER_SWITCH_PATH(11, 	2, 0x1A, 20, 1, 519), 0x00},//J11//L13

	{"SR_VCC_CPU0",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VCC_CPU1",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VCC_GPU0",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VCC_GPU1",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VCC_DDRIO0",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VCC_DDRIO1",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VCC_MAIN",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VCC_MEMC",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_VCC_1V8",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_VCC_3V3",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},

	{"boot_mode",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},//1-6
	{"reset",		gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x40;opendrain=1;}", 0x21},//7
	{"onoff",		gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=1;}", 0x31},//J1
	{"remote_en",	gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},//8
	{"onoff_2",		gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x01;opendrain=1;}", 0x51},//17
	
	{"onoff_otg",		gpio, BENCH_IMX8QM_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x01;opendrain=1;}", 0x51},//J12

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx8qm_revB_power_groups[] = {
	{"GROUP_SOC", "VCC_CPU0,VCC_CPU1,VCC_DDRIO0,VCC_DDRIO1,VCC_GPU0,VCC_GPU1,VCC_MAIN,VCC_MEMC"},
	{"GROUP_SOC_FULL", "VCC_1V8,VCC_3V3,VCC_CPU0,VCC_CPU1,VCC_DDRIO0,VCC_DDRIO1,VCC_GPU0,VCC_GPU1,VCC_MAIN,VCC_MEMC"},
	{"GROUP_PLATFORM", "VDD_PLATFORM"},
	{NULL, 0}
};

struct boot_mode bench_imx8qm_revB_boot_modes[] = {
	{"emmc", 0x08},
	{"sd", 0x0C},
	{"serial", 0x04},
	{NULL, 0}
};


#define BENCH_IMX8QXP_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_IMX8QXP_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx8qxp_board[] = {
	{"VCC_1V8",		power, BENCH_IMX8QXP_POWER_SWITCH_PATH(1, 	2, 0x19, 20, 1, 519), 0x00},
	{"VCC_3V3",		power, BENCH_IMX8QXP_POWER_SWITCH_PATH(2, 	2, 0x16, 20, 1, 519), 0x00},
	{"VCC_CPU",		power, BENCH_IMX8QXP_POWER_SWITCH_PATH(3, 	2, 0x1B, 20, 1, 996), 0x00},
	{"VCC_DDRIO",	power, BENCH_IMX8QXP_POWER_SWITCH_PATH(4, 	2, 0x12, 20, 1, 519), 0x00},
	{"VCC_GPU",		power, BENCH_IMX8QXP_POWER_SWITCH_PATH(5, 	2, 0x13, 20, 1, 996), 0x00},
	{"VCC_MAIN",	power, BENCH_IMX8QXP_POWER_SWITCH_PATH(6, 	2, 0x14, 20, 1, 519), 0x00},

	{"SR_VCC_1V8",	gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_3V3",	gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_CPU",	gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_DDRIO",gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_VCC_GPU",	gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},

	{"SR_VCC_MAIN",	gpio, BENCH_IMX8QXP_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx8qxp_power_groups[] = {
	{"GROUP_SOC", "VCC_CPU,VCC_DDRIO,VCC_GPU,VCC_MAIN"},
	{"GROUP_SOC_FULL", "VCC_1V8,VCC_3V3,VCC_CPU,VCC_DDRIO,VCC_GPU,VCC_MAIN"},
	{NULL, 0}
};


#define BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_IMX8QXP_revB_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx8qxp_revB_board[] = {
	{"VDD_PLATFORM",power, BENCH_IMX8QXP_revB_POWER_PATH(1, 	1, 0x10, 20, 0), 0x00},//J1
	{"VCC_CPU",		power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},//J2//L7
	{"VCC_GPU",		power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},//J3//L4
	{"VCC_MAIN",	power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 519), 0x00},//J4//L1+L2
	{"VCC_DDRIO",	power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 519), 0x00},//J5//L6
	{"VCC_1V8",		power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 519), 0x00},//J6//L5
	{"VCC_3V3",		power, BENCH_IMX8QXP_revB_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 519), 0x00},//J7//L3
	
	{"SR_VCC_CPU",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VCC_GPU",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VCC_MAIN",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VCC_DDRIO",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VCC_1V8",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VCC_3V3",	gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	
	{"boot_mode",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x0F;opendrain=1;}", 0x10},//1-4
	{"reset",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x10;opendrain=1;}", 0x21},//5
	{"onoff",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=1;}", 0x31},//J1
	{"remote_en",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},//8
	{"onoff_2",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x01;opendrain=1;}", 0x51},//17

	{"onoff_otg",		gpio, BENCH_IMX8QXP_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=1;}", 0x51},//J8
	
	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx8qxp_revB_power_groups[] = {
	{"GROUP_SOC", "VCC_CPU,VCC_DDRIO,VCC_GPU,VCC_MAIN"},
	{"GROUP_SOC_FULL", "VCC_1V8,VCC_3V3,VCC_CPU,VCC_DDRIO,VCC_GPU,VCC_MAIN"},
	{"GROUP_PLATFORM", "VDD_PLATFORM"},
	{NULL, 0}
};

struct boot_mode bench_imx8qxp_revB_boot_modes[] = {
	{"emmc", 0x02},
	{"sd", 0x03},
	{"serial", 0x01},
	{NULL, 0}
};

#define VAL_BOARD_4_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_4_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_4_board[] = {
	{"VDDCORE",		power, VAL_BOARD_4_POWER_SWITCH_PATH(1, 	2, 0x19, 20, 1, 180), 0x00},
	{"VDDRCORE",	power, VAL_BOARD_4_POWER_SWITCH_PATH(2, 	2, 0x16, 20, 1, 180), 0x00},
	{"VDDA18",		power, VAL_BOARD_4_POWER_SWITCH_PATH(3, 	2, 0x1B, 20, 1, 180), 0x00},
	{"VDDLPDDR",	power, VAL_BOARD_4_POWER_SWITCH_PATH(4, 	2, 0x12, 20, 1, 180), 0x00},
	{"DVDD18",		power, VAL_BOARD_4_POWER_SWITCH_PATH(5, 	2, 0x13, 20, 1, 180), 0x00},
	{"DVDD33",		power, VAL_BOARD_4_POWER_SWITCH_PATH(6, 	2, 0x14, 20, 1, 180), 0x00},
	{"VDDUSB",		power, VAL_BOARD_4_POWER_SWITCH_PATH(7, 	2, 0x15, 20, 1, 180), 0x00},

	{"SR_VDDCORE",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x01;opendrain=0;active_level=1;}", 0x00},
	{"SR_VDDRCORE",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x02;opendrain=0;active_level=1;}", 0x00},
	{"SR_VDDA18",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x04;opendrain=0;active_level=1;}", 0x00},
	{"SR_VDDLPDDR",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},
	{"SR_DVDD18",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=1;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},

	{"SR_DVDD33",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x10;opendrain=0;active_level=1;}", 0x00},
	{"SR_VDDUSB",	gpio, VAL_BOARD_4_GPIO_EXTENDER_PATH"/adp5585{addr=0x34;port=0;pin_bitmask=0x08;opendrain=0;active_level=1;}", 0x00},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_4_power_groups[] = {
	{"GROUP_SOC", "VDDCORE,VDDRCORE,VDDA18,VDDLPDDR,DVDD18,DVDD33"},
	{"GROUP_PLATFORM", "VDDUSB"},
	{NULL, 0}
};

#define VAL_BOARD_5_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_5_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_5_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_5_board[] = {
	{"0.9V_AWO",		power, VAL_BOARD_5_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 996), 0x00},
	{"0.9V_ISO",		power, VAL_BOARD_5_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},
	{"1.1V_DDR",		power, VAL_BOARD_5_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},
	{"VBAT_VDD",		power, VAL_BOARD_5_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},
	{"1.8V_DDR",		power, VAL_BOARD_5_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},
	{"5V_VDD_SOM",		power, VAL_BOARD_5_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 996), 0x00},
	{"5V_VDD_Platform",	power, VAL_BOARD_5_POWER_PATH(7, 	1, 0x16, 20, 0), 0x00},

	{"SR_0.9V_AWO",			gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_0.9V_ISO",			gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_1.1V_DDR",			gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VBAT_VDD",			gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x01},
	{"SR_1.8V_DDR",			gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x01},
	{"SR_5V_VDD_SOM",	gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},

	// {"boot_mode",		gpio, VAL_BOARD_5_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},
	{"reset",		gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x02;opendrain=1;}", 0x21},
	// {"onoff",		gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=1;}", 0x31},
	{"onoff",		gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=1;}", 0x31},
	{"remote_en",	gpio, VAL_BOARD_5_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_5_power_groups[] = {
	{"GROUP_SOC", "0.9V_AWO,0.9V_ISO"},
	{"GROUP_DDR", "1.1V_DDR,1.8V_DDR"},
	{"GROUP_BAT", "VBAT_VDD"},
	{"GROUP_SOM", "5V_VDD_SOM"},
	{"GROUP_PLATFORM", "5V_VDD_Platform"},
	{NULL, 0}
};

#define VAL_BOARD_8_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_8_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_8_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_8_board[] = {
	{"VDD_CORE",		power, VAL_BOARD_8_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 996), 0x00},
	{"VDDR_CORE",		power, VAL_BOARD_8_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},
	{"SoC_DVDD3V3",		power, VAL_BOARD_8_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},
	{"SoC_DVDD1V8",		power, VAL_BOARD_8_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},
	{"VDDA_1V8",		power, VAL_BOARD_8_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},
	{"VDD_LPDDR4",		power, VAL_BOARD_8_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 996), 0x00},
	{"VDD_platform",	power, VAL_BOARD_8_POWER_PATH(7, 	1, 0x16, 20, 0), 0x00},

	{"SR_VDD_CORE",			gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDDR_CORE",			gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_SoC_DVDD3V3",			gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_SoC_DVDD1V8",			gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VDDA_1V8",			gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_LPDDR4",	gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},

	// {"boot_mode",		gpio, VAL_BOARD_8_revB_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},
	{"reset",		gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x02;opendrain=1;}", 0x21},
	// {"onoff",		gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=1;}", 0x31},
	{"onoff",		gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=1;}", 0x31},
	{"remote_en",	gpio, VAL_BOARD_8_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_8_power_groups[] = {
	{"GROUP_SOC", "VDD_CORE,VDDR_CORE"},
	{"GROUP_SOC_FULL", "VDD_CORE,VDDR_CORE,SoC_DVDD3V3,SoC_DVDD1V8,VDDA_1V8"},
	{"GROUP_DRAM", "VDD_LPDDR4"},
	{"GROUP_PLATFORM", "VDD_platform"},
	{NULL, 0}
};

#define BENCH_IMX8MQ_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_IMX8MQ_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define BENCH_IMX8MQ_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx8mq_board[] = {
	{"NVCC_DRAM",	power, BENCH_IMX8MQ_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 519), 0x00},
	{"VDD_DRAM",	power, BENCH_IMX8MQ_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 519), 0x00},
	{"VPU",			power, BENCH_IMX8MQ_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 519), 0x00},
	{"GPU",			power, BENCH_IMX8MQ_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 519), 0x00},
	{"VDD_ARM_CORE",power, BENCH_IMX8MQ_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 519), 0x00},
	{"VDD_ARM_SoC",	power, BENCH_IMX8MQ_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 519), 0x00},
	{"VDD_PLATFORM",power, BENCH_IMX8MQ_POWER_PATH(7, 	1, 0x16, 20, 0), 0x00},

	{"SR_NVCC_DRAM",	gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_DRAM",		gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VPU",			gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_GPU",			gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VDD_ARM_CORE",	gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_ARM_SoC",	gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},

	{"boot_mode",	gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},
	{"reset",		gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x40;opendrain=1;}", 0x21},
	{"onoff",		gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0xA0;opendrain=1;}", 0x31},
	{"remote_en",	gpio, BENCH_IMX8MQ_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx8mq_power_groups[] = {
  {"GROUP_SOC_FULL", "VDD_ARM_CORE,GPU,VPU,VDD_DRAM,VDD_ARM_SoC,NVCC_DRAM"},
  {"GROUP_PLATFORM", "VDD_PLATFORM"},
  {NULL, 0}
};

struct boot_mode bench_imx8mq_boot_modes[] = {
	{"emmc", 0x22},
	{"sd", 0x2C},
	{"serial", 0x10},
	{NULL, 0}
};

#define IMX6ULL_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX6ULL_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX6ULL_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_imx6ull_revB_board[] = {
	{"VDD_SNVS_IN",		power, IMX6ULL_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 996), 0x00},
	{"NVCC_NAND",		power, IMX6ULL_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},
	{"VDD_ARM_SOC_IN",	power, IMX6ULL_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},
	{"VDDA_ADC_3P3",	power, IMX6ULL_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},
	{"DRAM_1V35",		power, IMX6ULL_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},
	{"VDD_HIGH_IN",		power, IMX6ULL_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 996), 0x00},
	{"DCDC_3V3",		power, IMX6ULL_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 996), 0x00},
	{"VDD_PLATFORM",	power, IMX6ULL_POWER_PATH(8, 	1, 0x17, 20, 20), 0x00},

	{"SR_VDD_SNVS_IN",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_NVCC_NAND",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VDD_ARM_SOC_IN",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VDDA_ADC_3P3",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_DRAM_1V35",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_HIGH_IN",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_DCDC_3V3",		gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VDD_PLATFORM",	gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	
	// {"boot_mode",		gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},
	// // {"ft_por_b",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x01;opendrain=0;}", 0x70},
	{"reset",		gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=1;}", 0x21},
	{"onoff",		gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=1;}", 0x31},
	// {"remote_en",		gpio, IMX6ULL_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=1;}", 0x41},
	// {"mode_dir",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x10;opendrain=0;}", 0x31},
	// {"ft_sd_pwren",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	// {"ft_sd_cd",		gpio, IMX93EVK_GPIO_EXTENDER_PATH"/pca9655e{addr=0x21;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_imx6ull_revB_power_groups[] = {
	{"GROUP_SOC_FULL", "VDD_SNVS_IN,NVCC_NAND,VDD_ARM_SOC_IN,VDDA_ADC_3P3,DRAM_1V35,VDD_HIGH_IN,DCDC_3V3"},
	{"GROUP_SOC", "VDD_SNVS_IN,VDD_ARM_SOC_IN,VDDA_ADC_3P3,DRAM_1V35,DCDC_3V3"},
	{"GROUP_PLATFORM", "VDD_PLATFORM"},
	{NULL, 0}
};

struct boot_mode bench_imx6ull_revB_boot_modes[] = {
	{"emmc", 0x08},
	{"sd", 0x0C},
	{"serial", 0x04},
	{NULL, 0}
};


#define BENCH_MCU_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define BENCH_MCU_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define BENCH_MCU_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping bench_mcu_board[] = {
	{"VDD_DCDC",	power, BENCH_MCU_POWER_SWITCH_PATH(1, 	2, 0x10, 180, 1, 100180), 0x00},
	{"VDD_CORE",	power, BENCH_MCU_POWER_SWITCH_PATH(2, 	2, 0x11, 180, 1, 100180), 0x00},
	{"VDD_LDO_SYS",	power, BENCH_MCU_POWER_SWITCH_PATH(3, 	2, 0x12, 180, 1, 100180), 0x00},
	{"VDD_MCU",	power, BENCH_MCU_POWER_SWITCH_PATH(4, 	2, 0x13, 180, 1, 100180), 0x00},
	{"VDD_P2",	power, BENCH_MCU_POWER_SWITCH_PATH(5, 	2, 0x14, 180, 1, 100180), 0x00},
	{"VDD_P3",	power, BENCH_MCU_POWER_SWITCH_PATH(6, 	2, 0x15, 180, 1, 100180), 0x00},
	{"VDD_P4",	power, BENCH_MCU_POWER_SWITCH_PATH(7, 	2, 0x16, 180, 1, 100180), 0x00},
	{"VDD_USB",	power, BENCH_MCU_POWER_SWITCH_PATH(8, 	2, 0x17, 180, 1, 100180), 0x00},
	{"VDDA",	power, BENCH_MCU_POWER_SWITCH_PATH(9, 	2, 0x18, 180, 1, 100180), 0x00},
	{"VDD_BAT",power, BENCH_MCU_POWER_SWITCH_PATH(10,	2, 0x19, 180, 1, 100180), 0x00},

	{"SR_VDD_DCDC",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_CORE",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VDD_LDO_SYS",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VDD_MCU",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VDD_P2",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_VDD_P3",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_VDD_P4",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_VDD_USB",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_VDDA",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_VDD_BAT",	gpio, BENCH_MCU_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group bench_mcu_power_groups[] = {
	{NULL, 0}
};

#define IMX943EVK_POWER_SWITCH_GROUP_PATH(group, sensor1, addr, rsense1, group2, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";group2="#group2";sensor2="#sensor2";rsense2="#rsense2"}"
#define IMX943EVK_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define IMX943EVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"
struct mapping imx943evk19a0_board[] = {
	{"lp_vdd_soc",		power, IMX943EVK_POWER_PATH(1, 1, 0x11, 200, 200), 0x00},
	{"vdd_soc",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 3, 0x17, 10, 1, 2, 5010), 0x00},
	{"cpu_vdd_ddr",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 3, 0x16, 10, 1, 3, 10010), 0x00},
	{"cpu_vdd2h_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 2, 0x17, 200, 2, 1, 10200), 0x00},
	{"vdd2h_ddr",		power, IMX943EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},
	{"sd_card_3v3",		power, IMX943EVK_POWER_PATH(2, 3, 0x12, 100, 100), 0x00},
	{"nvcc_sd2",		power, IMX943EVK_POWER_PATH(2, 4, 0x12, 100, 100), 0x00},
	{"cpu_1v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 4, 0x16, 50, 3, 1, 10050), 0x00},
	{"vcc_1p8",		power, IMX943EVK_POWER_PATH(3, 2, 0x13, 500, 500), 0x00},
	{"cpu_vddq_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 1, 0x17, 20, 3, 3, 10020), 0x00},
	{"vcc_vddq_ddr",	power, IMX943EVK_POWER_PATH(3, 4, 0x13, 20, 20), 0x00},
	{"nvcc_bbsm",		power, IMX943EVK_POWER_PATH(4, 1, 0x15, 10000, 10000), 0x00},
	{"cpu_3v3",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 1, 0x16, 50, 4, 2, 10050), 0x00},
	{"vcc_3p3",		power, IMX943EVK_POWER_PATH(4, 3, 0x15, 100, 100), 0x00},
	{"ana_0v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 2, 0x16, 10, 4, 4, 10010), 0x00},
	{"sw1_csense",		power, IMX943EVK_POWER_PATH(7, 1, 0x1A, 50, 50), 0x00},
	{"sw2_csense",		power, IMX943EVK_POWER_PATH(7, 2, 0x1A, 10, 10), 0x00},
	{"sw3_csense",		power, IMX943EVK_POWER_PATH(7, 3, 0x1A, 50, 50), 0x00},
	{"sw4_csense",		power, IMX943EVK_POWER_PATH(7, 4, 0x1A, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x50},
	{"mode_dir",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x21},
	{"remote_en",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x10},
	{"onoff",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x31},
	{"reset",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x41},
	{"fta_jtag_host_en",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"fta_jtag_uart_sel",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"int_temp_som",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_sys_rst_b",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_pf5302_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"ft_pf9455_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"ft_bb_pgood",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"ft_3v3_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"ft_12v_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_12v_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_som_pmic_pwron",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_som_pmic_stby",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"SR_cpu_3v3",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_cpu_vdd_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_ana_0v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_cpu_1v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_cpu_vddq_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_cpu_vdd2h_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_vdd_soc",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx943evk19b1_board[] = {
	{"vcc_vddq_ddr",	power, IMX943EVK_POWER_PATH(1, 2, 0x11, 20, 20), 0x00},
	{"cpu_vdd2h_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 3, 0x17, 200, 2, 1, 2200), 0x00},
	{"vdd2h_ddr",		power, IMX943EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},
	{"sd_card_3v3",		power, IMX943EVK_POWER_PATH(2, 3, 0x12, 100, 100), 0x00},
	{"nvcc_sd2",		power, IMX943EVK_POWER_PATH(2, 4, 0x12, 100, 100), 0x00},
	{"cpu_vdd_ddr",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(3, 4, 0x13, 10, 3, 1, 2010), 0x00},
	{"vcc_1p8",		power, IMX943EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"cpu_vddq_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 2, 0x17, 20, 3, 3, 10020), 0x00},
	{"nvcc_bbsm",		power, IMX943EVK_POWER_PATH(4, 1, 0x15, 10000, 10000), 0x00},
	{"cpu_3v3",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(4, 2, 0x15, 50, 4, 4, 2050), 0x00},
	{"vcc_3p3",		power, IMX943EVK_POWER_PATH(4, 3, 0x15, 100, 100), 0x00},
	{"cpu_1v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 1, 0x16, 50, 5, 2, 2050), 0x00},
	{"vdd_soc",		power, IMX943EVK_POWER_PATH(5, 3, 0x16, 10, 10), 0x00},
	{"lp_vdd_soc",		power, IMX943EVK_POWER_PATH(5, 4, 0x16, 200, 200), 0x00},
	{"ana_0v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 4, 0x17, 10, 6, 1, 2010), 0x00},
	{"sw1_csense",		power, IMX943EVK_POWER_PATH(7, 1, 0x1A, 50, 50), 0x00},
	{"sw2_csense",		power, IMX943EVK_POWER_PATH(7, 2, 0x1A, 10, 10), 0x00},
	{"sw3_csense",		power, IMX943EVK_POWER_PATH(7, 3, 0x1A, 50, 50), 0x00},
	{"sw4_csense",		power, IMX943EVK_POWER_PATH(7, 4, 0x1A, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x50},
	{"mode_dir",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x21},
	{"remote_en",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x10},
	{"onoff",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x31},
	{"reset",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x41},
	{"fta_jtag_host_en",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"fta_jtag_uart_sel",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"int_temp_som",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"ft_sys_rst_b",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"ft_pf5302_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_pf9455_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_som_pmic_pwron",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_som_pmic_stby",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"ft_12v_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"ft_3v3_bb_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"ft_5v0_pmic_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"ft_sd_pwren",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"ft_1v8_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_3v3_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_5v0_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_12v_ext_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"SR_cpu_3v3",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_cpu_vdd_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_ana_0v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_cpu_1v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_cpu_vddq_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_cpu_vdd2h_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},

	{"onoff_ptc",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{"ptc",		ptc, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/ptc_revA{addr=0x57}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct mapping imx943obx_board[] = {
	{"vcc_vddq_ddr",	power, IMX943EVK_POWER_PATH(1, 2, 0x11, 20, 20), 0x00},
	{"cpu_vdd2h_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 3, 0x17, 200, 2, 1, 2200), 0x00},
	{"vdd2h_ddr",		power, IMX943EVK_POWER_PATH(2, 2, 0x12, 10, 10), 0x00},
	{"sd_card_3v3",		power, IMX943EVK_POWER_PATH(2, 3, 0x12, 100, 100), 0x00},
	{"nvcc_sd2",		power, IMX943EVK_POWER_PATH(2, 4, 0x12, 100, 100), 0x00},
	{"cpu_vdd_ddr",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(3, 4, 0x13, 10, 3, 1, 2010), 0x00},
	{"vcc_1p8",		power, IMX943EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"cpu_vddq_ddr",	power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 2, 0x17, 20, 3, 3, 10020), 0x00},
	{"nvcc_bbsm",		power, IMX943EVK_POWER_PATH(4, 1, 0x15, 10000, 10000), 0x00},
	{"cpu_3v3",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(4, 2, 0x15, 50, 4, 4, 2050), 0x00},
	{"vcc_3p3",		power, IMX943EVK_POWER_PATH(4, 3, 0x15, 100, 100), 0x00},
	{"cpu_1v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(5, 1, 0x16, 50, 5, 2, 2050), 0x00},
	{"vdd_soc",		power, IMX943EVK_POWER_PATH(5, 3, 0x16, 10, 10), 0x00},
	{"lp_vdd_soc",		power, IMX943EVK_POWER_PATH(5, 4, 0x16, 200, 200), 0x00},
	{"ana_0v8",		power, IMX943EVK_POWER_SWITCH_GROUP_PATH(6, 4, 0x17, 10, 6, 1, 2010), 0x00},

	{"boot_mode",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x50},
	{"mode_dir",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x21},
	{"remote_en",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x10},
	{"onoff",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x31},
	{"reset",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x41},
	{"fta_jtag_host_en",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"fta_jtag_uart_sel",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"temp_int_bb_ft4232h_b",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_sys_rst_b",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_som_pf9455_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"ft_som_pf5302_pgood",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"ft_som_pmic_pwron",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"ft_pmic_stby",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"SR_cpu_3v3",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"SR_cpu_vdd_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_ana_0v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_cpu_1v8",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_cpu_vddq_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_cpu_vdd2h_ddr",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pca9670{addr=0x24;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group imx943evk19_power_groups[] = {
	{"GROUP_SOC", "lp_vdd_soc,vdd_soc,nvcc_bbsm,ana_0v8,cpu_1v8,cpu_3v3,cpu_vdd_ddr,cpu_vddq_ddr,cpu_vdd2h_ddr"},
	{"GROUP_SOC_FULL", "lp_vdd_soc,vdd_soc,nvcc_bbsm,ana_0v8,cpu_1v8,cpu_3v3,cpu_vdd_ddr,cpu_vddq_ddr,cpu_vdd2h_ddr,nvcc_sd2"},
	{"GROUP_DRAM", "vdd2h_ddr,vcc_vddq_ddr,vcc_1p8"},
	{NULL, 0}
};

struct mapping val_board_7_board[] = {
	{"vdd1_1v8_lp4_mem",		power, IMX943EVK_POWER_PATH(1, 1, 0x11, 400, 400), 0x00},
	{"vdd_ddr_0v8",			power, IMX943EVK_POWER_PATH(1, 2, 0x11, 10, 10), 0x00},
	{"vdd_soc_lp",			power, IMX943EVK_POWER_PATH(1, 3, 0x11, 100, 100), 0x00},
	{"nvcc_bbsm_1v8",		power, IMX943EVK_POWER_PATH(1, 4, 0x11, 10000, 10000), 0x00},
	{"vdd_soc",			power, IMX943EVK_POWER_PATH(2, 1, 0x12, 5, 5), 0x00},
	{"nvcc_1v8_3v3_enet_ccm",	power, IMX943EVK_POWER_PATH(2, 2, 0x12, 100, 100), 0x00},
	{"nvcc_sdio2",			power, IMX943EVK_POWER_PATH(2, 3, 0x12, 100, 100), 0x00},
	{"nvcc_1v8_3v3_wakeup",		power, IMX943EVK_POWER_PATH(2, 4, 0x12, 100, 100), 0x00},
	{"vdd_usb_3v3",			power, IMX943EVK_POWER_PATH(3, 1, 0x13, 400, 400), 0x00},
	{"nvcc_3v3",			power, IMX943EVK_POWER_PATH(3, 2, 0x13, 100, 100), 0x00},
	{"vdd2_lp4_1v1",		power, IMX943EVK_POWER_PATH(3, 3, 0x13, 250, 250), 0x00},
	{"vdd2_lp4_1v1_mem",		power, IMX943EVK_POWER_PATH(3, 4, 0x13, 10, 10), 0x00},
	{"vddq_ddr_0v8",		power, IMX943EVK_POWER_PATH(4, 1, 0x14, 10, 10), 0x00},
	{"vrefh_1v8_adc",		power, IMX943EVK_POWER_PATH(4, 2, 0x14, 100, 100), 0x00},
	{"vdd_ana_1v8",			power, IMX943EVK_POWER_PATH(4, 3, 0x14, 50, 50), 0x00},
	{"vdd_ana_0v8",			power, IMX943EVK_POWER_PATH(4, 4, 0x14, 10, 10), 0x00},

	{"boot_mode",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x0F;opendrain=0;}", 0x80},
	{"ft_tst0",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"ft_tst1",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"ft_tst2",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"tp77",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"ft_por",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"reset",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x70},
	{"onoff",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x04;opendrain=0;}", 0x60},
	{"remote_en",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x51},
	{"mode_dir",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x10;opendrain=0;}", 0x41},
	{"ft_sd2_pwren",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"ft_sd2_cd",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x40;opendrain=0;}", 0x21},
	{"ft_fta_sel",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x31},
	{"ft_i2c_rpt_en",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x01;opendrain=0;}", 0x00},
	{"ft_sd3_pwren",	gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"brd_id0",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"brd_id1",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"brd_id2",		gpio, IMX943EVK_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},

	{"ft_io_rst1_b",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_int1_b",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct boot_mode imx943_board_boot_modes[] = {
	{"fuse", 0x08},
	{"usb", 0x09},
	{"emmc", 0x0a},
	{"sd", 0x0b},
	{"nor", 0x0c},
	{"nand", 0x0d},
	{NULL, 0}
};

struct board_power_group imx943fval_power_groups[] = {
	{NULL, 0}
};

#define VAL_BOARD_9_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_9_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_9_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_9_board[] = {
	{"J1_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 30120), 0x00},
	{"J2_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 30120), 0x00},
	{"J3_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 30120), 0x00},
	{"J4_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 30120), 0x00},
	{"J5_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 30120), 0x00},
	{"J6_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 30120), 0x00},
	{"J7_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 30120), 0x00},
	{"J8_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(8, 	2, 0x17, 20, 1, 30120), 0x00},
	{"J9_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(9, 	2, 0x18, 20, 1, 30120), 0x00},
	{"J10_Rail",	power, VAL_BOARD_9_POWER_SWITCH_PATH(10,	2, 0x19, 20, 1, 30120), 0x00},

	{"SR_J1_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J2_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_J3_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_J4_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_J5_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_J6_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_J7_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_J8_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_J9_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_J10_Rail",	gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},

	// {"reset",		gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=1;}", 0x21},
	// {"onoff",		gpio, VAL_BOARD_9_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=1;}", 0x31},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",	ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",	temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_9_power_groups[] = {
	{"GROUP_ALL", "J1_Rail,J2_Rail,J3_Rail,J4_Rail,J5_Rail,J6_Rail,J7_Rail,J8_Rail,J9_Rail,J10_Rail"},
	{NULL, 0}
};

struct boot_mode val_board_9_boot_modes[] = {
	{"emmc", 0x08},
	{"sd", 0x0C},
	{"serial", 0x04},
	{NULL, 0}
};

#define VAL_BOARD_10_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"
struct mapping val_board_10_board[] = {
	{"boot_mode",	gpio, VAL_BOARD_10_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x3F;opendrain=1;}", 0x10},//1-6
	{"onoff",		gpio, VAL_BOARD_10_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x01;opendrain=1;}", 0x21},//17
	{"reset",		gpio, VAL_BOARD_10_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x02;opendrain=1;}", 0x31},//18
	{"onoff_board",	gpio, VAL_BOARD_10_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x04;opendrain=1;}", 0x41},//19
	{"remote_en",	gpio, VAL_BOARD_10_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x08;opendrain=1;}", 0x51},//20
	
	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_10_power_groups[] = {
	{NULL, 0}
};

struct boot_mode val_board_10_boot_modes[] = {
	{"fuse_imx8qm", 0x00},
	{"usb_imx8qm", 0x04},
	{"emmc_imx8qm", 0x08},
	{"sd_imx8qm", 0x0C},
	{"qspi_imx8qm", 0x18},

	{"fuse_imx8qxp", 0x00},
	{"usb_imx8qxp", 0x01},
	{"emmc_imx8qxp", 0x02},
	{"sd_imx8qxp", 0x03},
	{"qspi_imx8qxp", 0x06},

	{NULL, 0}
};

#define VAL_BOARD_11_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_11_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_11_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"

struct mapping val_board_11_board[] = {
	{"J1_Rail",	power, VAL_BOARD_11_POWER_PATH(1, 	1, 0x10, 20, 0), 0x00},
	{"J2_Rail",	power, VAL_BOARD_11_POWER_PATH(2, 	1, 0x11, 20, 0), 0x00},
	{"J3_Rail",	power, VAL_BOARD_11_POWER_PATH(3, 	1, 0x12, 20, 0), 0x00},
	{"J4_Rail",	power, VAL_BOARD_11_POWER_PATH(4, 	1, 0x13, 20, 0), 0x00},
	{"J1_Rail",	power, VAL_BOARD_11_POWER_PATH(5, 	1, 0x14, 20, 0), 0x00},
	{"J2_Rail",	power, VAL_BOARD_11_POWER_PATH(6, 	1, 0x15, 20, 0), 0x00},
	{"J3_Rail",	power, VAL_BOARD_11_POWER_PATH(7, 	1, 0x16, 20, 0), 0x00},
	{"J4_Rail",	power, VAL_BOARD_11_POWER_PATH(8, 	1, 0x17, 20, 0), 0x00},
	{"J1_Rail",	power, VAL_BOARD_11_POWER_PATH(9, 	1, 0x18, 20, 0), 0x00},
	{"J2_Rail",	power, VAL_BOARD_11_POWER_PATH(10, 	1, 0x19, 20, 0), 0x00},
	{"J3_Rail",	power, VAL_BOARD_11_POWER_PATH(11, 	1, 0x1a, 20, 0), 0x00},
	{"J4_Rail",	power, VAL_BOARD_11_POWER_PATH(12, 	1, 0x1b, 20, 0), 0x00},
	{"J1_Rail",	power, VAL_BOARD_11_POWER_PATH(13, 	1, 0x1c, 20, 0), 0x00},
	{"J2_Rail",	power, VAL_BOARD_11_POWER_PATH(14, 	1, 0x1d, 20, 0), 0x00},
	{"J3_Rail",	power, VAL_BOARD_11_POWER_PATH(15, 	1, 0x1e, 20, 0), 0x00},
	{"J4_Rail",	power, VAL_BOARD_11_POWER_PATH(16, 	1, 0x1f, 20, 0), 0x00},
	
	{"J1",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=1;}", 0x11},
	{"J2",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=1;}", 0x21},
	{"J3",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=1;}", 0x31},
	{"J4",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=1;}", 0x41},
	{"J5",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=1;}", 0x01},
	{"J6",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=1;}", 0x51},
	{"J7",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=1;}", 0x61},
	{"J8",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=1;}", 0x71},
	{"J9",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=1;}", 0x81},
	{"J10",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=1;}", 0x91},
	{"J11",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x04;opendrain=1;}", 0xa1},
	{"J12",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x01;opendrain=1;}", 0xb1},
	{"J13",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x02;opendrain=1;}", 0xc1},
	{"J14",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x08;opendrain=1;}", 0xd1},
	{"J15",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x20;opendrain=1;}", 0xe1},
	{"J16",		gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x20;port=0;pin_bitmask=0x80;opendrain=1;}", 0xf1},

	{"GPIO1",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=0;}", 0x11},
	{"GPIO2",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x02;opendrain=0;}", 0x11},
	{"GPIO3",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x04;opendrain=0;}", 0x11},
	{"GPIO4",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x08;opendrain=0;}", 0x11},
	{"GPIO5",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x10;opendrain=0;}", 0x11},
	{"GPIO6",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x20;opendrain=0;}", 0x11},
	{"GPIO7",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x40;opendrain=0;}", 0x11},
	{"GPIO8",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x80;opendrain=0;}", 0x11},

	{"GPIO9",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x01;opendrain=1;}", 0x11},
	{"GPIO10",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x02;opendrain=1;}", 0x11},
	{"GPIO11",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x04;opendrain=1;}", 0x11},
	{"GPIO12",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x08;opendrain=1;}", 0x11},
	{"GPIO13",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x10;opendrain=1;}", 0x11},
	{"GPIO14",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x20;opendrain=1;}", 0x11},
	{"GPIO15",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x40;opendrain=1;}", 0x11},
	{"GPIO16",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=1;pin_bitmask=0x80;opendrain=1;}", 0x11},

	{"GPIO17",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x01;opendrain=1;}", 0x11},
	{"GPIO18",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x02;opendrain=1;}", 0x11},
	{"GPIO19",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x04;opendrain=1;}", 0x11},
	{"GPIO20",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x08;opendrain=1;}", 0x11},
	{"GPIO21",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x10;opendrain=1;}", 0x11},
	{"GPIO22",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x20;opendrain=1;}", 0x11},
	{"GPIO23",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x40;opendrain=1;}", 0x11},
	{"GPIO24",	gpio, VAL_BOARD_11_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=2;pin_bitmask=0x80;opendrain=1;}", 0x11},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},
	
	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},
	
	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",		ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",		temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_11_power_groups[] = {
	// {"GROUP_SOC", "J1_Rail,J2_Rail,J3_Rail,J4_Rail,J5_Rail,J6_Rail,J7_Rail,J8_Rail,J9_Rail,J10_Rail"},
	{NULL, 0}
};

struct boot_mode val_board_11_boot_modes[] = {
	{"emmc", 0x08},
	{"sd", 0x0C},
	{"serial", 0x04},
	{NULL, 0}
};

#define VAL_BOARD_12_POWER_SWITCH_PATH(group, sensor1, addr, rsense1, sensor2, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor1";addr="#addr";rsense1="#rsense1";sensor2="#sensor2";rsense2="#rsense2"}"
#define VAL_BOARD_12_POWER_PATH(group, sensor, addr, rsense1, rsense2) "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}/pac1934{group="#group";sensor="#sensor";addr="#addr";rsense1="#rsense1";rsense2="#rsense2"}"
#define VAL_BOARD_12_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0xF0}"
struct mapping val_board_12_board[] = {
	{"CPU1V1",		power, VAL_BOARD_12_POWER_SWITCH_PATH(1, 	2, 0x10, 20, 1, 996), 0x00},
	{"CPU0V8_0",		power, VAL_BOARD_12_POWER_SWITCH_PATH(2, 	2, 0x11, 20, 1, 996), 0x00},
	{"CPU0V8_1",		power, VAL_BOARD_12_POWER_SWITCH_PATH(3, 	2, 0x12, 20, 1, 996), 0x00},
	{"CPU1V8",		power, VAL_BOARD_12_POWER_SWITCH_PATH(4, 	2, 0x13, 20, 1, 996), 0x00},
	{"CPU3V3",		power, VAL_BOARD_12_POWER_SWITCH_PATH(5, 	2, 0x14, 20, 1, 996), 0x00},
	{"CPU_VCC1833_0",	power, VAL_BOARD_12_POWER_SWITCH_PATH(6, 	2, 0x15, 20, 1, 996), 0x00},
	{"CPU_VCC1833_1",	power, VAL_BOARD_12_POWER_SWITCH_PATH(7, 	2, 0x16, 20, 1, 996), 0x00},
	{"CPU_VCC1833_2",	power, VAL_BOARD_12_POWER_SWITCH_PATH(8, 	2, 0x17, 20, 1, 996), 0x00},
	{"CPU_VCC1833_3",	power, VAL_BOARD_12_POWER_SWITCH_PATH(9, 	2, 0x18, 20, 1, 996), 0x00},
	{"CPU_VCC1833_4",	power, VAL_BOARD_12_POWER_SWITCH_PATH(10, 	2, 0x19, 20, 1, 996), 0x00},
	{"CPU_VCC1833_5",	power, VAL_BOARD_12_POWER_SWITCH_PATH(11,	2, 0x1a, 20, 1, 996), 0x00},
	{"CPU_VCC1833_6",	power, VAL_BOARD_12_POWER_SWITCH_PATH(12, 	2, 0x1b, 20, 1, 996), 0x00},

	{"SR_CPU1V1",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_CPU0V8_0",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_CPU0V8_1",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_CPU1V8",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=1;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_CPU3V3",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x02;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_0",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x08;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_1",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x20;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_2",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=2;pin_bitmask=0x80;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_3",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x40;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_4",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x10;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_5",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x04;opendrain=0;}", 0x00},
	{"SR_CPU_VCC1833_6",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x22;port=0;pin_bitmask=0x01;opendrain=0;}", 0x00},

	{"reset",		gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x01;opendrain=0;}", 0x01},
	{"remote_en",	gpio, VAL_BOARD_12_GPIO_EXTENDER_PATH"/pcal6524h{addr=0x23;port=0;pin_bitmask=0x02;opendrain=0;}", 0x10},

	{"ft_io_nint",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x08}", 0x00},
	{"ft_io_nrst",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}", 0x00},

	{"ft_io_nint1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst1",	gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}", 0x00},

	{"ft_io_nint2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x40}", 0x00},
	{"ft_io_nrst2",	gpio, "/ft4232h_gpio{channel=0;pin_bitmask=0x80}", 0x00},

	{"93lcx6",	ftdi_eeprom, "/ft4232h_eeprom{uasize=0xFF}", 0x00},
	{"temp",	temperature, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/pct2075{addr=0x48}", 0x00},

	{"ptc",		ptc, "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}/ptc_revA{addr=0x32}", 0x00},

	{NULL, 0, NULL, 0} //null terminated
};

struct board_power_group val_board_12_power_groups[] = {
	{"GROUP_SOC", "CPU1V1,CPU0V8_0,CPU0V8_1,CPU1V8,CPU3V3,CPU_VCC1833_0,CPU_VCC1833_1,CPU_VCC1833_2,CPU_VCC1833_3,CPU_VCC1833_4,CPU_VCC1833_5,CPU_VCC1833_6"},
	{"CPU0V8", "CPU0V8_0,CPU0V8_1"},
	{NULL, 0}
};

struct board_info board_list[] =
{
	{"imx8dxlevk",		imx8xxl,		imx8xxl_boot_modes,		0,	NULL,				imx8xxl_power_groups,		imx8xxlevk_board_links,		&imx8dxlevk_ftdi_eeprom_user_area_info,		500},
	{"imx8dxlevkc1",	imx8xxl_board_c1,	imx8xxl_boot_modes,		0,	NULL,				imx8xxl_power_groups,		imx8xxlevk_board_links,		&imx8dxlevk_c1_ftdi_eeprom_user_area_info,	500},
	{"imx8dxl_ddr3_evk",imx8dxl_ddr3,		imx8xxl_boot_modes,		0,	NULL,				NULL,				imx8xxlevk_board_links,		NULL,						500},
	{"imx8dxl_obx",		imx8dxl_obx,		imx8xxl_boot_modes,		0,	NULL,				NULL,				imx8xxlevk_board_links,		&imx8dxl_obx_ftdi_eeprom_user_area_info,	500},
	{"imx8mpevkpwra0",	imx8mpevkpwr_board_a0,	imx8mpevk_board_boot_modes,	0,	NULL,				imx8mpevkpwr_power_groups,	imx8mpevk_board_links,		&imx8mpevkpwr_a0_ftdi_eeprom_user_area_info,	500},
	{"imx8mpevkpwra1",	imx8mpevkpwr_board_a1,	imx8mpevk_board_boot_modes,	0,	NULL,				imx8mpevkpwr_power_groups,	imx8mpevk_board_links,		&imx8mpevkpwr_a1_ftdi_eeprom_user_area_info,	500},
	{"imx8mpevk",		imx8mpevk_board,	imx8mpevk_board_boot_modes,	0,	NULL,				NULL,				imx8mpevk_board_links,		NULL,						500},
	{"imx8mpddr4",		imx8mpddr4_board,	null_boot_mode,			0,	NULL,				NULL,				null_board_links,		NULL,						500},
	{"imx8ulpevkb2",	imx8ulpevkb2_board,	imx8ulpevk_board_boot_modes,	1,	imx8ulpevk_board_boot_config,	imx8ulpevkpwr_power_groups,	null_board_links,		&imx8ulpevkb2_ftdi_eeprom_user_area_info,	9000},
	{"imx8ulpevk9",		imx8ulpevk9_board,	imx8ulpevk_board_boot_modes,	1,	imx8ulpevk_board_boot_config,	imx8ulpevkpwr_power_groups,	null_board_links,		&imx8ulpevk9_ftdi_eeprom_user_area_info,	9000},
	{"imx8ulpwatchval",	imx8ulpwatchval_board,	null_boot_mode,			0,	NULL,				imx8ulpwatchval_power_groups,	null_board_links,		&imx8ulpwatchval_ftdi_eeprom_user_area_info,	500},
	{"imx8ulpwatchuwb",	imx8ulpwatchuwb_board,	null_boot_mode,			0,	NULL,				imx8ulpwatchuwb_power_groups,	null_board_links,		NULL,						0},
	{"val_board_1",		val_board_1,		val_board_1_boot_modes,		2,	val_board_1_boot_config,	val_board_1_power_groups,	null_board_links,		&val_board_1_ftdi_eeprom_user_area_info,	500},
	{"val_board_2",		val_board_2,		val_board_2_boot_modes,		0,	NULL,				val_board_2_power_groups,	null_board_links,		&val_board_2_ftdi_eeprom_user_area_info,	500},
	{"imx91qsb",		imx91qsb_board,		imx91qsb_board_boot_modes,	0,	NULL,				NULL,				null_board_links,		&imx91qsb_ftdi_eeprom_user_area_info,		9000},
	{"imx91evk11",		imx93evk11b1_board,	imx91qsb_board_boot_modes,	0,	NULL,				imx93evk11_power_groups,	null_board_links,		&imx91evk11_ftdi_eeprom_user_area_info,		500},
	{"imx93evk11",		imx93evk11_board,	imx93evk11_board_boot_modes,	0,	NULL,				imx93evk11_power_groups,	null_board_links,		&imx93evk11_ftdi_eeprom_user_area_info,		500},
	{"imx93evk11b1",	imx93evk11b1_board,	imx93evk11_board_boot_modes,	0,	NULL,				imx93evk11_power_groups,	null_board_links,		&imx93evk11b1_ftdi_eeprom_user_area_info,	500},
	{"imx93wevk",		imx93evk11b1_board,	imx93evk11_board_boot_modes,	0,	NULL,				imx93evk11_power_groups,	null_board_links,		&imx93wevk_ftdi_eeprom_user_area_info,		500},
	{"val_board_3",		val_board_3,		imx93evk11_board_boot_modes,	0,	NULL,				imx93evk11_power_groups,	null_board_links,		&val_board_3_ftdi_eeprom_user_area_info,	500},
	{"imx93qsb",		imx93qsb_board,		imx93evk11_board_boot_modes,	0,	NULL,				NULL,				null_board_links,		NULL,						500},
	{"imx93evk14",		imx93evk14_board,	imx93evk11_board_boot_modes,	0,	NULL,				NULL,				null_board_links,		&imx93evk14_ftdi_eeprom_user_area_info,		500},
	{"imx95evk19",		imx95evk19_board,	imx95evk_board_boot_modes,	0,	NULL,				imx95evk19_power_groups,	null_board_links,		&imx95evk19_ftdi_eeprom_user_area_info,		500},
	{"imx95evk15",		imx95evk15_board,	imx95evk_board_boot_modes,	0,	NULL,				imx95evk15_power_groups,	null_board_links,		&imx95evk15_ftdi_eeprom_user_area_info,		500},
	{"imx952evk15",		imx952evk15_board,	imx952evk_board_boot_modes,	0,	NULL,				imx952evk15_power_groups,	null_board_links,		&imx952evk15_ftdi_eeprom_user_area_info,	500},
	{"imx952evk19",		imx952evk19_board,	imx952evk_board_boot_modes,	0,	NULL,				imx952evk19_power_groups,	null_board_links,		&imx952evk19_ftdi_eeprom_user_area_info,	500},
	{"nxp_custom",		nxp_custom_board,	null_boot_mode,			0,	NULL,				nxp_custom_power_groups,	null_board_links,		&nxp_custom_ftdi_eeprom_user_area_info,		500},
	{"nxp_custom_revB",	nxp_custom_revB_board,	nxp_custom_revB_boot_modes,	0,	NULL,				nxp_custom_revB_power_groups,	null_board_links,		&nxp_custom_revB_ftdi_eeprom_user_area_info,	500},
	{"val_board_4",		val_board_4_board,	null_boot_mode,			0,	NULL,				val_board_4_power_groups,	null_board_links,		&val_board_4_ftdi_eeprom_user_area_info,	500},
	{"val_board_5",		val_board_5_board,	null_boot_mode,			0,	NULL,				val_board_5_power_groups,	null_board_links,		&val_board_5_ftdi_eeprom_user_area_info,	500},
	{"val_board_6",		val_board_6_board,	null_boot_mode,			0,	NULL,				val_board_6_power_groups,	null_board_links,		&val_board_6_ftdi_eeprom_user_area_info,	500},
	{"val_board_8",		val_board_8_board,	null_boot_mode,			0,	NULL,				val_board_8_power_groups,	null_board_links,		&val_board_8_ftdi_eeprom_user_area_info,	500},
	{"bench_imx8qm",	bench_imx8qm_board,	null_boot_mode,			0,	NULL,				bench_imx8qm_power_groups,	null_board_links,		&bench_imx8qm_ftdi_eeprom_user_area_info,	500},
	{"bench_imx8qm_revB",	bench_imx8qm_revB_board, bench_imx8qm_revB_boot_modes,	0,	NULL,				bench_imx8qm_revB_power_groups,	null_board_links,		&bench_imx8qm_revB_ftdi_eeprom_user_area_info,	500},
	{"bench_imx8qxp",	bench_imx8qxp_board,	null_boot_mode,			0,	NULL,				bench_imx8qxp_power_groups,	null_board_links,		&bench_imx8qxp_ftdi_eeprom_user_area_info,	500},
	{"bench_imx8qxp_revB",	bench_imx8qxp_revB_board, bench_imx8qxp_revB_boot_modes, 0,	NULL,				bench_imx8qxp_revB_power_groups, null_board_links,		&bench_imx8qxp_revB_ftdi_eeprom_user_area_info,	500},
	{"bench_imx8mq",	bench_imx8mq_board,	bench_imx8mq_boot_modes,	0,	NULL,				bench_imx8mq_power_groups,	null_board_links,		&bench_imx8mq_ftdi_eeprom_user_area_info,	500},
	{"bench_imx6ull",	bench_imx6ull_revB_board, bench_imx6ull_revB_boot_modes, 0,	NULL,				bench_imx6ull_revB_power_groups, null_board_links,		&bench_imx6ull_revB_ftdi_eeprom_user_area_info,	500},
	{"bench_mcu",		bench_mcu_board,	null_boot_mode,			0,	NULL,				bench_mcu_power_groups,		null_board_links,		&bench_mcu_ftdi_eeprom_user_area_info,		500},
	{"imx943evk19a0",	imx943evk19a0_board,	imx943_board_boot_modes,	0,	NULL,				imx943evk19_power_groups,	null_board_links,		&imx943evk19_ftdi_eeprom_user_area_info,	500},
	{"imx943evk19b1",	imx943evk19b1_board,	imx943_board_boot_modes,	0,	NULL,				imx943evk19_power_groups,	null_board_links,		&imx943evk19b1_ftdi_eeprom_user_area_info,	500},
	{"imx943obx",		imx943obx_board,	imx943_board_boot_modes,	0,	NULL,				imx943evk19_power_groups,	null_board_links,		&imx943obx_ftdi_eeprom_user_area_info,		500},
	{"val_board_7",		val_board_7_board,	imx943_board_boot_modes,	0,	NULL,				NULL,				null_board_links,		&val_board_7_ftdi_eeprom_user_area_info,	500},
	{"val_board_9",		val_board_9_board,	null_boot_mode,			0,	NULL,				val_board_9_power_groups,	null_board_links,		&val_board_9_ftdi_eeprom_user_area_info,	500},
	{"val_board_10",	val_board_10_board,	val_board_10_boot_modes,	0,	NULL,				val_board_10_power_groups,	null_board_links,		&val_board_10_ftdi_eeprom_user_area_info,	500},
	{"val_board_11",	val_board_11_board,	val_board_11_boot_modes,	0,	NULL,				val_board_11_power_groups,	null_board_links,		&val_board_11_ftdi_eeprom_user_area_info,	500},
	{"val_board_12",	val_board_12_board,	null_boot_mode,			0,	NULL,				val_board_12_power_groups,	null_board_links,		&val_board_12_ftdi_eeprom_user_area_info,	500},
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
	int findflag = 0;
	char board_tmp[30] = "";

	if (strlen(board_name) == 0)
	{
		printf("\nmissing option <-board=>\n");
		printf("\nOr use option <-auto> to find the board automatically\n");
		printf("NOTE: if other boards are also connected to the same host, <-auto> may break its ttyUSB function temporarily.\n\n");
		return NULL;
	}

	for (int i = 0; i < num_of_boards; i++)
	{
		if (strcmp(board_name, board_list[i].name) == 0)//board found
		{
			return &board_list[i];
		}
	}
	printf("board model %s is not supported", board_name);

	strcpy(board_tmp, board_name);
	for (int j = 0; j < strlen(board_name); j++)
	{
		board_tmp[strlen(board_tmp) - j] = 0;
		for (int i = 0; i < num_of_boards; i++)
		{
			if (strstr(board_list[i].name, board_tmp) != NULL)
			{
				if (!findflag)
				{
					findflag = 1;
					printf(", do you mean:\n\n\t");
				}
				printf("%s ", board_list[i].name);
			}
		}
		if (findflag)
			break;
	}
	printf("\n");

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
			return i;
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
		if (board->mappings[i].type == power && board->mappings[i].initinfo == showid)
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

char* get_boot_mode_name_from_hex(struct board_info* board, int boot_mode_hex)
{
	int i = 0;

	while (board->boot_modes[i].name != NULL)
	{
		if (board->boot_modes[i].boot_mode_hex == boot_mode_hex)
		{
			return board->boot_modes[i].name;
		}
		i++;
	}
	return NULL;
}

char* get_boot_config_name_from_hex(struct board_info* board, int *boot_config_hex, int boot_mode_hex)
{
	int i = 0;

	while (board->boot_modes[i].name != NULL)
	{
		if (board->boot_modes[i].boot_mode_hex == boot_mode_hex)
		{
			int j;
			for (j = 0; j < board->boot_cfg_byte_num; j++)
			{
				if (board->boot_configs[i].boot_config_hex[j] != boot_config_hex[j])
					break;
			}
			if (j >= board->boot_cfg_byte_num)
				return board->boot_configs[i].name;
		}
		i++;
	}
	return NULL;
}

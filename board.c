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
#define IMX8XXL_POWER_PATH(channel,sensor) "/ft4232h_i2c{channel=0;dir_bitmask=0x04;val_bitmask=0x00}/pca9548{channel="#channel";addr=0x77}/pac1934{sensor="#sensor";addr=0x10}"
#define IMX8XXL_EXP_PATH(port,bitmask) "/ft4232h_i2c{channel=0;dir_bitmask=0x04;val_bitmask=0x00}/pca9548{channel=0;addr=0x77}/pca6416a{addr=0x20;port="#port";pin_bitmask=0"#bitmask"}"
struct mapping imx8xxl[] = {
	"on_board_5v0",power,IMX8XXL_POWER_PATH(1,1),
	"vdd_usb_3v3",power,IMX8XXL_POWER_PATH(1,2),
	"3v3_io",power,IMX8XXL_POWER_PATH(1,3),
	"3v3_enet",power,IMX8XXL_POWER_PATH(1,4),
	"3v3_pmic_in",power,IMX8XXL_POWER_PATH(2,1),
	"on_board_3v3", power, IMX8XXL_POWER_PATH(2,2),
	"vdd_snvs_4p2",power,IMX8XXL_POWER_PATH(2,3),
	"vdd_main",power,IMX8XXL_POWER_PATH(2,4),
	"vdd_memc",power,IMX8XXL_POWER_PATH(3,1),
	"vdd_ddr_vddq",power,IMX8XXL_POWER_PATH(3,2),
	"ddr_vdd2",power,IMX8XXL_POWER_PATH(3,3),
	"vdd_enet0_1p8_3p3",power,IMX8XXL_POWER_PATH(3,4),
	"vdd_ana",power,IMX8XXL_POWER_PATH(4,1),
	"ddr_vdd1",power,IMX8XXL_POWER_PATH(4,2),
	"1v8_1",power,IMX8XXL_POWER_PATH(4,3),
	"1v8_2",power,IMX8XXL_POWER_PATH(4,4),
	"1v8_3",power,IMX8XXL_POWER_PATH(5,1),
	"vdd_usb_1p8",power,IMX8XXL_POWER_PATH(5,2),
	"vdd_pcie_1p8",power,IMX8XXL_POWER_PATH(5,3),
	"on_board_1v8",power,IMX8XXL_POWER_PATH(5,4),

	"SR_vdd_main",gpio,IMX8XXL_EXP_PATH(1,0x01),
	"SR_vdd_memc",gpio,IMX8XXL_EXP_PATH(1,0x02),
	"SR_vdd_ddr_vddq",gpio,IMX8XXL_EXP_PATH(1,0x04),
	"SR_vdd_ana",gpio, IMX8XXL_EXP_PATH(1,0x08),


	"boot_mode",gpio,IMX8XXL_EXP_PATH(0,0x07),
	"sd_pwr_host",gpio, IMX8XXL_EXP_PATH(0,0x08),
	"sd_wp_host",gpio, IMX8XXL_EXP_PATH(0,0x10),
	"sd_cd_host",gpio, IMX8XXL_EXP_PATH(0,0x20),

	"jtag_select",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x10}",
	"reset",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x20}",
	"testmod_sel",gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x40}",
	"pwr_on/off", gpio, "/ft4232h_gpio{channel=1;pin_bitmask=0x80}",
	// you put all the pin in the imx8xxl board here
	//"sd_wp", gpio, "..."
	NULL, 0, NULL//null terminated   
};

struct boot_mode imx8xxl_boot_modes[] = {
	"efuse", 0x00,
	"usb", 0x01,
	"emmc", 0x02,
	"sd",0x03,
	"nand", 0x04,
	"m4_infinite_loop",0x05,
	"spi", 0x06,
	"dft_burnin_mode",0x07,
	NULL, 0
};

#define SIMULATION_POWER_PATH "/ft4232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=1;addr=0x77}"
#define SIMULATION_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=0;addr=0x77}"
struct mapping simulation_board[] = {
	"vdd_main", power, SIMULATION_POWER_PATH"/pac1934{sensor=2;addr=0x10}",
	"vdd_snvs", power, SIMULATION_POWER_PATH"/pac1934{sensor=1;addr=0x10}",
	//"test_monitor", power, "/ft4232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=1;addr=0x77}/pca9548{channel=1;addr=0x77}/pac1934{sensor=1;addr=0x10}",
	"boot_mode", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x0E}",
	"sd_wp", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x10}",
	"all_port0", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0xFF}",
	"reset",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x01}",
	"testmod_sel",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x02}",
	"onoff",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x04}",
	"SR_vdd_main",gpio,SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01}",

	// you put all the pin in the imx8xxl board here
	//"sd_wp", gpio, "..."
	NULL, 0, NULL//null terminated   
};

struct boot_mode simulation_board_boot_modes[] = {
	"emmc", 0x02,
	"nand", 0x01,
	"sd", 0x03,
	NULL, 0
};

#define IMX8MPEVK_GPIO_EXTENDER_PATH "/ft4232h_i2c{channel=1;dir_bitmask=0xF0;val_bitmask=0x00}"
struct mapping imx8mpevk_board[] = {
	"boot_mode", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x0F}",
	"ft_io1", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x10}",
	"ft_io2", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x20}",
	"ft_io3", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x40}",
	"ft_io4", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x80}",
	"remote_en", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01}",
	"ft_io5", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x02}",
	"ft_io6", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x04}",
	"ft_io7", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x08}",
	"ft_io8", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x10}",
	"ft_io9", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x20}",
	"ft_io10", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x40}",
	"ft_io11", gpio, IMX8MPEVK_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x80}",

	"reset_b",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x10}",
	"reset",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x20}",
	"io_nrst",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x40}",
	"onoff",gpio,"/ft4232h_gpio{channel=0;pin_bitmask=0x80}",
	"io_nint",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x08}",

	"ft_gpio1",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x10}",
	"ft_gpio2",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x20}",
	"ft_gpio3",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x40}",
	"ft_gpio4",gpio,"/ft4232h_gpio{channel=1;pin_bitmask=0x80}",

	NULL, 0, NULL//null terminated
};

struct boot_mode imx8mpevk_board_boot_modes[] = {
	"fuse", 0x00,
	"usb", 0x01,
	"emmc", 0x02,
	"sd",0x03,
	"nand_256", 0x04,
	"nand_512",0x05,
	"qspi_3b_read", 0x06,
	"qspi_hyperflash",0x07,
	"ecspi",0x08,
	"infinite_loop",0x0E,
	NULL, 0
};

struct board_info board_list[] =
{
	"simulation_board", simulation_board,simulation_board_boot_modes,"[default_group:vdd_main]",
	"imx8xxl", imx8xxl,imx8xxl_boot_modes,NULL,
	"imx8mpevk", imx8mpevk_board, imx8mpevk_board_boot_modes, NULL
	//"imx9xxl",&imx9xxl_pins,
};
int num_of_boards = sizeof(board_list) / sizeof(struct board_info);

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

int get_path(char* path, char* gpio_name, struct board_info* board)
{
	int i = 0;
	while (board->mappings[i].name != NULL)
	{
		if (strcmp(gpio_name, board->mappings[i].name) == 0)
		{
			strcpy(path, board->mappings[i].path);
			return 0;
		}
		i++;
	}
	//printf("path not found\n");
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
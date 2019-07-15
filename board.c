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


struct mapping imx8xxl[]={
	"vdd_main", power, "/ft4232h{channel=1;scl=0;sda=1;sel=2}/pca9548{channel=2;addr=0x77}/pac1934{sensor=4;addr=0x77}",
	// you put all the pin in the imx8xxl board here
	//"sd_wp", gpio, "..."
	NULL, 0, NULL//null terminated   
};


#define SIMULATION_POWER_PATH "/ft2232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=1;addr=0x77}"
#define SIMULATION_GPIO_EXTENDER_PATH "/ft2232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=0;addr=0x77}"
struct mapping simulation_board[]={
	"vdd_main", power, SIMULATION_POWER_PATH"/pac1934{sensor=2;addr=0x10}",
	"vdd_snvs", power, SIMULATION_POWER_PATH"/pac1934{sensor=1;addr=0x10}",
	//"test_monitor", power, "/ft2232h_i2c{channel=0;dir_bitmask=0x08;val_bitmask=0x08}/pca9548{channel=1;addr=0x77}/pca9548{channel=1;addr=0x77}/pac1934{sensor=1;addr=0x10}",
	"boot_mode", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x0E}",
	"sd_wp", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0x10}",
	"all_port0", gpio, SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=0;pin_bitmask=0xFF}",
	"reset",gpio,"/ft2232h_gpio{channel=1;pin_bitmask=0x01}",
	"testmod_sel",gpio,"/ft2232h_gpio{channel=1;pin_bitmask=0x02}",
	"onoff",gpio,"/ft2232h_gpio{channel=1;pin_bitmask=0x04}",
	"SR_vdd_main",gpio,SIMULATION_GPIO_EXTENDER_PATH"/pca6416a{addr=0x20;port=1;pin_bitmask=0x01}",

	// you put all the pin in the imx8xxl board here
	//"sd_wp", gpio, "..."
	NULL, 0, NULL//null terminated   
};

struct boot_mode simulation_board_boot_modes[]={
	"emmc", 0x02,
	"nand", 0x01,
	"sd", 0x03,
	NULL, 0
};

struct board_info board_list[]=
{
	"simulation_board", simulation_board,simulation_board_boot_modes,
	"imx8xxl", imx8xxl,NULL,
	//"imx9xxl",&imx9xxl_pins,
	
};
int num_of_boards=sizeof(board_list)/sizeof(struct board_info);



struct board_info* get_board(char* board_name)
{
	for(int i=0; i< num_of_boards; i++)
	{

		if(strcmp(board_name, board_list[i].name )==0)//board found
		{
			return &board_list[i]; 
		}
	}
	printf("board model %s is not supported\n", board_name);
	return NULL;
}

int get_path(char* path, char* gpio_name, struct board_info* board)
{
	int i=0;
	while(board->mappings[i].name!=NULL)
	{
		if(strcmp(gpio_name,board->mappings[i].name)==0)
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
get the maximum length of the power related variable name
*/
int get_max_power_name_length(struct board_info* board)
{
	int max=0;
	int i=0;
	while(board->mappings[i].name!=NULL)
	{
		if(board->mappings[i].type==power&&strlen(board->mappings[i].name)>max)
		{
			max=(int)strlen(board->mappings[i].name);
		}
		i++;
	}
	return max;
}

int get_boot_mode_offset(unsigned char boot_mode_pin_bitmask)
{
	int offset=0;
	unsigned char hex=boot_mode_pin_bitmask;
	while(hex%2==0)
	{
		offset++;
		hex=hex>>1;
	}
	if(offset>8)
	{
		printf("get_boot_mode_offset: something is wrong with the pin bitmask %x\n", boot_mode_pin_bitmask );
		return -1;
	}
	//printf("offset is %d\n", offset);
	return offset;

}
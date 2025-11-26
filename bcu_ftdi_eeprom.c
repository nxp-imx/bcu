/*
* Copyright 2020-2021 NXP.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bcu_ftdi_eeprom.h"

struct ftdi_eeprom_field_code_tb ftdi_eeprom_field[] = {
	{ftdi_eeprom_board_id, 0x1, "NXP i.MX8DXL EVK Board"},
	{ftdi_eeprom_board_id, 0x2, "NXP i.MX8DXL EVK DDR3 Board"},
	{ftdi_eeprom_board_id, 0x3, "NXP i.MX8MP EVK Board"},
	{ftdi_eeprom_board_id, 0x4, "NXP i.MX8MP EVK PWR Board"},
	{ftdi_eeprom_board_id, 0x5, "NXP i.MX8MP DDR3L Board"},
	{ftdi_eeprom_board_id, 0x6, "NXP i.MX8MP DDR4 Board"},
	{ftdi_eeprom_board_id, 0x7, "NXP i.MX8ULP EVK Board"},
	{ftdi_eeprom_board_id, 0x8, "NXP VAL_BOARD_1 Board"},
	{ftdi_eeprom_board_id, 0x9, "NXP VAL_BOARD_2 Board"},
	{ftdi_eeprom_board_id, 0xa, "NXP i.MX8ULP EVK9 Board"},
	{ftdi_eeprom_board_id, 0xb, "NXP i.MX8ULP Watch VAL Board"},
	{ftdi_eeprom_board_id, 0xc, "NXP i.MX93 11x11 EVK Board"},
	{ftdi_eeprom_board_id, 0xd, "NXP VAL_BOARD_3 Board"},
	{ftdi_eeprom_board_id, 0xe, "NXP i.MX8DXL Orange Box"},
	{ftdi_eeprom_board_id, 0xf, "NXP i.MX95 EVK 19x19 Board"},
	{ftdi_eeprom_board_id, 0x10,"NXP Custom Board"},
	{ftdi_eeprom_board_id, 0x11,"NXP VAL_BOARD_4 Board"},
	{ftdi_eeprom_board_id, 0x12,"NXP BENCH_IMX8QM Board"},
	{ftdi_eeprom_board_id, 0x13,"NXP BENCH_IMX8QXP Board"},
	{ftdi_eeprom_board_id, 0x14,"NXP BENCH_IMX8MQ Board"},
	{ftdi_eeprom_board_id, 0x15,"NXP BENCH_MCU Board"},
	{ftdi_eeprom_board_id, 0x16,"NXP Custom Board RevB"},
	{ftdi_eeprom_board_id, 0x17,"NXP BENCH_IMX8QM_RevB Board"},
	{ftdi_eeprom_board_id, 0x18,"NXP BENCH_IMX8QXP_RevB Board"},
	{ftdi_eeprom_board_id, 0x19, "NXP i.MX91 QSB Board"},
	{ftdi_eeprom_board_id, 0x1a, "NXP i.MX95 EVK 15x15 Board"},
	{ftdi_eeprom_board_id, 0x1b, "NXP i.MX91 11x11 EVK Board"},
	{ftdi_eeprom_board_id, 0x1c, "NXP i.MX93 14x14 EVK Board"},
	{ftdi_eeprom_board_id, 0x1d,"NXP VAL_BOARD_5 Board"},
	{ftdi_eeprom_board_id, 0x1e,"NXP VAL_BOARD_6 Board"},
	{ftdi_eeprom_board_id, 0x1f, "NXP VAL_BOARD_7 Board"},
	{ftdi_eeprom_board_id, 0x20,"NXP BENCH_IMX6ULL Board"},
	{ftdi_eeprom_board_id, 0x21,"NXP VAL_BOARD_8 Board"},
	{ftdi_eeprom_board_id, 0x22,"NXP VAL_BOARD_9 Board"},
	{ftdi_eeprom_board_id, 0x23,"NXP VAL_BOARD_10 Board"},
	{ftdi_eeprom_board_id, 0x24,"NXP i.MX943 19x19 EVK Board"},
	{ftdi_eeprom_board_id, 0x25,"NXP VAL_BOARD_11 Board"},
	{ftdi_eeprom_board_id, 0x26,"NXP VAL_BOARD_12 Board"},
	{ftdi_eeprom_board_id, 0x27,"NXP i.MX943 Orange Box Board"},
	{ftdi_eeprom_board_id, 0x28,"NXP i.MX93W EVK Board"},
	{ftdi_eeprom_board_id, 0x29,"NXP i.MX952 EVK 19x19 Board"},
	{ftdi_eeprom_board_id, 0x2a,"NXP i.MX952 EVK 15x15 Board"},

	{ftdi_eeprom_soc_id, 0x1, "i.MX8DXL"},
	{ftdi_eeprom_soc_id, 0x2, "i.MX8MP"},
	{ftdi_eeprom_soc_id, 0x3, "i.MX8ULP"},
	{ftdi_eeprom_soc_id, 0x4, "VAL_BOARD_1"},
	{ftdi_eeprom_soc_id, 0x5, "VAL_BOARD_2"},
	{ftdi_eeprom_soc_id, 0x6, "i.MX93"},
	{ftdi_eeprom_soc_id, 0x7, "i.MX95"},
	{ftdi_eeprom_soc_id, 0x8, "NXP Custom SOC"},
	{ftdi_eeprom_soc_id, 0x9, "VAL_BOARD_4 SOC"},
	{ftdi_eeprom_soc_id, 0xa, "BENCH_IMX8QM SOC"},
	{ftdi_eeprom_soc_id, 0xb, "BENCH_IMX8QXP SOC"},
	{ftdi_eeprom_soc_id, 0xc, "BENCH_IMX8MQ SOC"},
	{ftdi_eeprom_soc_id, 0xd, "BENCH_MCU SOC"},
	{ftdi_eeprom_soc_id, 0xe, "NXP Custom RevB SOC"},
	{ftdi_eeprom_soc_id, 0xf, "BENCH_IMX8QM_RevB SOC"},
	{ftdi_eeprom_soc_id, 0x10, "BENCH_IMX8QXP_RevB SOC"},
	{ftdi_eeprom_soc_id, 0x11, "i.MX91"},
	{ftdi_eeprom_soc_id, 0x12, "VAL_BOARD_5 SOC"},
	{ftdi_eeprom_soc_id, 0x13, "VAL_BOARD_6 SOC"},
	{ftdi_eeprom_soc_id, 0x14, "i.MX943"},
	{ftdi_eeprom_soc_id, 0x15, "BENCH_IMX6ULL SOC"},
	{ftdi_eeprom_soc_id, 0x16, "VAL_BOARD_8 SOC"},
	{ftdi_eeprom_soc_id, 0x17, "VAL_BOARD_9 SOC"},
	{ftdi_eeprom_soc_id, 0x18, "VAL_BOARD_10 SOC"},
	{ftdi_eeprom_soc_id, 0x19, "VAL_BOARD_11 SOC"},
	{ftdi_eeprom_soc_id, 0x1a, "VAL_BOARD_12 SOC"},
	{ftdi_eeprom_soc_id, 0x1b, "i.MX93W"},
	{ftdi_eeprom_soc_id, 0x1c, "i.MX952"},

	{ftdi_eeprom_pmic_id, 0x1, "PPF7100BVMA1ES"},  //On imx8dxl-evk
	{ftdi_eeprom_pmic_id, 0x2, "PCA9450CHN"},  //On imx8mpevk-cpu and imx8mpevk-cpu-pwr
	{ftdi_eeprom_pmic_id, 0x3, "PPF7100BMMA2ES"},
	{ftdi_eeprom_pmic_id, 0x4, "PCA9460AUK"},  //On imx8ulp-evk a0
	{ftdi_eeprom_pmic_id, 0x5, "PCA9450AAHN"},  //On val_board_1 a0
	{ftdi_eeprom_pmic_id, 0x6, "PCA9450BHN"},  //On val_board_2 a0
	{ftdi_eeprom_pmic_id, 0x7, "PCA9460B"},  //On imx8ulp watch val
	{ftdi_eeprom_pmic_id, 0x8, "PCA9451AHN"},  //On imx93 evk
	{ftdi_eeprom_pmic_id, 0x9, "PPF0900AMBA1ES"},  //On imx95 evk 19x19
	{ftdi_eeprom_pmic_id, 0xa, "NXP Custom PMIC"},  //
	{ftdi_eeprom_pmic_id, 0xb, "VAL_BOARD_4 PMIC"},  //
	{ftdi_eeprom_pmic_id, 0xc, "BENCH_IMX8QM PMIC"},  //
	{ftdi_eeprom_pmic_id, 0xd, "BENCH_IMX8QXP PMIC"},  //
	{ftdi_eeprom_pmic_id, 0xe, "BENCH_IMX8MQ PMIC"},  //
	{ftdi_eeprom_pmic_id, 0xf, "BENCH_MCU PMIC"},  //
	{ftdi_eeprom_pmic_id, 0x10, "NXP Custom RevB PMIC"},  //
	{ftdi_eeprom_pmic_id, 0x11, "BENCH_IMX8QM_RevB PMIC"},  //
	{ftdi_eeprom_pmic_id, 0x12, "BENCH_IMX8QXP_RevB PMIC"},  //
	{ftdi_eeprom_pmic_id, 0x13, "PF9453AHN"},  //On imx91 qsb
	{ftdi_eeprom_pmic_id, 0x14, "PPF0900AVNA2ES"},  //On imx95 evk 15x15
	{ftdi_eeprom_pmic_id, 0x15, "PCA9452"},  //On imx93 14x14 evk
	{ftdi_eeprom_pmic_id, 0x16, "VAL_BOARD_5 PMIC"},
	{ftdi_eeprom_pmic_id, 0x17, "VAL_BOARD_6 PMIC"},
	{ftdi_eeprom_pmic_id, 0x18, "MFS5600"},  //On val_board_7
	{ftdi_eeprom_pmic_id, 0x19, "BENCH_IMX6ULL PMIC"},
	{ftdi_eeprom_pmic_id, 0x20, "VAL_BOARD_8 PMIC"},
	{ftdi_eeprom_pmic_id, 0x21, "VAL_BOARD_9 PMIC"},
	{ftdi_eeprom_pmic_id, 0x22, "VAL_BOARD_10 PMIC"},
	{ftdi_eeprom_pmic_id, 0x23, "VAL_BOARD_11 PMIC"},
	{ftdi_eeprom_pmic_id, 0x24, "VAL_BOARD_12 PMIC"},

	{0, 0, NULL}//null terminated
};

struct ftdi_eeprom_user_area imx8ulp_ftdi_eeprom_user_area_info = {
	0x1,
	0x7, 0x0, 0x0,
	0x3, 0x0, 0x0,
	0x4, 0xF, 0xF,
	25,
	1
};

struct ftdi_eeprom_user_area imx8dxlevk_ftdi_eeprom_user_area_info = {
	0x1,
	0x1, 0x0, 0x0,
	0x1, 0x1, 0x0,
	0x1, 0xF, 0xF,
	24,
	1
};

struct ftdi_eeprom_user_area imx8dxlevk_c1_ftdi_eeprom_user_area_info = {
	0x1,
	0x1, 0x1, 0x2,
	0x1, 0x1, 0x0,
	0x1, 0xF, 0xF,
	24,
	1
};

struct ftdi_eeprom_user_area imx8mpevkpwr_a0_ftdi_eeprom_user_area_info = {
	0x1,
	0x4, 0x0, 0x0,
	0x2, 0x1, 0x0,
	0x2, 0xF, 0xF,
	27,
	1
};

struct ftdi_eeprom_user_area imx8mpevkpwr_a1_ftdi_eeprom_user_area_info = {
	0x1,
	0x4, 0x1, 0x0,
	0x2, 0x1, 0x0,
	0x2, 0xF, 0xF,
	27,
	1
};

struct ftdi_eeprom_user_area imx8ulpevkb2_ftdi_eeprom_user_area_info = {
	0x1,
	0x7, 0x2, 0x1,
	0x3, 0x0, 0x0,
	0x4, 0xF, 0xF,
	15,
	1
};

struct ftdi_eeprom_user_area imx8ulpevk9_ftdi_eeprom_user_area_info = {
	0x1,
	0xa, 0x0, 0x0,
	0x3, 0x0, 0x0,
	0x4, 0xF, 0xF,
	15,
	1
};

struct ftdi_eeprom_user_area imx8ulpwatchval_ftdi_eeprom_user_area_info = {
	0x1,
	0xb, 0x0, 0x0,
	0x3, 0x0, 0x0,
	0x7, 0xF, 0xF,
	20,
	1
};

struct ftdi_eeprom_user_area val_board_1_ftdi_eeprom_user_area_info = {
	0x1,
	0x8, 0x0, 0x0,
	0x4, 0x0, 0x0,
	0x5, 0xF, 0xF,
	37,
	1
};

struct ftdi_eeprom_user_area val_board_2_ftdi_eeprom_user_area_info = {
	0x1,
	0x9, 0x1, 0x0,
	0x5, 0x0, 0x0,
	0x6, 0xF, 0xF,
	34,
	1
};

struct ftdi_eeprom_user_area imx91qsb_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x19, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x11, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x13, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	13,
	1
};

struct ftdi_eeprom_user_area imx91evk11_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1b, 0x0, 0x0, //board_id, board_rev_number, board_rev_char
	0x11, 0x0, 0x0,  //soc_id, soc_rev_number, soc_rev_char
	0x8,  0xF, 0xF,  //pmic_id, pmic_rev_number, pmic_rev_char
	21,
	1
};

struct ftdi_eeprom_user_area imx93evk11_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0xc, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x6, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x8, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	21,
	1
};

struct ftdi_eeprom_user_area imx93wevk_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x28, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x1b, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x8, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	21,
	1
};

struct ftdi_eeprom_user_area imx93evk11b1_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0xc, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x6, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x8, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	21,
	1
};

struct ftdi_eeprom_user_area imx93evk14_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1c, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x6,  0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x15, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	4,
	1
};

struct ftdi_eeprom_user_area val_board_3_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0xd, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x6, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x8, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	19,
	1
};

struct ftdi_eeprom_user_area imx8dxl_obx_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0xe, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x1, 0x1, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x1, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	0,
	1
};

struct ftdi_eeprom_user_area imx95evk19_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0xf, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x7, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x9, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,
	1
};

struct ftdi_eeprom_user_area imx952evk19_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x29, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x1c, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x9, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,
	1
};

struct ftdi_eeprom_user_area imx952evk15_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x2a, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x1c, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x9, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,
	1
};

struct ftdi_eeprom_user_area nxp_custom_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x10, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x8, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xa, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	10,				//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_4_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x11, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x9, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xb, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	7,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx8qm_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x12, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xa, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xc, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	10,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx8qxp_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x13, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xb, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xd, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx8mq_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x14, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xc, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xe, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_mcu_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x15, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xd, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0xf, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,				//no of power rails
	1
};

struct ftdi_eeprom_user_area nxp_custom_revB_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x16, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xe, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x10, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx8qm_revB_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x17, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0xf, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x11, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	10,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx8qxp_revB_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x18, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x10, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x12, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,				//no of power rails
	1
};

struct ftdi_eeprom_user_area imx95evk15_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1a, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x7,  0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x14, 0xF, 0xF,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,
	1
};

struct ftdi_eeprom_user_area val_board_5_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1d, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x12, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x16, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_6_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1e, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x13, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x17, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	6,		//no of power rails
	1
};

struct ftdi_eeprom_user_area imx943evk19_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x24, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x14, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x18, 0xf, 0xf,	//pmic_id, pmic_rev_number, pmic_rev_char
	19,		//no of power rails
	1
};

struct ftdi_eeprom_user_area imx943evk19b1_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x24, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x14, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x18, 0xf, 0xf,	//pmic_id, pmic_rev_number, pmic_rev_char
	19,		//no of power rails
	1
};

struct ftdi_eeprom_user_area imx943obx_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x27, 0x1, 0x0,	//board_id, board_rev_number, board_rev_char
	0x14, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x18, 0xf, 0xf,	//pmic_id, pmic_rev_number, pmic_rev_char
	15,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_7_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x1f, 0x0, 0x0,	//board_id, board_rev_number, board_rev_char
	0x14, 0x0, 0x0,	//soc_id, soc_rev_number, soc_rev_char
	0x18, 0xf, 0xf,	//pmic_id, pmic_rev_number, pmic_rev_char
	7,				//no of power rails
	1
};

struct ftdi_eeprom_user_area bench_imx6ull_revB_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x20, 0x1, 0x1,	//board_id, board_rev_number, board_rev_char
	0x15, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x19, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	8,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_8_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x21, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x16, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x20, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	7,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_9_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x22, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x17, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x21, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	10,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_10_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x23, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x18, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x22, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	0,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_11_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x25, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x19, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x23, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	16,		//no of power rails
	1
};

struct ftdi_eeprom_user_area val_board_12_ftdi_eeprom_user_area_info = {
	0x1,		//config
	0x26, 0x3, 0xa,	//board_id, board_rev_number, board_rev_char
	0x1a, 0x1, 0x1,	//soc_id, soc_rev_number, soc_rev_char
	0x24, 0x1, 0x1,	//pmic_id, pmic_rev_number, pmic_rev_char
	11,		//no of power rails
	1
};

void data_add_1(struct ftdi_eeprom_user_area* data)
{
	data->board_id += 0x40;
	data->board_rev_c += 1;
	data->board_rev_n += 1;
	data->soc_rev_c += 1;
	data->soc_rev_n += 1;
	if (data->pmic_rev_c != 0xF)
		data->pmic_rev_c += 1;
	if (data->pmic_rev_n != 0xF)
		data->pmic_rev_n += 1;
	data->nbr_pwr_rails += 1;
	data->sn += 0x0101;
}

void data_minus_1(struct ftdi_eeprom_user_area* data)
{
	data->board_id -= 0x40;
	data->board_rev_c -= 1;
	data->board_rev_n -= 1;
	data->soc_rev_c -= 1;
	data->soc_rev_n -= 1;
	if (data->pmic_rev_c != 0xF)
		data->pmic_rev_c -= 1;
	if (data->pmic_rev_n != 0xF)
		data->pmic_rev_n -= 1;
	data->nbr_pwr_rails -= 1;
	data->sn -= 0x0101;
}

char* get_code_definition(int field, int code_idx)
{
	int i = 0;
	while (ftdi_eeprom_field[i].definition != NULL)
	{
		if (ftdi_eeprom_field[i].field == field &&
			ftdi_eeprom_field[i].code == code_idx)
		{
			return ftdi_eeprom_field[i].definition;
		}
		i++;
	}

	return NULL;
}

void bcu_random_sn(unsigned char* sn, int len)
{
	int i;

#ifdef _WIN32
	srand(GetTickCount64());
#else
	srand((unsigned)(clock()));
#endif
	for (i = 0; i < len; i++)
	{
		sprintf(&sn[i], "%1X", rand() % 16);
	}
	sn[i] = 0;
}

int bcu_ftdi_eeprom_erase(struct eeprom_device* eeprom)
{
	return eeprom->eeprom_erase(eeprom);
}

int bcu_ftdi_eeprom_read(struct eeprom_device* eeprom, unsigned int read_idx, void* read_buf)
{
	struct ftdi_eeprom_data eeprom_data;
	struct ftdi_eeprom_user_area* eeprom_ua_data = (struct ftdi_eeprom_user_area*)eeprom_data.ua_data;
	int ret = 0, temp = 0;

	((char *)read_buf)[0] = 0;

	ret = eeprom->eeprom_read(eeprom, eeprom_data.ua_data, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, eeprom_data.ftdi_sn);
	if (ret < 0)
		return ret;

	data_minus_1(eeprom_ua_data);

	if (eeprom_ua_data->config_flag == 0x0 || eeprom_ua_data->config_flag == 0x3)
	{
		printf("Invalid EEPROM context, use -w option to write the default values.\n");
		return -1;
	}

	switch (read_idx)
	{
	case ftdi_eeprom_config_flag:
		temp = eeprom_ua_data->config_flag;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_board_id:
		strcpy(read_buf, get_code_definition(ftdi_eeprom_board_id, eeprom_ua_data->board_id));
		break;
	case ftdi_eeprom_board_rev:
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->board_rev_c;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->board_rev_n;
		break;
	case ftdi_eeprom_soc_id:
		strcpy(read_buf, get_code_definition(ftdi_eeprom_soc_id, eeprom_ua_data->soc_id));
		break;
	case ftdi_eeprom_soc_rev:
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->soc_rev_c;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->soc_rev_n;
		break;
	case ftdi_eeprom_pmic_id:
		strcpy(read_buf, get_code_definition(ftdi_eeprom_pmic_id, eeprom_ua_data->pmic_id));
		break;
	case ftdi_eeprom_pmic_rev:
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->pmic_rev_c;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->pmic_rev_n;
		break;
	case ftdi_eeprom_nbr_pwr_rails:
		temp = eeprom_ua_data->nbr_pwr_rails;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_sn:
		temp = eeprom_ua_data->sn;
		memcpy(read_buf, &temp, sizeof(short));
		break;
	default:
		break;
	}

	return ret;
}

int bcu_ftdi_eeprom_read_code(struct eeprom_device* eeprom, unsigned int read_idx, void* read_buf)
{
	struct ftdi_eeprom_data eeprom_data;
	struct ftdi_eeprom_user_area* eeprom_ua_data = (struct ftdi_eeprom_user_area*)eeprom_data.ua_data;
	int ret = 0, temp = 0;

	((char *)read_buf)[0] = 0;

	ret = eeprom->eeprom_read(eeprom, eeprom_data.ua_data, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, eeprom_data.ftdi_sn);
	if (ret < 0)
		return ret;

	data_minus_1(eeprom_ua_data);

	if (eeprom_ua_data->config_flag == 0x0 || eeprom_ua_data->config_flag == 0x3)
	{
		printf("Invalid EEPROM context, use -w option to write the default values.\n");
		return -1;
	}

	switch (read_idx)
	{
	case ftdi_eeprom_config_flag:
		temp = eeprom_ua_data->config_flag;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_board_id:
		temp = eeprom_ua_data->board_id;
		memcpy(read_buf, &temp, sizeof(short));
		break;
	case ftdi_eeprom_board_rev:
		((char *)read_buf)[0] = eeprom_ua_data->board_rev_c;
		((char *)read_buf)[1] = eeprom_ua_data->board_rev_n;
		break;
	case ftdi_eeprom_soc_id:
		temp = eeprom_ua_data->soc_id;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_soc_rev:
		((char *)read_buf)[0] = eeprom_ua_data->soc_rev_c;
		((char *)read_buf)[1] = eeprom_ua_data->soc_rev_n;
		break;
	case ftdi_eeprom_pmic_id:
		temp = eeprom_ua_data->pmic_id;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_pmic_rev:
		((char *)read_buf)[0] = eeprom_ua_data->pmic_rev_c;
		((char *)read_buf)[1] = eeprom_ua_data->pmic_rev_n;
		break;
	case ftdi_eeprom_nbr_pwr_rails:
		temp = eeprom_ua_data->nbr_pwr_rails;
		memcpy(read_buf, &temp, sizeof(char));
		break;
	case ftdi_eeprom_sn:
		temp = eeprom_ua_data->sn;
		memcpy(read_buf, &temp, sizeof(short));
		break;
	default:
		break;
	}

	return ret;
}

int bcu_ftdi_eeprom_print(struct eeprom_device* eeprom)
{
	struct ftdi_eeprom_data eeprom_data;
	struct ftdi_eeprom_user_area* eeprom_ua_data = (struct ftdi_eeprom_user_area*)eeprom_data.ua_data;
	int ret = 0;
	char rev[3] = {0};

	ret = eeprom->eeprom_read(eeprom, eeprom_data.ua_data, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, eeprom_data.ftdi_sn);
	if (ret < 0)
		return ret;

	data_minus_1(eeprom_ua_data);

	// for (int i = 0; i <10; i++)
	// 	printf("0x%x ", eeprom_data.ua_data[i]);
	// printf("\n");

	if (eeprom_ua_data->config_flag == 0x0 || eeprom_ua_data->config_flag == 0x3)
	{
		printf("Invalid EEPROM context, use -w option to write the default values.\n");
		return -1;
	}

	if (eeprom->device.parent == NULL)
		printf("\nFTDI EEPROM SN: %s\n", eeprom_data.ftdi_sn);
	else
		printf("\n");

	rev[0] = 'A' + eeprom_ua_data->board_rev_c;
	rev[1] = '0' + eeprom_ua_data->board_rev_n;
	printf("Board Info: %s Rev %s\n", get_code_definition(ftdi_eeprom_board_id, eeprom_ua_data->board_id), rev);

	rev[0] = 'A' + eeprom_ua_data->soc_rev_c;
	rev[1] = '0' + eeprom_ua_data->soc_rev_n;
	printf("  SoC Info: %s Rev %s\n", get_code_definition(ftdi_eeprom_soc_id, eeprom_ua_data->soc_id), rev);

	printf(" PMIC Info: %s", get_code_definition(ftdi_eeprom_pmic_id, eeprom_ua_data->pmic_id));
	if (eeprom_ua_data->pmic_rev_c != 0xF && eeprom_ua_data->pmic_rev_n != 0xF)
	{
		rev[0] = 'A' + eeprom_ua_data->pmic_rev_c;
		rev[1] = '0' + eeprom_ua_data->pmic_rev_n;
		printf(" Rev %s\n", rev);
	}
	else
	{
		rev[0] = 0;
		printf("\n");
	}

	printf("Number of available power rails: %d\n", eeprom_ua_data->nbr_pwr_rails);
	printf("Serial Number: %d\n\n", eeprom_ua_data->sn);

	return 0;
}

int bcu_ftdi_eeprom_write_default_sn(struct eeprom_device* eeprom, struct ftdi_eeprom_user_area* ua_data, unsigned char* ftdi_sn)
{
	int ret = 0;
	unsigned char* wbuf = (unsigned char*)ua_data;

	data_add_1(ua_data);

	ret = eeprom->eeprom_write(eeprom, wbuf, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, ftdi_sn);

	return ret;
}

int bcu_ftdi_eeprom_write_default(struct eeprom_device* eeprom, struct ftdi_eeprom_user_area* ua_data)
{
	int ret = 0;
	unsigned char sn[BCU_FTDI_EEPROM_SN_LEN] = {0};

	bcu_random_sn(sn, BCU_FTDI_EEPROM_SN_LEN - 1);

	ret = bcu_ftdi_eeprom_write_default_sn(eeprom, ua_data, sn);

	return ret;
}

int bcu_ftdi_eeprom_write(struct eeprom_device* eeprom, unsigned int write_idx, void* write_buf)
{
	struct ftdi_eeprom_data eeprom_data;
	struct ftdi_eeprom_user_area* eeprom_ua_data = (struct ftdi_eeprom_user_area*)eeprom_data.ua_data;
	int ret = 0, temp = 0;

	ret = eeprom->eeprom_read(eeprom, eeprom_data.ua_data, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, eeprom_data.ftdi_sn);
	if (ret < 0)
		return ret;

	data_minus_1(eeprom_ua_data);

	if (eeprom_ua_data->config_flag == 0x0 || eeprom_ua_data->config_flag == 0x3)
	{
		printf("Invalid EEPROM context, use -w option to write the default values.\n");
		return -1;
	}

	switch (write_idx)
	{
	case ftdi_eeprom_config_flag:
		eeprom_ua_data->config_flag = 0x1;
		break;
	case ftdi_eeprom_board_id:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->board_id = temp;
		break;
	case ftdi_eeprom_board_rev:
		eeprom_ua_data->board_rev_c = ((char *)write_buf)[0] - 'A';
		eeprom_ua_data->board_rev_n = ((char *)write_buf)[1] - '0';
		break;
	case ftdi_eeprom_soc_id:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->soc_id = temp;
		break;
	case ftdi_eeprom_soc_rev:
		eeprom_ua_data->soc_rev_c = ((char *)write_buf)[0] - 'A';
		eeprom_ua_data->soc_rev_n = ((char *)write_buf)[1] - '0';
		break;
	case ftdi_eeprom_pmic_id:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->pmic_id = temp;
		break;
	case ftdi_eeprom_pmic_rev:
		eeprom_ua_data->pmic_rev_c = ((char *)write_buf)[0] - 'A';
		eeprom_ua_data->pmic_rev_n = ((char *)write_buf)[1] - '0';
		break;
	case ftdi_eeprom_nbr_pwr_rails:
		memcpy(&temp, write_buf, sizeof(char));
		eeprom_ua_data->nbr_pwr_rails = temp;
		break;
	case ftdi_eeprom_sn:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->sn = temp;
		break;
	default:
		break;
	}

	data_add_1(eeprom_ua_data);

	ret = eeprom->eeprom_write(eeprom, eeprom_data.ua_data, BCU_FTDI_EEPROM_UAREA_SADDR, BCU_FTDI_EEPROM_USE_LEN, eeprom_data.ftdi_sn);

	return ret;
}

int bcu_ftdi_eeprom_update_usr_sn(struct eeprom_device* eeprom, unsigned short ua_sn)
{
	return bcu_ftdi_eeprom_write(eeprom, ftdi_eeprom_sn, &ua_sn);
}

int bcu_ftdi_eeprom_update_board_rev(struct eeprom_device* eeprom, unsigned char* ua_sn)
{
	return bcu_ftdi_eeprom_write(eeprom, ftdi_eeprom_board_rev, ua_sn);
}

int bcu_ftdi_eeprom_update_soc_rev(struct eeprom_device* eeprom, unsigned char* ua_sn)
{
	return bcu_ftdi_eeprom_write(eeprom, ftdi_eeprom_soc_rev, ua_sn);
}

int bcu_ftdi_eeprom_update_pmic_rev(struct eeprom_device* eeprom, unsigned char* ua_sn)
{
	return bcu_ftdi_eeprom_write(eeprom, ftdi_eeprom_pmic_rev, ua_sn);
}

int bcu_eeprom_checkboard(struct eeprom_device* eeprom, struct ftdi_eeprom_user_area* eeprom_data)
{
	unsigned short board_id;
	unsigned char board_rev[3];
	int status = 0;

	status = bcu_ftdi_eeprom_read_code(eeprom, ftdi_eeprom_board_id, &board_id);
	if (status)
		return status;

	if (board_id != eeprom_data->board_id)
		return -1;

	status = bcu_ftdi_eeprom_read_code(eeprom, ftdi_eeprom_board_rev, board_rev);
	if (status)
		return status;

	if (board_rev[0] != eeprom_data->board_rev_c ||
	    board_rev[1] != eeprom_data->board_rev_n)
		return -11;

	return 0;
}

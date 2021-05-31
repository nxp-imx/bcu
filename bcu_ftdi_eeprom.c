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
	{ftdi_eeprom_board_id, 0x8, "NXP VAL 1 Board"},

	{ftdi_eeprom_soc_id, 0x1, "i.MX8DXL"},
	{ftdi_eeprom_soc_id, 0x2, "i.MX8MP"},
	{ftdi_eeprom_soc_id, 0x3, "i.MX8ULP"},
	{ftdi_eeprom_soc_id, 0x4, "VAL_1"},

	{ftdi_eeprom_pmic_id, 0x1, "PPF7100BVMA1ES"},  //On imx8dxl-evk
	{ftdi_eeprom_pmic_id, 0x2, "PCA9450CHN"},  //On imx8mpevk-cpu and imx8mpevk-cpu-pwr
	{ftdi_eeprom_pmic_id, 0x3, "PPF7100BMMA2ES"},
	{ftdi_eeprom_pmic_id, 0x4, "PCA9460AUK"},  //On imx8ulp-evk a0
	{ftdi_eeprom_pmic_id, 0x5, "PCA9450AHN"},  //On val_board_1 a0

	{0, 0, NULL}//null terminated
};

struct ftdi_eeprom_user_area imx8ulp_ftdi_eeprom_user_area_info = {
	0x1,
	0x7, 0x1, 0x1,
	0x3, 0x1, 0x1,
	0x4, 0xF, 0xF,
	25,
	1
};

struct ftdi_eeprom_user_area imx8dxlevk_ftdi_eeprom_user_area_info = {
	0x1,
	0x1, 0x1, 0x1,
	0x1, 0x1, 0x2,
	0x1, 0xF, 0xF,
	24,
	1
};

struct ftdi_eeprom_user_area imx8mpevkpwr_a0_ftdi_eeprom_user_area_info = {
	0x1,
	0x4, 0x1, 0x1,
	0x2, 0x1, 0x2,
	0x2, 0xF, 0xF,
	27,
	1
};

struct ftdi_eeprom_user_area imx8mpevkpwr_a1_ftdi_eeprom_user_area_info = {
	0x1,
	0x4, 0x1, 0x2,
	0x2, 0x1, 0x2,
	0x2, 0xF, 0xF,
	27,
	1
};

struct ftdi_eeprom_user_area imx8ulpevk_ftdi_eeprom_user_area_info = {
	0x1,
	0x7, 0x1, 0x1,
	0x3, 0x1, 0x1,
	0x4, 0xF, 0xF,
	15,
	1
};

struct ftdi_eeprom_user_area val_board_1_ftdi_eeprom_user_area_info = {
	0x1,
	0x8, 0x1, 0x1,
	0x4, 0x1, 0x1,
	0x5, 0xF, 0xF,
	33,
	1
};

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
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->board_rev_c - 1;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->board_rev_n - 1;
		break;
	case ftdi_eeprom_soc_id:
		strcpy(read_buf, get_code_definition(ftdi_eeprom_soc_id, eeprom_ua_data->soc_id));
		break;
	case ftdi_eeprom_soc_rev:
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->soc_rev_c - 1;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->soc_rev_n - 1;
		break;
	case ftdi_eeprom_pmic_id:
		strcpy(read_buf, get_code_definition(ftdi_eeprom_pmic_id, eeprom_ua_data->pmic_id));
		break;
	case ftdi_eeprom_pmic_rev:
		((char *)read_buf)[0] = 'A' + eeprom_ua_data->pmic_rev_c - 1;
		((char *)read_buf)[1] = '0' + eeprom_ua_data->pmic_rev_n - 1;
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

	rev[0] = 'A' + eeprom_ua_data->board_rev_c - 1;
	rev[1] = '0' + eeprom_ua_data->board_rev_n - 1;
	printf("Board Info: %s Rev %s\n", get_code_definition(ftdi_eeprom_board_id, eeprom_ua_data->board_id), rev);

	rev[0] = 'A' + eeprom_ua_data->soc_rev_c - 1;
	rev[1] = '0' + eeprom_ua_data->soc_rev_n - 1;
	printf("  SoC Info: %s Rev %s\n", get_code_definition(ftdi_eeprom_soc_id, eeprom_ua_data->soc_id), rev);

	printf(" PMIC Info: %s", get_code_definition(ftdi_eeprom_pmic_id, eeprom_ua_data->pmic_id));
	if (eeprom_ua_data->pmic_rev_c != 0xF && eeprom_ua_data->pmic_rev_n != 0xF)
	{
		rev[0] = 'A' + eeprom_ua_data->pmic_rev_c - 1;
		rev[1] = '0' + eeprom_ua_data->pmic_rev_n - 1;
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
		eeprom_ua_data->board_rev_c = ((char *)write_buf)[0] - 'A' + 1;
		eeprom_ua_data->board_rev_n = ((char *)write_buf)[1] - '0' + 1;
		break;
	case ftdi_eeprom_soc_id:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->soc_id = temp;
		break;
	case ftdi_eeprom_soc_rev:
		eeprom_ua_data->soc_rev_c = ((char *)write_buf)[0] - 'A' + 1;
		eeprom_ua_data->soc_rev_n = ((char *)write_buf)[1] - '0' + 1;
		break;
	case ftdi_eeprom_pmic_id:
		memcpy(&temp, write_buf, sizeof(short));
		eeprom_ua_data->pmic_id = temp;
		break;
	case ftdi_eeprom_pmic_rev:
		eeprom_ua_data->pmic_rev_c = ((char *)write_buf)[0] - 'A' + 1;
		eeprom_ua_data->pmic_rev_n = ((char *)write_buf)[1] - '0' + 1;
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
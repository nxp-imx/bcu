/*
* Copyright 2020 NXP.
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
#ifndef BCU_FTDI_EEPROM_H
#define BCU_FTDI_EEPROM_H

#include "chip.h"

#define BCU_FTDI_EEPROM_UAREA_SADDR (0x1A)
#define BCU_FTDI_EEPROM_USE_LEN (10)
#define BCU_FTDI_EEPROM_SN_LEN (7)

enum ftdi_eeprom_field {
	ftdi_eeprom_config_flag,
	ftdi_eeprom_board_id,
	ftdi_eeprom_board_rev,
	ftdi_eeprom_soc_id,
	ftdi_eeprom_soc_rev,
	ftdi_eeprom_pmic_id,
	ftdi_eeprom_pmic_rev,
	ftdi_eeprom_nbr_pwr_rails,
	ftdi_eeprom_sn
};

struct ftdi_eeprom_field_code_tb {
	int field;
	unsigned int code;
	char* definition;
};

struct ftdi_eeprom_user_area
{
	unsigned short config_flag : 2,
		       board_id : 14;
	unsigned char  board_rev_n : 4,
		       board_rev_c : 4;
	unsigned char  soc_id;
	unsigned char  soc_rev_n : 4,
		       soc_rev_c : 4;
	unsigned char  pmic_id;
	unsigned char  pmic_rev_n : 4,
		       pmic_rev_c : 4;
	unsigned char  nbr_pwr_rails;
	unsigned short sn;
};

struct ftdi_eeprom_data
{
	unsigned char ftdi_sn[BCU_FTDI_EEPROM_SN_LEN];
	unsigned char ua_data[BCU_FTDI_EEPROM_USE_LEN];
};

int bcu_ftdi_eeprom_erase(struct eeprom_device* eeprom);
int bcu_ftdi_eeprom_read(struct eeprom_device* eeprom, unsigned int read_idx, void* read_buf);
int bcu_ftdi_eeprom_print(struct eeprom_device* eeprom);
int bcu_ftdi_eeprom_write(struct eeprom_device* eeprom, unsigned int write_idx, void* write_buf);
int bcu_ftdi_eeprom_write_default(struct eeprom_device* eeprom, struct ftdi_eeprom_user_area* ua_data);
int bcu_ftdi_eeprom_update_usr_sn(struct eeprom_device* eeprom, unsigned short ua_sn);
int bcu_ftdi_eeprom_update_board_rev(struct eeprom_device* eeprom, unsigned char* ua_sn);
int bcu_ftdi_eeprom_update_soc_rev(struct eeprom_device* eeprom, unsigned char* ua_sn);
int bcu_ftdi_eeprom_update_pmic_rev(struct eeprom_device* eeprom, unsigned char* ua_sn);
int bcu_eeprom_checkboard(struct eeprom_device* eeprom, struct ftdi_eeprom_user_area* eeprom_data);

extern struct ftdi_eeprom_user_area imx8ulp_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8dxlevk_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8dxlevk_c1_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8mpevkpwr_a0_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8mpevkpwr_a1_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8ulpevkb2_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8ulpevk9_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8ulpwatchval_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_1_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_2_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx91qsb_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx91evk11_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx93evk11_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx93evk11b1_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx93wevk_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx93evk14_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_3_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx8dxl_obx_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx95evk19_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx95evk15_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx952evk19_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx952evk15_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area nxp_custom_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_4_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx8qm_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx8qxp_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx8mq_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_mcu_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area nxp_custom_revB_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx8qm_revB_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx8qxp_revB_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_5_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_6_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx943evk19_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx943evk19b1_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area imx943obx_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_7_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area bench_imx6ull_revB_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_8_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_9_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_10_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_11_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_user_area val_board_12_ftdi_eeprom_user_area_info;
extern struct ftdi_eeprom_field_code_tb ftdi_eeprom_field[];


#endif //BCU_FTDI_EEPROM_H

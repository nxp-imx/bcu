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
#ifndef PARSER_H
#define PARSER_H

#include "chip.h"
#include "board.h"

#define PARSER_EEPROM_READ_AND_PRINT	1
#define PARSER_EEPROM_READ_TO_FILE	2
#define PARSER_EEPROM_WRITE_DEFAULT	3
#define PARSER_EEPROM_WRITE_FROM_FILE	4
#define PARSER_EEPROM_UPDATE_USER_SN	5
#define PARSER_EEPROM_ERASE		6

#define NO_BOARD_AND_ID	1
#define NO_BOARD	2
#define NO_ID		3
#define NO_USE_AUTO_FIND	4

/*used for storing options specified by user*/
struct options_setting {
	char board[100]; //indicating the model of the board, i.e. i.MX8QMEVK
	int restore;
	int keep_settings;
	int auto_find_board;
	int delay;
	int hold;
	int output_state;
	int boot_mode_hex;
	int boot_config_hex[MAX_BOOT_CONFIG_BYTE];
	int active_low;
	char path[MAX_PATH_LENGTH];
	char gpio_name[MAX_MAPPING_NAME_LENGTH];
	int location_id;
	int temperature;
	int dump;
	char *dumpname;
	int force;
	int pmt;
	int nodisplay;
	int refreshms;
	int use_rms;
	int rangefixed;
	int use_hwfilter;
	int use_bipolar;
	int use_no_skip;
	int dump_statistics;
	int eeprom_function;
	short eeprom_usr_sn;
	unsigned char eeprom_board_rev[3];
	unsigned char eeprom_soc_rev[3];
	int download_doc;
	int download_pre_release;
	char groups[MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
	int init;
	float ptc_temp;
	int ptc_onoff;
	int ptc_sensor;
};

struct group {
	char name[MAX_MAPPING_NAME_LENGTH];
	double sum;
	double max;
	double min;
	double avg;
	int avg_data_size;
	int member_index[MAX_NUMBER_OF_POWER];
	char member_list[MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER];
	int num_of_members;
};

int update_parameter_string(char* path, char* parameter_name, char* str);
int extract_parameter_string(char* chip_specification, char* parameter_name, char* result);
int extract_parameter_value(char* chip_specification, char* parameter_name);
int get_chip_specification_by_chipname(char* path, char* chip_specification, char* chipname);
void get_chip_name(char* chip_specification, char* chip_name);
void free_device_linkedlist_backward(struct device* ptr);
void free_device_linkedlist_forward(struct device* ptr);
void* build_device_linkedlist_forward(void** head, char* path);
void* build_device_linkedlist_smart(void** new_head, char* new_path, void* old_head, char* old_path);

int parse_board_id_options(int argc, char** argv, struct options_setting* setting);
int parse_options(char* cmd, int argc, char** argv, struct options_setting* setting);
void set_options_default(struct options_setting* setting);
int parse_groups(struct group* groups, struct board_info* board);
void groups_init(struct group* groups, int num);
int str_replace(char *str, char *source, char *dest);

int compare_version(const char *v1, const char *v2);

#endif //PARSER_H

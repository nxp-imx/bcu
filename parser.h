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
#ifndef PARSER_H
#define PARSER_H

#include "chip.h"
#include "board.h"

/*used for storing options specified by user*/
struct options_setting {
	char board[100]; //indicating the model of the board, i.e. i.MX8QMEVK
	int delay;
	int hold;
	int output_state;
	int boot_mode_hex;
	int active_low;
	char path[MAX_PATH_LENGTH];
	char gpio_name[MAX_MAPPING_NAME_LENGTH];
	int location_id;
	int dump;
	char dumpname[50];
	int nodisplay;
	int refreshms;
	char groups[MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
};

struct group {
	char name[MAX_MAPPING_NAME_LENGTH];
	float sum;
	float max;
	float min;
	float avg;
	int avg_data_size;
	int member_index[MAX_NUMBER_OF_POWER];
	char member_list[MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER];
	int num_of_members;
};

int extract_parameter_string(char* chip_specification, char* parameter_name, char* result);
int extract_parameter_value(char* chip_specification, char* parameter_name);
void get_chip_name(char* chip_specification, char* chip_name);
void free_device_linkedlist_backward(struct device* ptr);
void free_device_linkedlist_forward(struct device* ptr);
void* build_device_linkedlist_forward(void** head, char* path);
void* build_device_linkedlist_smart(void** new_head, char* new_path, void* old_head, char* old_path);

int parse_board_id_options(int argc, char** argv, struct options_setting* setting);
int parse_options(int argc, char** argv, struct options_setting* setting);
void set_options_default(struct options_setting* setting);
int parse_groups(char* input, struct group* groups, struct board_info* board);
void groups_init(struct group* groups, int num);
#endif //PARSER_H
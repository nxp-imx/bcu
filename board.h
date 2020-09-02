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
#ifndef BOARD_H
#define BOARD_H

#define MAX_PATH_LENGTH 800     //maximum path length allowed
#define MAX_NUMBER_OF_POWER 100 //maximum number of power types
#define MAX_NUMBER_OF_GROUP 10 //maximum number of power groups
#define MAX_MAPPING_NAME_LENGTH 30	//maximum length of the name of the mapping

enum mapping_type {
	power,
	gpio,
	bcu_eeprom
};

/*
to avoid any parsing issue:
-ALL LOWER CASE LETTER
-NO SPACE BETWEEN CHARACTERs
-TO USE HEX AS PARAMETER, PUT '0x' BEFORE THE NUMBER
*/
struct mapping {
	char* name;
	int type;
	char* path;
	unsigned char initinfo; //High 4 bits means initid, Low 4 bits means default output state for this pin
};

struct boot_mode {
	char* name;
	unsigned char boot_mode_hex;
};

struct board_links {
	char* flashbin;
	char* sdcard;
};

struct board_power_group {
	char* group_name;
	char* group_string;
};

struct board_info {
	char* name;
	struct mapping* mappings;
	struct boot_mode* boot_modes;
	struct board_power_group* power_groups;
	struct board_links* links;
};

/*find if there is the gpio_name on the board*/
int have_gpio(char* gpio_name, struct board_info* board);
struct board_info* get_board_by_id(int id);
int get_board_numer(void);
/*given the name of the board, return coresponding board info struct*/
struct board_info* get_board(char* board_name);
/*given board_info, and the desired variable name, find the coresponding path of the variable*/
int get_path(char* path, char* item_name, struct board_info* board);
/*set path of the item*/
int set_path(char* path, char* item_name, struct board_info* board);
/*get the item location by item_name*/
int get_item_location(char* item_name, struct board_info* board);
/*given board_info, and the initid, find the coresponding gpio name, path and output state of the variable*/
int get_gpio_info_by_initid(char* gpio_name, char* path, int initid, struct board_info* board);
int get_power_index_by_showid(int showid, struct board_info* board);
/*get the length of the longest power-related variable name*/
int get_max_power_name_length(struct board_info* board);
/*convert bitmask to offset of the boot mode, for example, bitmask 0x38 offset is 3, because 00111000>>3=00000111 */
int get_boot_mode_offset(unsigned char boot_mode_pin_bitmask);
char* get_boot_mode_name_from_hex(struct board_info* board, int boot_mode_hex);

#endif //BOARD_H
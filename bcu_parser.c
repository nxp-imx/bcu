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

#include "bcu_parser.h"
#include "chip.h"
#include "board.h"

#define MAX_CHIP_SPEC_LENGTH 200

extern struct name_and_init_func chip_list[];
extern int num_of_chips;
extern struct board_info board_list[];
extern int num_of_boards;


int update_parameter_string(char* path, char* parameter_name, char* str)
{
	char tpath[MAX_PATH_LENGTH];
	char* ptr = strstr(path, parameter_name);
	if (ptr == NULL)
	{
		printf("could not locate parameter %s\n", parameter_name);
		return -1;
	}
	char* equal_sign = strchr(ptr, '=');
	char* brk = strpbrk(equal_sign, ";}");
	if (equal_sign == NULL)
	{
		printf("can not understand the specification \n");
		return -1;
	}
	strcpy(tpath, brk);
	equal_sign[1] = '\0';
	strcat(equal_sign, str);
	strcat(equal_sign, tpath);
	//int length = (int)(brk - equal_sign - 1);
	//strncpy(result, equal_sign + 1, length);
	//*(result + length) = '\0';
	return 0;

}

int extract_parameter_string(char* chip_specification, char* parameter_name, char* result)
{
	char* ptr = strstr(chip_specification, parameter_name);
	if (ptr == NULL)
	{
		if(strcmp(parameter_name, "sensor2") != 0)
			printf("could not locate parameter %s\n", parameter_name);
		return -1;
	}
	char* equal_sign = strchr(ptr, '=');
	char* brk = strpbrk(equal_sign, ";}");
	if (equal_sign == NULL)
	{
		printf("can not understand the specification \n");
		return -1;
	}
	int length = (int)(brk - equal_sign - 1);
	strncpy(result, equal_sign + 1, length);
	*(result + length) = '\0';
	return 0;

}

int extract_parameter_value(char* chip_specification, char* parameter_name)
{
	char str[MAX_CHIP_SPEC_LENGTH];
	int status = extract_parameter_string(chip_specification, parameter_name, str);
	if (status == -1)
	{
		return -1;
	}
	if (str[0] == '0' && str[1] == 'x')
		return strtol(str + 2, NULL, 16);
	else
		return strtol(str, NULL, 10);
}

void get_chip_name(char* chip_specification, char* chip_name)
{
	char* ptr = strchr(chip_specification, '{');
	if (ptr == NULL)
		strcpy(chip_name, chip_specification);
	else
	{
		int length = (int)(ptr - chip_specification);
		strncpy(chip_name, chip_specification, length);
		*(chip_name + length) = '\0';
	}
}

void free_device_linkedlist_backward(struct device* ptr)
{
	void* delete_this;

	while (ptr != NULL)
	{
		delete_this = ptr;
		void* parent = ptr->parent;
		if (parent == NULL) //if we have reached the end, the the device must be ftdi, so we must close and free the ftdidevice;
		{
			//printf("closing and freeing ftdi device\n");
			struct device* ft = delete_this;
			ft->free(ft);
			return;
		}
		ptr = ptr->parent;

		free(delete_this);
	}
	return;
}

void free_device_linkedlist_forward(struct device* ptr)
{
	void* delete_this;

	while (ptr != NULL)
	{
		delete_this = ptr;
		void* parent = ptr->parent;
		if (parent == NULL) //if the device has no parent, then the device must be ftdi, so we must close and free the ftdidevice;
		{
			// printf("closing and freeing ftdi device\n");
			struct device* ft = delete_this;
			ft->free(ft);
		}
		ptr = ptr->child;

		if (parent != NULL)
			free(delete_this);
	}
	return;
}

int get_chip_specification_by_chipname(char* path, char* chip_specification, char* chipname)
{
	char now_chip_name[MAX_CHIP_SPEC_LENGTH];
	char remaining_path[MAX_PATH_LENGTH];

	if (path[0] == '/')
		path++; //ignore the first '/'
	if (strlen(path) < MAX_PATH_LENGTH)
		strcpy(remaining_path, path);

	strcpy(chip_specification, strtok(remaining_path, "/"));

	while (chip_specification != NULL)
	{
		get_chip_name(chip_specification, now_chip_name);
		if (strcmp(chipname, now_chip_name) == 0) {
			return 0;
		}

		strcpy(chip_specification, strtok(NULL, "/"));
	}
	return -1;
}

void* build_device_linkedlist_forward(void** head, char* path)
{
	//printf("original path: %s\n", path );

	char remaining_path[MAX_PATH_LENGTH];
	char chip_name[MAX_CHIP_SPEC_LENGTH];
	struct device* current = NULL;
	struct device* previous = NULL;

	if (path[0] == '/')
		path++; //ignore the first '/'
	if (strlen(path) < MAX_PATH_LENGTH)
		strcpy(remaining_path, path);
	else {
		printf("entered path exceeded maximum length of the buffer\n");
		return NULL;
	}
	char* chip_specification = strtok(remaining_path, "/");
	//printf("chip_specification: %s\n", chip_specification);
	int found = 0;
	int is_head = 1;
	while (chip_specification != NULL)
	{
		get_chip_name(chip_specification, chip_name);
		found = 0;
		for (int i = 0; i < num_of_chips; i++)
		{
			if (strcmp(chip_list[i].name, chip_name) == 0)
			{
				found = 1;
				current = chip_list[i].create_funcptr(chip_specification, previous);
				if (current == NULL)
				{
					printf("failed to create %s data structure\n", chip_name);
					return NULL;
				}
				if (is_head)
				{
					if (head != NULL)
						*head = current;
					is_head = 0;
				}

				if (previous != NULL)
					previous->child = current;

				previous = current;
			}
		}
		if (!found)
		{
			printf("did not recognize chip '%s'\n", chip_name);
			free_device_linkedlist_backward(current);
			return NULL;
		}
		chip_specification = strtok(NULL, "/");
		//printf("chip_specification: %s\n", chip_specification);
	}
	if (current)
		current->child = NULL;
	//printf("finished\n");
	return current;
}

void set_options_default(struct options_setting* setting)
{
	strcpy(setting->board, "");
	setting->auto_find_board = 0;
	setting->path[0] = '\0';
	setting->location_id = -1;
	setting->output_state = -1;
	setting->delay = 0;
	setting->boot_mode_hex = -1;
	for (unsigned int i = 0; i < MAX_BOOT_CONFIG_BYTE; i++)
	{
		setting->boot_config_hex[i] = -1;
	}
	setting->gpio_name[0] = '\0';
	setting->dump = 0;
	setting->force = 0;
	setting->pmt = 0;
	setting->nodisplay = 0;
	setting->dumpname = NULL;
	setting->refreshms = 0;
	setting->use_rms = 0;
	setting->rangefixed = 0;
	setting->use_hwfilter = 0;
	setting->use_bipolar = 1;
	setting->dump_statistics = 0;
	setting->eeprom_function = 0;
	setting->download_doc = 0;
	setting->download_pre_release = 0;
}


/*
 * parse the options entered in command line, stores them in the option_setting structure, which use for actual command
 */
extern char GV_LOCATION_ID[];
int parse_board_id_options(int argc, char** argv, struct options_setting* setting)
{
	//find if the board model is specified first,
	//this way, board dependent setting such as choosing board-specific gpio pin are done correctly
	int argc_count;
	for (argc_count = 2; argc_count < argc; argc_count++)
	{
		//printf("parsing %s\n", argv[argc_count]);
		char* begin = strchr(argv[argc_count], '=');
		char* input = begin + 1;
		if (strncmp(argv[argc_count], "-board=", 7) == 0 && strlen(argv[argc_count]) > 7)
		{
			strcpy(setting->board, input);
			printf("board model is %s\n", setting->board);
			// break;
		}

		if (strncmp(argv[argc_count], "-id=", 4) == 0 && strlen(argv[argc_count]) > 4)
		{
			strcpy(GV_LOCATION_ID, input);
			// printf("location_id is %s\n", GV_LOCATION_ID);
		}

		if (strcmp(argv[argc_count], "-auto") == 0)
		{
			setting->auto_find_board = 1;
			printf("will auto find the board...\n");
		}
	}

	if (strcmp(argv[1], "upgrade") == 0 || strcmp(argv[1], "uuu") == 0 ||
	    strcmp(argv[1], "lsftdi") == 0 || strcmp(argv[1], "lsboard") == 0 ||
	    strcmp(argv[1], "version") == 0 || strcmp(argv[1], "help") == 0)
	{
		return 0;
	}

	if (!setting->auto_find_board)
	{
		return NO_USE_AUTO_FIND;
	}
	else
	{
		if (strlen(setting->board) == 0 && strlen(GV_LOCATION_ID) == 0)
			return NO_BOARD_AND_ID; //Not provide -board and -id, try auto find board then
		if (strlen(setting->board) == 0)
			return NO_BOARD; //Not provide -board
		if (strlen(GV_LOCATION_ID) == 0)
			return NO_ID; //Not provide -id
	}

	return 0;
}

int parse_options(char* cmd, int argc, char** argv, struct options_setting* setting)
{
	struct board_info* board = NULL;
	if (strcmp(cmd, "upgrade") &&
	    strcmp(cmd, "uuu") &&
	    strcmp(cmd, "lsftdi") &&
	    strcmp(cmd, "lsboard") &&
	    strcmp(cmd, "version") &&
	    strcmp(cmd, "help"))
	{
		board = get_board(setting->board);
		if (board == NULL)
			return -1;
	}

	for (int i = 2; i < argc; i++)
	{
		//printf("parsing %s\n", argv[i]);
		char* begin = strchr(argv[i], '=');
		char* input = begin + 1;

		if (strncmp(argv[i], "-path=", strlen("-path=")) == 0 && strlen(argv[i]) > strlen("-path="))
		{
			strcpy(setting->path, input);
			printf("set customized path as: %s\n", setting->path);
		}
		else if (strncmp(argv[i], "-id=", 4) == 0 && strlen(argv[i]) > 4)
		{
			strcpy(GV_LOCATION_ID, input);
			printf("location_id is %s\n", GV_LOCATION_ID);
		}
		else if (strcmp(argv[i], "-auto") == 0)
		{
			setting->auto_find_board = 1;
			// printf("will auto find the board...\n");
		}
		else if (strncmp(argv[i], "-boothex=", 9) == 0 && strlen(argv[i]) > 4)
		{
			setting->boot_mode_hex = strtol(input, NULL, 16);
		}
		else if (strncmp(argv[i], "-bootbin=", 9) == 0 && strlen(argv[i]) > 4)
		{
			setting->boot_mode_hex = strtol(input, NULL, 2);
		}
		else if (strncmp(argv[i], "-delay=", 7) == 0 && strlen(argv[i]) > 7)
		{
			setting->delay = atoi(input);
			printf("delay is %d\n", setting->delay);
		}
		else if (strncmp(argv[i], "-hold=", 6) == 0 && strlen(argv[i]) > 6)
		{
			setting->hold = atoi(input);
			printf("hold is %d\n", setting->hold);
		}
		else if (strcmp(argv[i], "high") == 0)
		{
			setting->output_state = 1;
			printf("will set gpio high\n");
		}
		else if (strcmp(argv[i], "low") == 0)
		{
			setting->output_state = 0;
			printf("will set gpio low\n");
		}
		else if (strcmp(argv[i], "-toggle") == 0)
		{
			setting->output_state = 2;
			printf("toggle gpio\n");
		}
		else if (strcmp(argv[i], "1") == 0)
		{
			setting->output_state = 1;
			printf("will set gpio high\n");
		}
		else if (strcmp(argv[i], "0") == 0)
		{
			setting->output_state = 0;
			printf("will set gpio low\n");
		}
		else if (strncmp(argv[i], "-dump=", 6) == 0 && strlen(argv[i]) > 6)
		{
			setting->dump = 1;

			setting->dumpname = strdup(input);
			int len1 = strlen(setting->dumpname), len2 = strlen(".csv");
			if (len1 == 0)
			{
				free(setting->dumpname);
				setting->dumpname = strdup("monitor_record.csv");
			}
			else
			{
				if(strcmp(setting->dumpname + len1 - len2, ".csv"))
				{
					setting->dumpname = realloc(setting->dumpname, strlen(setting->dumpname) + strlen(".csv") + 1);
					strcat(setting->dumpname, ".csv");
				}
			}
			printf("dump data into %s file\n", setting->dumpname);
		}
		else if (strcmp(argv[i], "-dump") == 0)
		{
			setting->dump = 1;
			setting->dumpname = strdup("monitor_record.csv");
			printf("dump data into %s file\n", setting->dumpname);
		}
		else if (strcmp(argv[i], "-pmt") == 0)
		{
			setting->pmt = 1;
		}
		else if (strcmp(argv[i], "-nodisplay") == 0)
		{
			setting->nodisplay = 1;
			if (!setting->dump)
			{
				setting->dump = 1;
				setting->dumpname = strdup("monitor_record.csv");
				printf("dump data into %s file\n", setting->dumpname);
			}
		}
		else if (strncmp(argv[i], "-hz=", 4) == 0 && strlen(argv[i]) > 4)
		{
			double hz = atof(input);
			setting->refreshms = (int)(1000.0 / hz);
		}
		else if (strcmp(argv[i], "-rms") == 0)
		{
			setting->use_rms = 1;
		}
		else if (strcmp(argv[i], "-hwfilter") == 0)
		{
			setting->use_hwfilter = 1;
		}
		else if (strcmp(argv[i], "-unipolar") == 0)
		{
			setting->use_bipolar = 0;
		}
		else if (strcmp(argv[i], "-stats") == 0)
		{
			setting->dump_statistics = 1;
		}
		else if (strcmp(argv[i], "-erase") == 0)
		{
			setting->eeprom_function = PARSER_EEPROM_ERASE;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			setting->force = 1;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{
			setting->eeprom_function = PARSER_EEPROM_WRITE_DEFAULT;
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			setting->eeprom_function = PARSER_EEPROM_READ_AND_PRINT;
		}
		else if (strcmp(argv[i], "-wf") == 0)
		{
			setting->eeprom_function = PARSER_EEPROM_WRITE_FROM_FILE;
		}
		else if (strcmp(argv[i], "-rf") == 0)
		{
			setting->eeprom_function = PARSER_EEPROM_READ_TO_FILE;
		}
		else if (strncmp(argv[i], "-wsn=", 5) == 0 && strlen(argv[i]) > 5)
		{
			setting->eeprom_function = PARSER_EEPROM_UPDATE_USER_SN;

			setting->eeprom_usr_sn = atoi(input);
			printf("eeprom user SN will be set to %d\n", setting->eeprom_usr_sn);
		}
		else if (strncmp(argv[i], "-sn=", 4) == 0 && strlen(argv[i]) > 4)
		{
			setting->eeprom_usr_sn = atoi(input);
			printf("eeprom user SN will be set to %d\n", setting->eeprom_usr_sn);
		}
		else if (strncmp(argv[i], "-brev=", 6) == 0 && strlen(argv[i]) > 6)
		{
			strcpy(setting->eeprom_board_rev, input);
			printf("eeprom user board revision will be set to %s\n", setting->eeprom_board_rev);
		}
		else if (strncmp(argv[i], "-srev=", 6) == 0 && strlen(argv[i]) > 6)
		{
			strcpy(setting->eeprom_soc_rev, input);
			printf("eeprom user soc revision will be set to %s\n", setting->eeprom_soc_rev);
		}
		else if (strcmp(argv[i], "-doc") == 0)
		{
			setting->download_doc = 1;
		}
		else if (strcmp(argv[i], "-pre") == 0)
		{
			setting->download_pre_release = 1;
		}
		else if (strncmp(argv[i], "-board=", 7) == 0 && strlen(argv[i]) > 7)
		{
			//do nothing
		}
		else
		{
			if (board == NULL)
				return -1;
			int j = 0;
			int found = 0;
			while (board->mappings[j].name != NULL)
			{
				if (strcmp(board->mappings[j].name, argv[i]) == 0)
				{
					found = 1;
					strcpy(setting->gpio_name, argv[i]);
					break;
				}
				j++;
			}
			int k = 0;
			if (board->boot_modes != NULL)
			{
				while (board->boot_modes[k].name != NULL)
				{
					if (strcmp(board->boot_modes[k].name, argv[i]) == 0)
					{
						found = 1;
						setting->boot_mode_hex = board->boot_modes[k].boot_mode_hex;
						break;
					}
					k++;
				}
			}
			k = 0;
			if (board->boot_configs != NULL)
			{
				while (board->boot_configs[k].name != NULL)
				{
					if (strcmp(board->boot_configs[k].name, argv[i]) == 0)
					{
						found = 1;
						for (int bootcfg_n = 0; bootcfg_n < MAX_BOOT_CONFIG_BYTE; bootcfg_n++)
						{
							setting->boot_config_hex[bootcfg_n] = board->boot_configs[k].boot_config_hex[bootcfg_n];
						}
						break;
					}
					k++;
				}
			}
			if (!found)
			{
				printf("could not recognize input %s\n", argv[i]);
				return -1;
			}
		}
	}
	return 0;
}

/*
given the previous linkedlist pointed by old_head;
this function compares the new path and the old path
and modifies the linkedlist so that the different part between the two path are rebuild
WITHOUT reinitializing the identical part between the two path, which could save a lot of
initialization time.

if the given previous linkedlist is null,
completely rebuild the linkedlist from scratch
*/
void* build_device_linkedlist_smart(void** new_head, char* new_path, void* old_head, char* old_path)
{
	if (old_head == NULL || old_path == NULL)// first time building
	{
		//printf("building device linkedlist completely from scratch\n");
		return build_device_linkedlist_forward(new_head, new_path);
	}

	//printf("attempting to partially rebuild device linked list \n");
	char new_remaining_path[MAX_PATH_LENGTH];
	char old_remaining_path[MAX_PATH_LENGTH];
	char chip_name[MAX_CHIP_SPEC_LENGTH];

	if (new_path[0] == '/')
		new_path++; //ignore the first '/'

	if (old_path[0] == '/')
		old_path++; //ignore the first '/'

	strcpy(new_remaining_path, new_path);
	strcpy(old_remaining_path, old_path);

	//first obtained the specifications from old path:
	int num_of_chips_in_old_path = 0;
	char* old_chip_specifications[MAX_CHIP_SPEC_LENGTH];
	int num_of_chips_in_new_path = 0;
	char* new_chip_specifications[MAX_CHIP_SPEC_LENGTH];
	for (int i = 0; i < 20; i++)//initialize so that valgrind does not complain
	{
		old_chip_specifications[i] = NULL;
		new_chip_specifications[i] = NULL;
	}
	old_chip_specifications[0] = strtok(old_remaining_path, "/");
	while (old_chip_specifications[num_of_chips_in_old_path] != NULL)
	{
		num_of_chips_in_old_path++;
		old_chip_specifications[num_of_chips_in_old_path] = strtok(NULL, "/");
	}
	new_chip_specifications[0] = strtok(new_remaining_path, "/");
	while (new_chip_specifications[num_of_chips_in_new_path] != NULL)
	{
		num_of_chips_in_new_path++;
		new_chip_specifications[num_of_chips_in_new_path] = strtok(NULL, "/");
	}

	int index = 0;

	struct device* current = old_head;
	struct device* previous = NULL;
	while (new_chip_specifications[index] != NULL && old_chip_specifications[index] != NULL
		&& strcmp(new_chip_specifications[index], old_chip_specifications[index]) == 0)
	{
		previous = current;
		current = current->child;
		index++;
	}
	/*
	not able to find a simple solution across all situation, i decided to handle this problem by cases
	after the while loop above; most important thing to realize:
	1,previous points to the last identical chip specification between two path, right before they diverge
	2,current and index points to the first difference between the two path.
	*/
	if (index == 0)//case 0: they are different since the very beginning
	{
		//printf("completely different\n");
		//free all and completely rebuild
		free_device_linkedlist_forward(old_head);
		return build_device_linkedlist_forward(new_head, new_path);
	}
	else if (new_chip_specifications[index] == NULL && old_chip_specifications[index] == NULL)//case 1: they are identical
	{
		//printf("same\n");
		//no need to free anything
		*new_head = old_head;
		return previous;
	}
	else if (new_chip_specifications[index] != NULL && old_chip_specifications[index] != NULL)//case 2: they differ before any of them ends
	{
		//printf("different before end!\n");		
		free_device_linkedlist_forward(current);//free everything since the first difference
		*new_head = old_head;
		current = NULL;
		previous->child = NULL;
		//need reallocation in next step
	}
	else if (new_chip_specifications[index] == NULL && old_chip_specifications[index] != NULL)//case 3: the new one is a subset of old one
	{
		//printf("new shorter\n");		
		free_device_linkedlist_forward(current);//free unwanted extra parts
		*new_head = old_head;
		return previous;

	}
	else if (new_chip_specifications[index] != NULL && old_chip_specifications[index] == NULL)//case 4 the old one is a subset of new one
	{
		//printf("old shorter\n");		
		//no need to free anything
		*new_head = old_head;
		//need reallocation in next step
	}

	//reallocation
	while (new_chip_specifications[index] != NULL)
	{
		get_chip_name(new_chip_specifications[index], chip_name);
		int found = 0;
		for (int i = 0; i < num_of_chips; i++)
		{
			if (strcmp(chip_list[i].name, chip_name) == 0)
			{
				found = 1;
				current = chip_list[i].create_funcptr(new_chip_specifications[index], previous);
				if (current == NULL)
				{
					printf("failed to create %s data structure\n", chip_name);
					return NULL;
				}
				current->child = NULL;
				if (previous != NULL) {
					previous->child = current;
					previous = current;
				}
			}

		}
		if (!found)
		{
			printf("did not recognize chip '%s'\n", chip_name);
			free_device_linkedlist_backward(current);
			return NULL;
		}
		index++;
	}
	return current;
}

static int parse_group(int id, struct group* group_ptr, struct board_info* board)
{
	strcpy(group_ptr->name, board->power_groups[id].group_name);
	// printf("group name: %s\n", group_ptr->name);

	group_ptr->num_of_members = 0;
	if (strlen(board->power_groups[id].group_string) < (MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER))
		strcpy(group_ptr->member_list, board->power_groups[id].group_string);
	else
	{
		printf("entered group string exceeded maximum buffer size\n");
		return -1;
	}
	char group_member_string[MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER];
	strcpy(group_member_string, group_ptr->member_list);
	// printf("group string: %s\n", group_member_string);
	char* group_member_name = strtok(group_member_string, ",");
	while (group_member_name != NULL)
	{
		// printf("member: %s\n", group_member_name);
		int power_index = 0;
		int mapping_index = 0;
		int found = 0;
		while (board->mappings[mapping_index].name != NULL)
		{
			if (board->mappings[mapping_index].type == power)
			{
				if (strcmp(group_member_name, board->mappings[mapping_index].name) == 0)
				{
					found = 1;
					group_ptr->member_index[group_ptr->num_of_members] = power_index;
					group_ptr->num_of_members++;
					break;
				}
				power_index++;
			}

			mapping_index++;
		}
		if (found == 0)
			return -1;
		group_member_name = strtok(NULL, ",");
	}
	return 0;
}

int parse_groups(struct group* groups, struct board_info* board)
{
	int num_of_groups = 0;

	if (board->power_groups == NULL)
		return num_of_groups;

	while (board->power_groups[num_of_groups].group_name != NULL)
	{
		num_of_groups++;
	}

	for (int i = 0; i < num_of_groups; i++) {
		if (parse_group(i, &groups[i], board) == -1) {
			printf("failed to understand the power groups setting\n");
			return -1;
		}
	}

	return num_of_groups;
}

void groups_init(struct group* groups, int num)
{
	for (int i = 0; i < num; i++)
	{
		groups[i].min = 99999;
		groups[i].max = 0;
		groups[i].sum = 0;
		groups[i].avg = 0;
		groups[i].avg_data_size = 0;
	}
}

void __str_replace(char* cp, int n, char* str)
{
	int lenofstr;
	char* tmp;
	lenofstr = strlen(str);
	if (lenofstr < n)
	{
		tmp = cp + n;
		while (*tmp)
		{
			*(tmp - (n - lenofstr)) = *tmp;
			tmp++;
		}
		*(tmp - (n - lenofstr)) = *tmp;
	}
	else
	{
		if (lenofstr > n)
		{
			tmp = cp;
			while (*tmp) tmp++;
			while (tmp >= cp + n)
			{
				*(tmp + (lenofstr - n)) = *tmp;
				tmp--;
			}
		}
	}
	strncpy(cp, str, lenofstr);
}

int str_replace(char *str, char *source, char *dest)
{
	int count = 0;
	char *p;

	p = strstr(str, source);
	while(p)
	{
		count++;
		__str_replace(p, strlen(source), dest);
		p = p+strlen(dest);
		p = strstr(p, source);
	}

	return count;
}

int compare_version(const char *v1, const char *v2)
{
	const char *p_v1 = v1;
	const char *p_v2 = v2;

	while (*p_v1 && *p_v2) {
		char buf_v1[32] = {0};
		char buf_v2[32] = {0};

		char *i_v1 = strchr(p_v1, '.');
		char *i_v2 = strchr(p_v2, '.');

		if (!i_v1 || !i_v2) break;

		if (i_v1 != p_v1) {
			strncpy(buf_v1, p_v1, i_v1 - p_v1);
			p_v1 = i_v1;
		}
		else
			p_v1++;

		if (i_v2 != p_v2) {
			strncpy(buf_v2, p_v2, i_v2 - p_v2);
			p_v2 = i_v2;
		}
		else
			p_v2++;

		int order = atoi(buf_v1) - atoi(buf_v2);
		if (order != 0)
			return order < 0 ? -1 : 1;
	}

	double res = atof(p_v1) - atof(p_v2);

	if (res < 0) return -1;
	if (res > 0) return 1;
	return 0;
}

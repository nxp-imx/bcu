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

#include "parser.h"
#include "chip.h"
#include "board.h"

#define MAX_CHIP_SPEC_LENGTH 200

extern struct name_and_init_func chip_list[];
extern int num_of_chips;
extern struct board_info board_list[];
extern int num_of_boards;


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
			//printf("closing and freeing ftdi device\n");
			struct device* ft = delete_this;
			ft->free(ft);
		}
		ptr = ptr->child;

		free(delete_this);
	}
	return;
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
	strcpy(setting->board, "imx8mpevk");
	setting->path[0] = '\0';
	setting->location_id = -1;
	setting->output_state = -1;
	setting->delay = 0;
	setting->boot_mode_hex = -1;
	setting->gpio_name[0] = '\0';
	setting->dump = 0;
	setting->nodisplay = 0;
	setting->dumpname[0] = '\0';
	setting->refreshms = 0;
}


/*
 * parse the options entered in command line, stores them in the option_setting structure, which use for actual command
 */
extern char GV_LOCATION_ID[];
int parse_options(int argc, char** argv, struct options_setting* setting)
{
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
		else if (strncmp(argv[i], "-group=", 7) == 0 && strlen(argv[i]) > 7)
		{
			strcpy(setting->groups, input);
			printf("groups is %s\n", setting->groups);
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
			strcpy(setting->dumpname, input);
			int len1 = strlen(setting->dumpname), len2 = strlen(".csv");
			if (len1 < len2)
				strcpy(setting->dumpname, "monitor_record.csv");
			else
			{
				if(strcmp(setting->dumpname + len1 - len2, ".csv"))
				{
					strcat(setting->dumpname, ".csv");
				}
			}
			printf("dump data into %s file\n", setting->dumpname);
		}
		else if (strcmp(argv[i], "-dump") == 0)
		{
			setting->dump = 1;
			strcpy(setting->dumpname, "monitor_record.csv");
			printf("dump data into %s file\n", setting->dumpname);
		}
		else if (strcmp(argv[i], "-nodisplay") == 0)
		{
			setting->nodisplay = 1;
			setting->dump = 1;
			strcpy(setting->dumpname, "monitor_record.csv");
			printf("dump data into %s file\n", setting->dumpname);
		}
		else if (strncmp(argv[i], "-hz=", 4) == 0 && strlen(argv[i]) > 4)
		{
			int hz = atoi(input);
			setting->refreshms = 1000 / hz;
		}
		else if (strncmp(argv[i], "-board=", 7) == 0 && strlen(argv[i]) > 7)
		{
			//do nothing
		}
		else
		{
			struct board_info* board = get_board(setting->board);
			if (board == NULL)
			{
				return-1;
			}
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

static int parse_group(char* input, struct group* group_ptr, struct board_info* board)
{
	//first find name;
	char* brk = strchr(input, ':');
	if (brk == NULL)
		return -1;
	int length = brk - input;
	if (strlen(brk + 1) < (MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER))
		strcpy(group_ptr->member_list, brk + 1);
	else {
		printf("entered group string exceeded maximum buffer size\n");
		return -1;
	}
	strncpy(group_ptr->name, input, length);
	group_ptr->name[length] = '\0';//null terminated
	printf("group name: %s\n", group_ptr->name);

	group_ptr->num_of_members = 0;
	char group_member_string[1000];
	strcpy(group_member_string, brk + 1);
	char* group_member_name = strtok(group_member_string, ",");
	while (group_member_name != NULL)
	{
		printf("member: %s\n", group_member_name);
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

int parse_groups(char* input, struct group* groups, struct board_info* board)
{
	char groups_specification[MAX_NUMBER_OF_POWER][MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
	char* tok;
	char remaining_input[MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
	strcpy(remaining_input, input);
	tok = strtok(remaining_input, "]");
	int num_of_groups = 0;
	while (tok != NULL)
	{
		printf("group_specification: %s\n", tok + 1);//+1 because we need to ignore '['
		strcpy(groups_specification[num_of_groups], tok + 1);
		num_of_groups++;
		tok = strtok(NULL, "]");
	}

	for (int i = 0; i < num_of_groups; i++) {
		if (parse_group(groups_specification[i], &groups[i], board) == -1) {
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
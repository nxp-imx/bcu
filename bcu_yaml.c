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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "bcu_yaml.h"

struct bcu_yaml_version ver_before_big_ver[] =
{
	{"1.0.131"},
	{"1.0.153"},
	{"1.0.195"},
	{"1.0.237"},
	{"1.0.253"},
	{"1.1.1"},
	{"1.1.25"},
	{"1.1.26"},
	{"1.1.28"},
	{"1.1.29"},
	{"1.1.37"},
	{"1.1.39"},
	{"1.1.41"},
	{"1.1.45"},
	{"1.1.47"},
	{"1.1.53"},
	{"1.1.55"},
	{"1.1.57"},
	{"1.1.71"},
	{"1.1.72"},
	{"1.1.75"},
	{"1.1.78"},
	{"1.1.85"},
	{"1.1.89"},
	{"1.1.90"},
	{"1.1.97"},
	{"1.1.98"},
	{"1.1.100"},
	{"1.1.101"},
	{"1.1.109"},
	{"1.1.110"},
	{"1.1.120"},
	{"1.1.124"},
	{"1.1.125"},
	{NULL}
};

void get_yaml_file_path(char* path)
{
#if defined(linux) || defined(__APPLE__)
	char * tmpenv;
	if (( tmpenv = getenv( "HOME" )) != NULL )
		strcat(path, tmpenv);
	else
		printf("HOME env variable not set.\n");
	strcat(path, "/bcu_config.yaml");
#else
	const char* homeProfile = "USERPROFILE";
	GetEnvironmentVariable(homeProfile, path, 128);
	strcat(path, "\\bcu_config.yaml");
#endif
}

void writeConf(void)
{
	char yamfile[128] = {0};
	get_yaml_file_path(yamfile);

	FILE *fp = fopen(yamfile, "w+");
	if (fp == NULL) {
		printf("writeConf: Failed to open config file: %s!\n", yamfile);
		return;
	}

	char text[512];
	char chip_specification[MAX_PATH_LENGTH];
	int i = 0, j;

	sprintf(text, "config_version: %s                              \n", GIT_VERSION);
	fputs(text, fp);
	fputs("# show_id can set the display order of the rails.\n# show_id should start from 1.\n# If show_id: 0, it means that this rail will not be displayed and dumped.\n", fp);
	fputs("#\n# Please DO NOT delete any line of a power rail!\n# If you don't want to show it, please just set its \"show_id\" as 0.\n", fp);
	fputs("#\n# Unit of rsense1 and rsense2 is Milliohm.\n", fp);
	sprintf(text, "#\n# Can add, modify or delete groups for each platform with power measurement.\n# Up to %d groups can be added to each platform.\n", MAX_NUMBER_OF_GROUP);
	fputs(text, fp);
	fputs("# Please follow the existing examples to add groups.\n", fp);

	for(j = 0; j < get_board_numer(); j++)
	{
		struct board_info* board=get_board_by_id(j);

		sprintf(text, "\n\nboardname: %s\n", board->name);
		fputs(text, fp);
		sprintf(text, "mappings:\n");
		fputs(text, fp);
		i = 0;
		while (board->mappings[i].name != NULL)
		{
			if(board->mappings[i].type == power)
			{
				sprintf(text, "    - %-20s: ", board->mappings[i].name);
				fputs(text, fp);

				int rs1, rs2;
				char sr_name[100], sr_path[MAX_PATH_LENGTH];

				get_chip_specification_by_chipname(board->mappings[i].path, chip_specification, "pac1934");
				rs1 = extract_parameter_value(chip_specification, "rsense1");
				rs2 = extract_parameter_value(chip_specification, "rsense2");

				sprintf(text, "{ ");
				fputs(text, fp);

				sprintf(text, "rsense1: %-8d, ", rs1);
				fputs(text, fp);
				sprintf(text, "rsense2: %-8d, ", rs2);
				fputs(text, fp);

				sprintf(text, "show_id: %-3d ", i + 1);
				fputs(text, fp);

				strcpy(sr_name, "SR_");
				strcat(sr_name, board->mappings[i].name);
				if (get_path(sr_path, sr_name, board) != -1)
				{
					sprintf(text, "} #Range: rsense1=>%.1fmA, rsense2=>%.1fmA\n", 100000.0 / rs1, 100000.0 / rs2);
					fputs(text, fp);
				}
				else
				{
					// sprintf(text, "} #Range: don't have extra RS\n");
					// fputs(text, fp);
					sprintf(text, "}\n");
					fputs(text, fp);
				}
			}
			i++;
		}

		sprintf(text, "groups: #Please do NOT delete this line.\n");
		fputs(text, fp);
		if (board->power_groups != NULL)
		{
			i = 0;
			while (board->power_groups[i].group_name != NULL)
			{
				sprintf(text, "    - %-20s: ", board->power_groups[i].group_name);
				fputs(text, fp);

				sprintf(text, "\"%s\"\n", board->power_groups[i].group_string);
				fputs(text, fp);

				i++;
			}
		}
	}

	fclose(fp);
}

int updateRsense(struct board_info* board, char* rail_name, char* rs1, char* rs2)
{
	char path[MAX_PATH_LENGTH];

	if (get_path(path, rail_name, board) < 0)
	{
		printf("updateRsense: rail path not found!\n");
		return -1;
	}

	update_parameter_string(path, "rsense1", rs1);
	update_parameter_string(path, "rsense2", rs2);

	if (set_path(path, rail_name, board) < 0)
	{
		printf("updateRsense: set rail path failed!\n");
		return -1;
	}
	return 0;
}

#define FILE_MAX_LENGTH 4096
int replace_str(char* path, char* source, char* dest)
{
	char buffer[FILE_MAX_LENGTH];
	int fp_start = 0;
	int fp_end = 0;
	int buffer_length = 0;
	int move_length = 0;
	FILE *fp = fopen(path, "r+");

	if(fp == NULL)
	{
		printf("File open failed\n");
		return 0;
	}
	while (1)
	{
		memset(buffer, 0, FILE_MAX_LENGTH);
		if(fgets(buffer, FILE_MAX_LENGTH, fp) == NULL)
		{
			break;
		}
		else
		{
			buffer_length = strlen(buffer);
			for(int j = 0; j < buffer_length; j++)
			{
				if(strncmp(source, &buffer[j], strlen(source)) == 0)
				{
					fp_end = ftell(fp);
					move_length = buffer_length - j;
#if defined(linux) || defined(__APPLE__)
					fseek(fp, -(move_length), SEEK_CUR);
#else
					fseek(fp, -(move_length + 1), SEEK_CUR);
#endif
					fp_start = ftell(fp);
					if (strlen(source) >= strlen(dest))
					{
						for(int i = 0; i < strlen(source); i++)
						{
							if(i < strlen(dest))
								fputc(dest[i], fp);
							else
								fputc(' ', fp);
						}
					} else {
						for(int i = 0; i < strlen(dest); i++)
						{
							fputc(dest[i], fp);
						}
					}
					fseek(fp, fp_end, SEEK_SET);
				}
			}
		}
	}
	printf("File update successfully!\n");
	fclose(fp);
	return 0;
}

int readConf(char* boardname, struct options_setting* setting)
{
	char yamfile[128] = {0};
	get_yaml_file_path(yamfile);
	FILE* fh = fopen(yamfile, "r");
	if (fh == NULL)
	{
		printf("readConf: Failed to open config file: %s!\n", yamfile);
		return -1;
	}

	yaml_parser_t parser;
	yaml_token_t token;

	if (!yaml_parser_initialize(&parser))
	{
		printf("Failed to initialize parser!\n");
		return -2;
	}

	yaml_parser_set_input_file(&parser, fh);

	/*
	* state = 0 -> key
	* state = 1 -> value
	*/
	int state = 0;
	char* tk;
	int i = -1;
	int now_status = -1;
	struct board_info* now_board;
	char now_rail[MAX_MAPPING_NAME_LENGTH];
	int group_id = 0;
	char rs1[10], rs2[10];
	char version[30] = "";

	do
	{
		yaml_parser_scan(&parser, &token);

		switch (token.type)
		{
		case YAML_KEY_TOKEN: state = 0; break;
		case YAML_VALUE_TOKEN: state = 1; break;
		case YAML_SCALAR_TOKEN:
			tk = token.data.scalar.value;

			if (state == 0)
			{
				if (!strcmp(tk, "config_version"))
					now_status = STATUS_CHECK_VERSION;
				else if (!strcmp(tk, "boardname"))
					now_status = STATUS_CHANGE_BOARD;
				else if (now_status == STATUS_WAITING_WANTED_BOARD)
					break;
				else if (!strcmp(tk, "mappings"))
					now_status = -1;
				else if (!strcmp(tk, "rsense1"))
					now_status = STATUS_CHANGE_RSENSE1;
				else if (!strcmp(tk, "rsense2"))
					now_status = STATUS_CHANGE_RSENSE2;
				else if (!strcmp(tk, "show_id"))
					now_status = STATUS_CHANGE_SHOWID;
				else if (!strcmp(tk, "groups"))
				{
					now_status = STATUS_GROUPS;
					group_id = 0;
					now_board->power_groups = NULL;
				}
				else if (!strcmp(tk, "show_sel"))
					now_status = -1;
				else if (!strcmp(tk, "israngefixed"))
					now_status = STATUS_CHANGE_FIXEDRAIL;
				else if (!strcmp(tk, "extra_sr"))
					now_status = STATUS_CHANGE_DEFAULT_RS;
				else
				{
					if (now_status == STATUS_GROUPS || now_status == STATUS_CHANGE_GROUPS)
					{
						if (now_status == STATUS_CHANGE_GROUPS)
							group_id++;
						else
							now_board->power_groups = (struct board_power_group*)malloc(sizeof(struct board_power_group) * (MAX_NUMBER_OF_GROUP + 1));
						if (group_id >= MAX_NUMBER_OF_GROUP)
						{
							printf("%s: Too much group! Please keep the number of groups NOT greater than %d\n", __func__, MAX_NUMBER_OF_GROUP);
							return -2;
						}
						now_status = STATUS_CHANGE_GROUPS;
						// printf("group name: %s\n", tk);
						now_board->power_groups[group_id].group_name = malloc(sizeof(char) * MAX_MAPPING_NAME_LENGTH);
						strcpy(now_board->power_groups[group_id].group_name, tk);
						now_board->power_groups[group_id + 1].group_name = NULL;
					}
					else
					{
						now_status = STATUS_CHANGE_RAIL;
						// printf("railname: %s\n", tk);
						strcpy(now_rail, tk);
					}
				}
			}
			else
			{
				switch (now_status)
				{
				case STATUS_CHECK_VERSION:
				{
					if (compare_version(&GIT_VERSION[4], &tk[4]) != 0)
					{
						printf("\nConfig file version mismatch!\n");
						int temp = 0;
						while (ver_before_big_ver[temp].version)
						{
							if (compare_version(ver_before_big_ver[temp].version, &tk[4]) >= 0)
							{
								printf("        BCU version: %s\n", GIT_VERSION);
								printf("Config file version: %s\n", tk);
								printf("Config file version is too old!\nPlease delete the old config file: %s, then run BCU again!\n", yamfile);
								return -3;
							}
							if (compare_version(ver_before_big_ver[temp].version, &GIT_VERSION[4]) >= 0)
							{
								printf("        BCU version: %s\n", GIT_VERSION);
								printf("Config file version: %s\n", tk);
								printf("BCU version is too old!\nPlease delete the old config file: %s, then run BCU again!\n", yamfile);
								return -3;
							}
							temp++;
						}
						printf("No big change between these two version.\nWill update config file: %s automatically!\n", yamfile);
						replace_str(yamfile, tk, GIT_VERSION);
						strcpy(version, GIT_VERSION);
					}
					else
						strcpy(version, tk);
				}break;
				case STATUS_WAITING_WANTED_BOARD: break;
				case STATUS_CHANGE_BOARD:
				{
					// printf("boardname: %s\n", tk);
					if (strcmp(tk, boardname))
					{
						now_status = STATUS_WAITING_WANTED_BOARD;
						break;
					}
					now_board = get_board(tk);
					if (now_board == NULL)
						return -2;
				}break;
				case STATUS_CHANGE_RAIL:
				{
					// printf("railname: %s\n", tk);
				}break;
				case STATUS_CHANGE_FIXEDRAIL:
				{
					setting->rangefixed = 0;
				}break;
				case STATUS_CHANGE_RSENSE1:
				{
					// printf("rse1: %s\n", tk);
					strcpy(rs1, tk);
				}break;
				case STATUS_CHANGE_RSENSE2:
				{
					// printf("rse2: %s\n", tk);
					strcpy(rs2, tk);
					updateRsense(now_board, now_rail, rs1, rs2);
				}break;
				case STATUS_CHANGE_SHOWID:
				{
					// printf("id: %s\n", tk);
					int item = get_item_location(now_rail, now_board);
					if (item < 0)
						return -2;
					now_board->mappings[item].initinfo = atoi(tk);
				}break;
				case STATUS_CHANGE_GROUPS:
				{
					// printf("group string: %s\n", tk);
					now_board->power_groups[group_id].group_string = malloc(sizeof(char) * MAX_MAPPING_NAME_LENGTH * MAX_NUMBER_OF_POWER);
					strcpy(now_board->power_groups[group_id].group_string, tk);
				}break;
				case STATUS_CHANGE_DEFAULT_RS:
					break;
				default:
					break;
				}
			}
			break;
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);

	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_token_delete(&token);
	yaml_parser_delete(&parser);

	fclose(fh);

	if (strcmp(version, GIT_VERSION))
	{
		if (compare_version(&GIT_VERSION[4], &version[4]) != 0)
		{
			printf("\nConfig file version is too old!\nPlease delete the old config file: %s, then run BCU again!\n", yamfile);
			return -3;
		}
	}

	return 0;
}

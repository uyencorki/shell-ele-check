#ifndef SUPPORT_FUNCTION_H
#define SUPPORT_FUNCTION_H


#include "stdio.h"
#include <iostream>
#include <fstream>
#include <string>
#include "string.h"

#include <vector>
#include <QString>

using namespace std;



//CONVERT
//########################################################################################################
string sf_string_to_hex(const std::string& input);

long sf_hex_to_int(char ch);
long sf_char_hex_to_int(const char *input_char);
//########################################################################################################







//FILE
//########################################################################################################
//ham phu tro
int sf_filesize(QString filename);

//tim kiem
int sf_find_data_after_key_in_string(string input_char, string key, string &output);

int sf_find_data_after_key_in_file_linebyline(QString filename, string key, string &output);

int sf_find_data_between_2_key_in_string(string input_char, string start_key, string end_key, string &output);

//change
int sf_change_data_line_in_file(QString filename, int line_num_change, string data_change);
//########################################################################################################

#endif // SUPPORT_FUNCTION_H

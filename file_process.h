#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include "stdio.h"
#include <iostream>
#include <fstream>

#include <sstream>

#include "support_function.h"
#include <QString>


using namespace std;






class file_process
{
    //luc them mot gia tri vao day thi can luu y
    /*
     * 1. create string df name = ten dung de tim kiem ben trong file
     * 2. create string name de lu gia tri get duoc
     * 3. fix load_from_conf
     * 4. fix save_to_conf
     * 5. fix get_setting_para in mainwindow de apdung data tu conf to program
     * 6. fix SETTING_load_from_line_edit de get file dua vao data for save
     */
private:
    string DEBUG_PORT_df = "DEBUG_PORT = ";

    string I2C_slave_addr_df = "I2C_slave_addr = ";
    string I2C_data_byte_df = "I2C_data_byte = ";
    string I2C_register_addr_df = "I2C_register_addr = ";

    string cmd_dir_df = "cmd_dir = ";
    string cmd_shell_df = "cmd_shell = ";

    string sd_card_dir_df = "sd_card_dir = ";
    string sd_card_dir_test_df = "sd_card_dir_test = ";


    string SD_CARD_check_df = "SD_CARD_check = ";
    string GPIO_check_df = "GPIO_check = ";
    string SPI_check_df = "SPI_check = ";
    string UART_check_df = "UART_check = ";
    string I2C_check_df = "I2C_check = ";
    string UVC_check_df = "UVC_check = ";
    string GMSM_check_df = "GMSM_check = ";
    string WIFI_check_df = "WIFI_check = ";
    string LED_check_df = "LED_check = ";


    string GPIO_mode_df = "GPIO_mode = ";
    string GPIO_port_df = "GPIO_port = ";



public:
    QString file_name = "setting.conf";
//    string file_name = "setting.conf";

    string DEBUG_PORT;
    string I2C_slave_addr, I2C_data_byte, I2C_register_addr;
    string cmd_dir, cmd_shell, sd_card_dir_full, sd_card_dir, sd_card_dir_mount, sd_card_dir_test;

    string SD_CARD_check;
    string GPIO_check;
    string SPI_check;
    string UART_check;
    string I2C_check;
    string UVC_check;
    string GMSM_check;
    string WIFI_check;
    string LED_check;

    string GPIO_mode, GPIO_port;



public:
    file_process();



    int load_from_conf();
    int save_to_conf();
    int change_data_conf(string df_name, string data_change);


};









#endif // FILE_PROCESS_H

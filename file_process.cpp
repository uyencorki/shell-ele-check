#include "file_process.h"

#include "string.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QApplication>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextEdit>

#include <QThread>
#include <QTime>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QDate>

#include <iostream>
#include <string>


using namespace std;


file_process::file_process()
{


}


int file_process::load_from_conf()
{

    string s = "Somewhere down the road";
    int ret;
    //load cac setting info
    ret = sf_find_data_after_key_in_file_linebyline(file_name, DEBUG_PORT_df, DEBUG_PORT);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, I2C_slave_addr_df, I2C_slave_addr);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, I2C_data_byte_df , I2C_data_byte);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, I2C_register_addr_df, I2C_register_addr);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, cmd_dir_df, cmd_dir);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, cmd_shell_df, cmd_shell);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, sd_card_dir_df, sd_card_dir_full);
    if(ret==-1) goto error;    



    //phan tich ra
    if(1){
        stringstream abc(sd_card_dir_full);
        abc >> sd_card_dir;
        abc >> sd_card_dir_mount;
//        if(sd_card_dir_mount.length()==0) goto error;
    }


    ret = sf_find_data_after_key_in_file_linebyline(file_name, sd_card_dir_test_df, sd_card_dir_test);
    if(ret==-1) goto error;




    //load cac check conf
    ret = sf_find_data_after_key_in_file_linebyline(file_name, SD_CARD_check_df, SD_CARD_check);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, GPIO_check_df, GPIO_check);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, SPI_check_df, SPI_check);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, UART_check_df, UART_check);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, I2C_check_df, I2C_check);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, UVC_check_df, UVC_check);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, GMSM_check_df, GMSM_check);
    if(ret==-1) goto error;

    ret = sf_find_data_after_key_in_file_linebyline(file_name, WIFI_check_df, WIFI_check);
    if(ret==-1) goto error;



    ret = sf_find_data_after_key_in_file_linebyline(file_name, LED_check_df, LED_check);
    if(ret==-1) goto error;



    //load GPIO_conf
    ret = sf_find_data_after_key_in_file_linebyline(file_name, GPIO_mode_df, GPIO_mode);
    if(ret==-1) goto error;
    ret = sf_find_data_after_key_in_file_linebyline(file_name, GPIO_port_df, GPIO_port);
    if(ret==-1) goto error;


//    cout << "GMSM_check = " << GMSM_check << endl;
//    cout << "WIFI_check = " << WIFI_check << endl;
//    cout << "LED_check = " << LED_check << endl;


    return 1;
error:
    return -1;

}

int file_process::change_data_conf(string df_name, string data_change)
{
    int line_num; string output;
    string data_change_full;

    //tim vi tri can thay doi. output show ra gia tri after key
    line_num = sf_find_data_after_key_in_file_linebyline(file_name, df_name, output);
    if(line_num==-1) goto error;
    //tao lai data can sua lai
    data_change_full = df_name + data_change;

    sf_change_data_line_in_file(file_name, line_num, data_change_full);

    return 1;
error:
    return -1;
}


int file_process::save_to_conf()
{
    string data_save = "";
    string end_line = "\n";


    data_save +=end_line;
    data_save += DEBUG_PORT_df + DEBUG_PORT + end_line;
    data_save +=end_line;

    data_save += I2C_slave_addr_df + I2C_slave_addr + end_line;
    data_save += I2C_data_byte_df + I2C_data_byte + end_line;
    data_save += I2C_register_addr_df + I2C_register_addr + end_line;
    data_save +=end_line;

    data_save += cmd_shell_df + cmd_shell+ end_line;
    data_save += cmd_dir_df + cmd_dir + end_line;
    data_save +=end_line;

    data_save += sd_card_dir_df + sd_card_dir + " " + sd_card_dir_mount + end_line;
    data_save += sd_card_dir_test_df + sd_card_dir_test + end_line;
    data_save +=end_line;


    //test mode
    data_save +=end_line;
    data_save += SD_CARD_check_df + SD_CARD_check + end_line;
    data_save += GPIO_check_df + GPIO_check + end_line;
    data_save += SPI_check_df + SPI_check + end_line;
    data_save += UART_check_df + UART_check + end_line;
    data_save += I2C_check_df + I2C_check + end_line;
    data_save += UVC_check_df + UVC_check + end_line;
    data_save += GMSM_check_df + GMSM_check + end_line;
    data_save += WIFI_check_df + WIFI_check + end_line;
    data_save += LED_check_df + LED_check + end_line;
    data_save += LED_check_df + LED_check + end_line;


    data_save +=end_line;


    //GPIO_conf
    data_save +=end_line;
    data_save += GPIO_mode_df + GPIO_mode + end_line;
    data_save += GPIO_port_df + GPIO_port + end_line;
    data_save +=end_line;




//    cout << data_save << endl;

    //save
//    ofstream file_save;
//    file_save.open(file_name, ios_base::trunc | ios_base::out);
//    file_save << data_save << endl;
//    file_save.close();


    QFile file;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;
    QTextStream out(&file);
    QString Q_data = QString::fromStdString(data_save);

    out << Q_data << endl;

    file.close();

    return 1;
}






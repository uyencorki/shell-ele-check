#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <sys/stat.h>

#include <QThread>
#include <QFontDatabase>

using namespace std;
using namespace cv;

/*
DEBUG_PORT = COM28

I2C_slave_addr = 53
I2C_data_byte = 1
I2C_register_addr = 2d

cmd_dir = /hnbin/cmd/bin/
cmd_shell = test_shell

sd_card_dir = /dev/mmcblk0p1
sd_card_dir_test = [1;35m/dev/mmcblk0p1[0m

 */


const char *pass = "hinoeng";
QSerialPort *serial = new QSerialPort();


QString data_check[3] = {"11", "25", "39"};


//SPI
//spi send phai tach ra ntn
const char *spi_loop_check_cmd = "68 6e 5f 73 70 69 5f 74 65 73 74"; //h n _ s p i _ t e s t";

//UART
const char *uart_loop_check_cmd_send = "68 6e 5f 75 61 72 74 5f 74 65 73 74";// "hn_uart_test";

const char *uart_loop_check_cmd_get = "686e5f756172745f74657374";// "hn_uart_test";

#define GPIO_port_max_def  20
#define GPIO_numcheck   4 //so lan blink 0 - 1

int GPIO_DEF_POS[GPIO_port_max_def];
unsigned int GPIO_NUM_PORT;

int GPIO_ketqua[GPIO_port_max_def][GPIO_port_max_def] = {{0}}; //[num_gpio][]

vector <int> GPIO_PORT_OK;
vector <int> GPIO_PORT_NOT_OK;




enum mode_printf{
    PRINT_NO = 0,
    PRINT_YES = 1,
};

const int time_out_check = 2000;

QString button_START = "background-color: rgb(150, 180, 220)";

QString button_blue = "background-color: rgb(51,153,255)";
QString button_red = "background-color: rgb(250, 55, 55)";


QString button_gray = "background-color: rgb(191, 191, 191)";
QString button_lock = button_gray;
QString button_unlock = "background-color: rgb(150, 180, 220)";

QString button_process = "background-color: rgb(250,100,42)";
QString button_state_process = "background-color: rgb(250,210,152)";


QString button_set = button_unlock;
QString button_blank = "";

bool just_check_wifi = false;
bool just_check_GMSI = false;

#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

   // cout << "run step 1 " << endl;

    ui->setupUi(this);

    //Set name
    this->setWindowTitle("SHELL_ELECTRONICS_BOARD_CHECK 2021_09_27");

    //set UI
    ui->textEdit->setReadOnly(true);

    //button color
    ui->START->setStyleSheet(button_START);

    //Lock button
    on_actionLock_button_triggered();

    //File setting Link get
    QString file = QCoreApplication::applicationFilePath();         //Get the excute file path
    QString filePath = QFileInfo(file).absolutePath();

    QString qcheckbox_style = "QCheckBox::indicator {\n     width: 60px;\n     height: 60px;\n}\n\nQCheckBox::indicator:checked {\n     image: url(" + filePath + "/Icon_conf/check.jpg);\n}\n\nQCheckBox::indicator:unchecked\n{\n	image: url(" + filePath + "/Icon_conf/uncheck.jpg);\n}";

    //set may cai checkbox

    ui->GMSM_check->setStyleSheet(qcheckbox_style);

    ui->WIFI_check->setStyleSheet(qcheckbox_style);

    ui->Check__LED->setStyleSheet(qcheckbox_style);


    QString path("D:/HUU_UYEN/Software/QT_creator/build-FULLHAN_STREAMING_CHECK-Desktop_Qt_5_14_2_MinGW_32_bit-Release/release/Setting_conf_日野エンジニアリング/");
    QDir dir; // Initialize to the desired dir if 'path' is relative
              // By default the program's working directory "." is used.

    // We create the directory if needed
    if (!dir.exists(path))
        dir.mkpath(path); // You can check the success if needed

    QFile file_1(path + "NewFile.kml");
    file_1.open(QIODevice::ReadWrite); // Or QIODevice::ReadWrite



    //FILE and setting
    QString QString_settingfile = filePath  + "/Setting_conf_日野エンジニアリング/setting.conf";
//    file_conf.file_name = QString_settingfile.toUtf8().constData();
    file_conf.file_name = QString_settingfile;

    qDebug() << "QString_settingfile"  <<QString_settingfile << endl;

//    file_conf.file_name = QString_settingfile;
    get_setting_para();

//    cout << "QString_settingfile = " << QString_settingfile.toUtf8().toStdString() << endl;
//    qDebug() << "QString_settingfile " << QString_settingfile.toUtf8()<<endl;
//    qDebug()<<QString_settingfile.toUtf8()<<"\nExist: "<<f1.exists();


//    QString fileName1 = "E:/QT projects/Path_in_Japanese/ランゲージ/ランゲージ/New folder/testbuilding.IVE";
//    QFile f1(fileName1);
//    qDebug()<<fileName1.toUtf8()<<"\nExist: "<<f1.exists();
//    qDebug() << QString::fromWCharArray(L"日本語")<< endl;
//    qDebug() << QString::fromWCharArray(L"日野エンジニアリング")<< endl;
////    qDebug() <<  QString::fromWCharArray(fileName1) << endl;
//    qDebug() << "fileName1  " << fileName1 << endl;
//    cout << "japanese 1 "<<endl;
//    cout << " 日野エンジニアリング" << endl;
//    cout << "japanese 2 "<<endl;
//    cout << str.toStdString() << endl;
//    cout << string.toStdString() << endl;


    string a = "12345 日野エンジニアリング";
//    cout << "a = " << a << endl;


    if(SETTING_CONF_OK==false)
    {
        cout << "GET FILE_CONF ERROR" <<endl;
        ui->textEdit->append("GET FILE_CONF ERROR");
    }
    else
    {
        cout << "GET FILE_CONF OK" <<endl;
        ui->textEdit->append("GET FILE_CONF OK");
    }

    //Set name again
    this->setWindowTitle(this->windowTitle() + "  " + QString::fromStdString(file_conf.DEBUG_PORT));

    //UART DEBUG check
    connect(&timer_uart_setting_wait,SIGNAL(timeout()),  this, SLOT(uart_debug_setting()));
    timer_uart_setting_wait.start(1000);


    connect(&count_display_timer,SIGNAL(timeout()),  this, SLOT(qtext_edit_count_display_timer()));



    //GPIO blink conf
    connect(&timer_blink,SIGNAL(timeout()),  this, SLOT(gpio_blink_()));


    //camera setting
    //set Qtimer camera
    connect(&Timer_get_camera,SIGNAL(timeout()),this, SLOT(Process()));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));


    webmainWindow = new QMainWindow();

    toolBar = new QToolBar;
    toolBar->addWidget(locationEdit);


    webmainWindow->addToolBar(toolBar);
    ui->webview->setLayout(new QHBoxLayout);
    ui->webview->layout()->addWidget(webmainWindow);



    //set network
    QNetworkProxyFactory::setUseSystemConfiguration(false);




    // connect WiFi
//    findTimer = new QTimer();
//    findTimer->setInterval(1000);
//    connect(findTimer,&QTimer::timeout,this,&MainWindow::findActiveWirelesses);
//    findTimer->start();
//    foundCount = 0;
//    ui->treeWidgetWiFis->setColumnWidth(0,50);
//    ui->treeWidgetWiFis->setColumnWidth(1,200);
//    findActiveWirelesses();

}



MainWindow::~MainWindow()
{
    serialPort->close();
//    Reset_Camera();
    delete ui;

}

// connect to WiFi
void MainWindow::findActiveWirelesses()
{
    QNetworkConfigurationManager ncm;
    netcfgList = ncm.allConfigurations();
    WiFisList.clear();
    for (auto &x : netcfgList)
    {
        if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
        {
            if(x.name() == "")
                WiFisList << "Unknown(Other Network)";
            else
                WiFisList << x.name();

            qDebug() << x.type();
        }
    }
    for(int i=0; i<WiFisList.size(); i++)
    {
        bool exist = false;
        QTreeWidgetItem * item = new QTreeWidgetItem();
        cout << "topLevelItemCount " <<ui->treeWidgetWiFis->topLevelItemCount() << endl;

        for(int j=0; j<ui->treeWidgetWiFis->topLevelItemCount(); j++)
        {
            QTreeWidgetItem *index = ui->treeWidgetWiFis->topLevelItem(j);
            QString str = index->text(1);
            if(str == WiFisList[i])
            {
                exist = true;
                break;
            }
        }
        if(!exist)
        {
            item->setTextAlignment(0,Qt::AlignVCenter);
            item->setTextAlignment(1,Qt::AlignHCenter);
            item->setText(0,QString::number(++foundCount));
            item->setText(1,WiFisList[i]);
            ui->treeWidgetWiFis->addTopLevelItem(item);
        }
    }
}



void MainWindow::closeEvent(QCloseEvent *event) {
    //quit camera trong truong hop tat dot ngot
    if((Camera_UVC_start==true)||(Camera_WIFI_start==true))
    {
        serialPort->write("quit");
        serialPort->write("\n");

        Camera_UVC_start = false;
        Camera_WIFI_start = false;

    }

    if(UVC_had_run==true) Reset_Camera();

//    cout << "quit" << endl;
    QThread::msleep(300);

}


//UART DEBUG Setting
//##############################################################################################



bool MainWindow::send_message_yes_no(QString title, QString mess){
    QMessageBox messageBox1(QMessageBox::Question,
                 tr(title.toUtf8().constData()),
                 tr(mess.toUtf8().constData()),
                 QMessageBox::Yes | QMessageBox::No,
                 this);

if(messageBox1.exec()==QMessageBox::No) return false;
else return true;
}


void MainWindow::remove_last_line_textedit(QTextEdit *text_edit)
{
    text_edit->setFocus();
    QTextCursor storeCursorPos = text_edit->textCursor();
    text_edit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    text_edit->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    text_edit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    text_edit->textCursor().removeSelectedText();
    text_edit->textCursor().deletePreviousChar();
    text_edit->setTextCursor(storeCursorPos);
}



bool Count_start = false;
int num_count = 0;
bool Count_up = true;
int Count_start_num;


void MainWindow::qtext_edit_count_display_timer()
{
    if(Count_start==false)
    {
        num_count = Count_start_num;
        ui->textEdit->insertPlainText (QString::number(num_count));
        Count_start = true;
    }
    else
    {
        for(int i=0; i< QString::number(num_count).size(); i++) ui->textEdit->textCursor().deletePreviousChar();

        if(Count_up==true) num_count++;
        else num_count--;
        ui->textEdit->insertPlainText (QString::number(num_count));

        //end trong truong hop time out
        if((Count_up==false)&&(num_count==0))
        {

            count_display_timer.stop();
            emit count_display_timeout();
        }

        if((NEW_BOARD_STATE==1)&&(Count_up==true))
        {
            if(num_count==40) {
                NEW_BOARD_STATE = FOUND_NEW_BOARD_FINISHED;
                count_display_timer.stop();
            }
        }
    }
}





void MainWindow::uart_debug_setting()
{
//    cout << "run to uart_debug_setting " << endl;

    //neu setting file khong OK, return
    if(SETTING_CONF_OK==false) return;

    serialPort->close();
    serialPort->setPortName(QString::fromStdString(file_conf.DEBUG_PORT));
    serialPort->open(QIODevice::ReadWrite);
    if (!serialPort->isOpen()) {

        //bat dau count_display
        if(count_display_timer.isActive()==false) {
            //bat dau lai count
            Count_start = false;
            //set dem len or xuong
            Count_up = true;
            //set gia tri ban dau
            Count_start_num = 1;
            //print some infor
            ui->textEdit->append("UART DEBUG CONNECT ERROR   ");
            //start
            count_display_timer.start(1000);
        }

        DEBUG_OPEN_OK = false;
        return;
    }

    //stop count_display
    if(count_display_timer.isActive()==true) count_display_timer.stop();

    //uart debug config
    serialPort->setBaudRate(QSerialPort::Baud115200/*, QSerialPort::AllDirections*/);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);//
    serialPort->setStopBits(QSerialPort::OneStop);

    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    connect(serialPort,SIGNAL(readyRead()),this, SLOT(receive_data()));
//    cout << "connect to recive data " << endl;

    //remove_last_line_textedit(ui->textEdit);
    ui->textEdit->append("UART DEBUG CONNECT OK");

    //stop qua trinh check uart debug
    timer_uart_setting_wait.stop();
    DEBUG_OPEN_OK = true;

}





// _hn_start#xin chao#hn_end_
const char *serial_string_hn_start = "_hn_start#";
const char *serial_string_hn_end = "#hn_end_";

string DBG_input;

void MainWindow::receive_data()
{

    QByteArray databyte = serialPort->readAll();
    QString data = databyte;

//    cout << "pre time = " << input << endl;
    DBG_input += data.toUtf8().constData();
//    cout << "now  = " << input << endl;


//    ui->textEdit->append(QString::fromStdString(DBG_input));

//    cout << "DBG_input111 = " << DBG_input << endl;

    int pos1 = 0, pos2 = 0;
    string get_data = "";

    //dung de save du lieu lai cho lan sau
    string remain = DBG_input;

    //dung de check xem thu board da khoi dong ok hay chua. Bang cach test xem thu co dong len nay xuat hien hay khong
//    if((START_RUNNING==false)&&(LOCK_BUTTON==true))
    {
        //for one time
        string start_key_startup_first = "ROM:	";

        //for test
        string start_key_startup = "ROM:	";
        string end_key_startup = " flash.";

        //Chi danh rieng cho viec phat hien board moi. Phu thuoc vao DBG_input cung kha la khong an toan
        bool start_ok = false; //-> tim lai tu dau
        for(unsigned int i=0; i< DBG_input.length(); i++){

        //tim new board first
            if(DBG_input.compare(i, start_key_startup_first.length(), start_key_startup_first)==0)
            {
                NEW_BOARD_STATE = FOUND_NEW_BOARD_SIGNAL; //trang thai phat hien tin hieu dau vao
        //stop count_display
                if(count_display_timer.isActive()==true)
                {
                    count_display_timer.stop();
                }

            }


            //Check input
            if(NEW_BOARD_STATE==FOUND_NEW_BOARD_SIGNAL) // trang thai phat hien tin hieu dau vao
            {
                start_key_startup = "ROM:	";
                end_key_startup = " flash.";
                //        ROM:    Use nor flash.
            }
            else if(NEW_BOARD_STATE==FOUND_NEW_BOARD_SIGNAL_FINISHED) // da phat hien xong tin hieu, chuyen sang trang thai cho board finish startup
            {
                start_key_startup = "Enter 'help' ";
                end_key_startup = "built-in commands.";
                //        Enter 'help' for a list of built-in commands.
            }


            //tim hn_start
            if(start_ok==false){
                if(DBG_input.compare(i, start_key_startup.length(), start_key_startup)==0){
                    start_ok = true;
                    pos1 = i + start_key_startup.length();
                }
            }

            //tim hn_end
            if(start_ok==true){
                if(DBG_input.compare(i, end_key_startup.length(), end_key_startup)==0){
                    start_ok = true;
                    pos2 = i;

                    //result
                    get_data = DBG_input.substr(pos1, pos2 - pos1);
                    //show

                    // get ok, luu nhung phan tu con lai vao remain.
                    remain = DBG_input.substr(pos2 + end_key_startup.length());
                    DBG_input = remain;

                        if(NEW_BOARD_STATE==FOUND_NEW_BOARD_SIGNAL)
                        {

                            string startup_check = "Use nor";
                            if(get_data.compare(startup_check)==0)
                            {
                                NEW_BOARD_STATE = FOUND_NEW_BOARD_SIGNAL_FINISHED;

                                //resetlai toan bo
                                count_display_timer.stop();
                                on_actionReset_triggered();
//                                ui->textEdit->append("新しいストリーミングボードが起動しているのを見つけました");
                                ui->textEdit->append("Found new streaming board");


                                //bat dau count_display
                                if(count_display_timer.isActive()==false) {
                                    //bat dau lai count
                                    Count_start = false;
                                    //set dem len or xuong
                                    Count_up = true;
                                    //set gia tri ban dau
                                    Count_start_num = 0;
                                    //print some infor
    //                                ui->textEdit->append("新しいストリーミングボードを待ち : ");
                                    ui->textEdit->append("Wait for new streaming board startup : ");
                                    //start
                                    count_display_timer.start(1000);
                                }
                            }
                        }
                        else if(NEW_BOARD_STATE==FOUND_NEW_BOARD_SIGNAL_FINISHED)
                        {
                            string startup_check = "for a list of ";
                            if(get_data.compare(startup_check)==0)
                            {

                                //stop count_display
                                if(count_display_timer.isActive()==true)
                                {
                                    count_display_timer.stop();
                                }

    //                            ui->textEdit->append("新しいストリーミングボードが起動できました");
                                ui->textEdit->append("New streaming board finished startup");
                                NEW_BOARD_STATE = FOUND_NEW_BOARD_FINISHED; // da tim thay board

                            }
                        }
                    //set false de tim nhung phan tu tiep theo
                    start_ok = false;
                }
            }
        }

    }

    if(NEW_BOARD_STATE!=FOUND_NEW_BOARD_FINISHED) return;
    //For check
    string start_key = serial_string_hn_start;
    string end_key = serial_string_hn_end;

    //(wifi show all)
    if(WIFI_LINK_FOUND_ON==true)
    {
        start_key = "# Link streaming server : ";
        end_key = "action=stream";
//        ui->textEdit->append(QString::fromStdString(input));
    }

    //dung de check xem uvc da off hoan toan chua de goi len dieu khien WiFi stream
    if(UVC_check_STOP==true)
    {
        start_key = "EXIT";
        end_key = "linux#";
    }

    if(WIFI_STATION_check==true)
    {
        start_key = "STATION_MODE";
        end_key = "linux#";
    }


    //SD card
    //check mount
    if(SD_CARD_CHECK_MOUNT==true)
    {
        start_key = file_conf.sd_card_dir; //"/dev/mmcblk0p1";
        end_key = file_conf.sd_card_dir_mount; //"/mnt";
    }
    if(SD_CARD_CHECK_LS==true)
    {
        start_key = "ls " +  file_conf.sd_card_dir; //"ls /dev/mmcblk0p1";
        end_key = "fh-linux#";
    }

    //show all
//    cout << DBG_input << endl;
//    ui->textEdit->append(QString::fromStdString(DBG_input));

    bool start_ok = false; //-> tim lai tu dau
    for(unsigned int i=0; i< DBG_input.length(); i++){
        //tim hn_start
        if(start_ok==false){
            if(DBG_input.compare(i, start_key.length(), start_key)==0)
            {
                start_ok = true;
                pos1 = i + start_key.length();
//                cout << "1 : " << input.substr(pos1) << endl;
            }
        }

        //tim hn_end
        if(start_ok==true){
            if(DBG_input.compare(i, end_key.length(), end_key)==0){
                start_ok = true;
                pos2 = i;

                //result
                get_data = DBG_input.substr(pos1, pos2 - pos1);
                //show
                if(DEBUG_SHOW==true)
                    ui->textEdit->append(QString::fromStdString(get_data));

//                cout << get_data << endl;


                // get ok, luu nhung phan tu con lai vao remain.
                remain = DBG_input.substr(pos2 + end_key.length());


                if(WIFI_LINK_FOUND_ON==true)
                {
//                    ui->textEdit->append("Link for cheking WiFi stream :");//QString::fromStdString(start_key));
//                    ui->textEdit->append(QString::fromStdString(get_data));

//                    ui->textEdit->moveCursor (QTextCursor::End);
//                    ui->textEdit->insertPlainText (QString::fromStdString(end_key));

                    get_data = get_data + end_key;
                }

                if(UVC_check_STOP==true)
                {
//                    cout << "UVC_CLOSE FINISH" << endl;
                    UVC_check_STOP = false;
                    emit UVC_cmd_EXIT_finished();
                }

                if(WIFI_STATION_check==true)
                {
                //                    cout << "UVC_CLOSE FINISH" << endl;
                    WIFI_STATION_check = false;
                    emit UVC_cmd_EXIT_finished();
                }

                //Check data tra ve debug
                if(DEBUG_get_data_on==true)
                {
//                    cout << "data_get.size() = " << data_get.size() << " DEBUG_num_data_get = " << DEBUG_num_data_get << endl;

                    data_get.push_back(get_data);
                    if(data_get.size()==DEBUG_num_data_get){
                        //Get data finish -> emit signal receiver_uart_data_ok.
                        emit receive_uart_data_ok();
//                        cout << "emit" << endl;
                        DEBUG_get_data_on = false;
                    }
                }

                //set false de tim nhung phan tu tiep theo
                start_ok = false;
            }
        }
    }

    //update remain for next time
    DBG_input = remain;
}


void MainWindow::uart_send_shell(QString cmd, int mode)
{
    serialPort->write(file_conf.cmd_dir.c_str());
    serialPort->write(file_conf.cmd_shell.c_str());
    serialPort->write(" ");

    //pass
    serialPort->write(pass);
    serialPort->write(" ");

    //mode
    serialPort->write(QString::number(mode).toUtf8());
    serialPort->write(" ");

//    serialPort->write(cmd_dir);
    serialPort->write(cmd.toUtf8());
    serialPort->write("\n");
}




void MainWindow::uart_send(QString cmd)
{
    serialPort->write(cmd.toUtf8());
    serialPort->write("\n");
}

//##############################################################################################




//SD_CARD
//##############################################################################################
int MainWindow::check_insert(){
    //bat dau check
    Insert_OK = false;
    SD_CARD_CHECK_LS = true; // chen nay vao de set lai start_key vs end_key

    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    //Set data get
    DEBUG_num_data_get = 1;
    DEBUG_get_data_on = true;

    //send text
    char cmd[1000];
    sprintf(cmd, "ls %s", file_conf.sd_card_dir.c_str());
    uart_send(cmd);

    //set thoi gian doi du lieu toi da
    timer_loop.start(time_out_check);
    //loop start. Wait cho den khi nhan duoc data
    loop.exec();

    if(data_get.size()<DEBUG_num_data_get)
    {
//        cout << "time out" << endl;
        goto exit;
    }

    //data sau khi get 2 byte dau va 2 byte cuoi se la 0d0a -> loai bo 4 byte nay di roi so sanh voi sd_card_dir_test
    data_get[0] = data_get[0].substr(2, data_get[0].length()-4);
    if(data_get[0].compare(0, strlen(file_conf.sd_card_dir_test.c_str()), file_conf.sd_card_dir_test.c_str())==0) Insert_OK = true;


exit:
     //release data
     vector<string>().swap(data_get);

     SD_CARD_CHECK_LS = false;

     return Insert_OK;
}




int MainWindow::check_mount()
{
    //bat dau check
    SD_CARD_CHECK_MOUNT = true;
    Mount_OK = false;

    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit); // dung SD_card_check vi khong can lay du lieu tra ve
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    //Set data get
    DEBUG_num_data_get = 1;
    DEBUG_get_data_on = true;

    //send uart check
    uart_send("df");

    //set thoi gian doi du lieu toi da
    timer_loop.start(time_out_check);
    //loop start. Wait cho den khi nhan duoc data
    loop.exec();

    if(data_get.size()<DEBUG_num_data_get)
    {
//        cout << "time out" << endl;
        goto exit;
    }

    //co ton tai data tra ve -> da mount
    Mount_OK = true;


exit:

    //release data
    vector<string>().swap(data_get);

    SD_CARD_CHECK_MOUNT = false;

    return Mount_OK;

}

const char *write_test = "xin chao";
int MainWindow::check_write(){
    //bat dau check
    Insert_OK = false;

    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    DEBUG_num_data_get = 1;
    DEBUG_get_data_on = true;

    //send text
    char cmd[1000];
    sprintf(cmd, "echo %s >> %stest_sd_card", write_test, file_conf.sd_card_dir.c_str());
    uart_send_shell(cmd, PRINT_YES); // dung test_shell de echo se tao ra file co chua data with start and end key

    sprintf(cmd, "cat %stest_sd_card", file_conf.sd_card_dir.c_str());
    uart_send(cmd); // doc file vua tao

    //set thoi gian doi du lieu toi da
    timer_loop.start(time_out_check);
    //loop start. Wait cho den khi nhan duoc data
    loop.exec();

    if(data_get.size()<DEBUG_num_data_get)
    {
//        cout << "time out" << endl;
        goto exit;
    }


    if(data_get[0].compare(write_test)==0)
    {
        Insert_OK = true;
    }

exit:
    //xoa file vua tao
    sprintf(cmd, "rm -f %stest_sd_card", file_conf.cmd_dir.c_str());
    uart_send(cmd);

    //release data
    vector<string>().swap(data_get);

    return Insert_OK;


}

void MainWindow::SD_CARD_check_run()
{
    QThread::msleep(200);// doi 1s de khoi dong cho chac an
    if(SD_check==false) return;
    ui->textEdit->append(" - SD_CARD :");

    //moi vo cho bang false luon. Goto quit gia chung thi van la false
    SD_CARD_OK = false;


    //Tao 1 loop de doi du lieu tra ve tu UART_debug
    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    //check insert
    if(check_insert()==1)
    {
        ui->textEdit->append("SD_CARD had inserted");
    }
    else
    {
        ui->textEdit->append("SD_CARD not insert");
        goto quit;
    }

    //check mount
    if(check_mount()==1)
    {
        ui->textEdit->append("SD_CARD had mounted");
    }
    else
    {
        ui->textEdit->append("SD_CARD not mount");

        char cmd[1000];
        sprintf(cmd, "mount %s %s/", file_conf.sd_card_dir.c_str(), file_conf.sd_card_dir_mount.c_str());
        serialPort->write(cmd);
        serialPort->write("\n");

        //check mount again
        if(check_mount()!=1)
        {
            ui->textEdit->append("SD_CARD mount ERROR");
            goto quit;
        }
        else
            ui->textEdit->append("SD_CARD mount OK");
    }

    //check read write
    SD_CARD_OK = check_write();





quit:

    if(SD_CARD_OK==true)
    {
        ui->textEdit->append("SD_CARD OK");
       // ui->SD_CARD_STATE->setStyleSheet(button_blue);ui->SD_CARD_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("SD_CARD ERROR");
       // ui->SD_CARD_STATE->setStyleSheet(button_red);ui->SD_CARD_STATE->setText("NG");
    }


    //change button color
  //  ui->SD_CARD->setStyleSheet(button_set);

    return;
}

void MainWindow::on_SD_CARD_clicked()
{
    if(LOCK_BUTTON==true) return;
    SD_CARD_check_run();
}
//##############################################################################################





//GPIO
//##############################################################################################
void MainWindow::GPIO_setdir(int GPIO, int dir){
    char cmd[1000];
    sprintf(cmd, "%sgpio_dir %d %d", file_conf.cmd_dir.c_str(), GPIO, dir);
    uart_send_shell(cmd, PRINT_NO);
}

void MainWindow::GPIO_setvalue(int GPIO, int value){
    char cmd[1000];
    sprintf(cmd, "%sgpio_setvalue %d %d", file_conf.cmd_dir.c_str(), GPIO, value);
    uart_send_shell(cmd, PRINT_YES);
}

void MainWindow::GPIO_getvalue(int GPIO)
{
    char cmd[1000];
    sprintf(cmd, "%sgpio_getvalue %d", file_conf.cmd_dir.c_str(), GPIO);
    uart_send_shell(cmd, PRINT_YES);
}

void MainWindow::GPIO_setting_dir(unsigned int GPIO_set)
{
    for(unsigned int i=0;i<GPIO_NUM_PORT;i++){
        if(GPIO_set==i)  GPIO_setdir(GPIO_DEF_POS[i], 0);
        else GPIO_setdir(GPIO_DEF_POS[i], 1);
    }
}

void MainWindow::GPIO_setting_send(unsigned int GPIO_set, int value){
    //GPIO_set se duoc goi truoc tien
    //tiep theo se theo thu tu nhu trong gpio_def_enum tu vi tri thu 1 cho den thu n
/*
GPIO_0	 : set GPIO0 value to 1
GPIO_1	 : 0 => ERROR
GPIO_2	 : 0 => ERROR
GPIO_3	 : 0 => ERROR
GPIO_22	 : 0 => ERROR
GPIO_1	 : set GPIO1 value to 0
GPIO_0	 : 0 => OK
GPIO_2	 : 0 => OK
GPIO_3	 : 0 => OK
GPIO_22	 : 0 => OK
 */
    //set truoc roi moi bat dau get
    GPIO_setvalue(GPIO_DEF_POS[GPIO_set], value);

    for(unsigned int i=0;i<GPIO_NUM_PORT;i++){
        if(GPIO_set!=i)  GPIO_getvalue(GPIO_DEF_POS[i]);
    }
}

void MainWindow::GPIO_check_run_messenger(int GPIO_set)
{
    int GPIO_port_name = GPIO_DEF_POS[GPIO_set];

    QMessageBox message_check_result;
    message_check_result.setWindowTitle("Check blink GPIO_" + QString::number(GPIO_port_name));
    message_check_result.setText("Please choose OK or NG");

    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    /*QPushButton *result_NG = */message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);

    //message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->GPIO->y() - 50);

    message_check_result.exec();

    if (message_check_result.clickedButton() == result_OK) {
        GPIO_PORT_OK.push_back(GPIO_set);
        ui->textEdit->append("GPIO" + QString::number(GPIO_port_name) + " OK");
    }
    else
    {
        GPIO_PORT_NOT_OK.push_back(GPIO_set);
        ui->textEdit->append("GPIO" + QString::number(GPIO_port_name) + " ERROR");
    }


    wifi_stream_stop();
}

void MainWindow::GPIO_check_run()
{
    QThread::msleep(200);// doi 1s de khoi dong cho chac an
    if(GPIO_check==false) return;
    QString GPIO_check_info;



    ui->textEdit->append(" - GPIO : " + GPIO_check_info);

    GPIO_OK = false;

    //change button color
//    ui->GPIO->setStyleSheet(button_process);
//    ui->GPIO_STATE->setStyleSheet(button_state_process);ui->GPIO_STATE->setText("NG");

//    int GPIO_set = 0;

    for(unsigned int GPIO_set = 0; GPIO_set<GPIO_NUM_PORT; GPIO_set++)
    {
        GPIO_setting_dir(GPIO_set);

        QEventLoop loop;
        connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
        QTimer timer_loop;
        timer_loop.setSingleShot(true);
        connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    }



quit:

    //check
    if(GPIO_PORT_OK.size()==GPIO_NUM_PORT)
    {
        ui->textEdit->append("GPIO OK");
    }
    else if(GPIO_PORT_OK.size()>0)
    {
        ui->textEdit->append("GPIO_PORT_OK :  ");
        for(unsigned int i = 0; i< GPIO_PORT_OK.size();i++)
        {
            ui->textEdit->moveCursor (QTextCursor::End);
            ui->textEdit->insertPlainText (QString::number(GPIO_DEF_POS[GPIO_PORT_OK[i]])+"  ");
//            ui->textEdit->append(QString::number(GPIO_DEF_POS[GPIO_PORT_OK[i]])+"  ");
        }

        ui->textEdit->append("GPIO_PORT_NOT_OK :  ");
        for(unsigned int i = 0; i< GPIO_PORT_NOT_OK.size();i++)
        {
            ui->textEdit->moveCursor (QTextCursor::End);
            ui->textEdit->insertPlainText (QString::number(GPIO_DEF_POS[GPIO_PORT_NOT_OK[i]])+"  ");
        }
    }
    else {
        ui->textEdit->append("GPIO ERROR");
    }


    //Fill color
    if(GPIO_PORT_OK.size()==GPIO_NUM_PORT)
    {
        //ui->GPIO_STATE->setStyleSheet(button_blue);ui->GPIO_STATE->setText("OK");
    }
    else
    {
       // ui->GPIO_STATE->setStyleSheet(button_red);ui->GPIO_STATE->setText("NG");
    }




    //release data
    vector<string>().swap(data_get);
    //stop viec nhan debug
    DEBUG_get_data_on = false;

    //change button color
    //ui->GPIO->setStyleSheet(button_set);

    //release data
    vector<int>().swap(GPIO_PORT_OK);
    vector<int>().swap(GPIO_PORT_NOT_OK);
    //clear lai cac gia tri check
    memset(GPIO_ketqua, 0, sizeof GPIO_ketqua);
}

void MainWindow::on_GPIO_clicked()
{
    if(LOCK_BUTTON==true) return;
    GPIO_check_run();
}



void MainWindow::on_GPIO_BLINK_test_clicked()
{
    GPIO_BLINK_ON = !GPIO_BLINK_ON;

    //change button color
//    ui->GPIO_BLINK_test->setStyleSheet(button_process);

    if(GPIO_BLINK_ON==false)
    {
        //stop blink
        emit GPIO_blink_test_sig();
        return;
    }


//    GPIO_port_blink_test = ui->line_GPIO_blink->text().toInt();


    //set dir
    GPIO_setdir(GPIO_port_blink_test, 0);

    //Tao 1 loop de doi du lieu tra ve tu UART_debug
    QEventLoop loop;
    connect(this, &MainWindow::GPIO_blink_test_sig,  &loop, &QEventLoop::quit);





    //blink whith 0.5 s
    timer_blink.start(500);

    loop.exec();
    timer_blink.stop();

//    cout << "stop blink " << endl;

    GPIO_BLINK_ON = false;
    //change button color
//    ui->GPIO_BLINK_test->setStyleSheet(button_blank);


}
int gpio_blink_num = 0;
void MainWindow::gpio_blink_()
{
//    cout << "blink " <<  GPIO_port_blink_test << "  " << gpio_blink_num << endl;
    GPIO_setvalue(GPIO_port_blink_test, (gpio_blink_num++)%2);
}

//GPIO_conf
void MainWindow::on_GPIO_mode_clicked(bool checked)
{
//    cout << checked << endl;
//    cout << ui->GPIO_mode->isChecked() << endl;
    if(checked==true)
    {
//        ui->label_GPIO_mode->setText("Loop Test");
        //ui->GPIO->setText("GPIO_L");
    }
    else
    {

    }

//    ui->GPIO_mode->setChecked(checked);
}
//##############################################################################################




//SPI
//##############################################################################################
void MainWindow::spi_setting(){
    char cmd[1000];
    sprintf(cmd, "%sspi_setting -m 0", file_conf.cmd_dir.c_str());
    uart_send_shell(cmd, PRINT_NO);
}

void MainWindow::spi_send(QString cmd_send){
    char cmd[1000];
    sprintf(cmd, "%sspi_recv -a %s -p 1", file_conf.cmd_dir.c_str(), cmd_send.toUtf8().constData());
    uart_send_shell(cmd, PRINT_YES);
}

void MainWindow::SPI_check_run()
{
    QThread::msleep(200);// doi 1s de khoi dong cho chac an
    if(SPI_check==false) return;
    ui->textEdit->append(" - SPI :");

    SPI_OK = false;

    //change button color
    //ui->SPI->setStyleSheet(button_process);
    //ui->SPI_STATE->setStyleSheet(button_state_process);ui->SPI_STATE->setText("NG");

    //Tao 1 loop de doi du lieu tra ve tu UART_debug
    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));

    //SPI send xong nhan lai cung chi 1 data
    DEBUG_num_data_get = 1;
    DEBUG_get_data_on = true; // =true de ben receiver uart bat dau save get data

    //settting spi
    spi_setting();

    //send data
    spi_send(spi_loop_check_cmd);

    //set thoi gian doi du lieu toi da
    timer_loop.start(time_out_check);
    //loop start. Wait cho den khi nhan duoc data
    loop.exec();


    //FINISH

    //check time
    if(data_get.size()<DEBUG_num_data_get)
    {
        ui->textEdit->append("Time out");

        //ui->SPI_STATE->setStyleSheet(button_red);ui->SPI_STATE->setText("NG");
        goto quit;
    }

    //check data
    if(data_get[0].compare(0, strlen(spi_loop_check_cmd), spi_loop_check_cmd)==0)
    {
        SPI_OK = true;
    }
    else
    {
        SPI_OK = false;
    }


quit:
    if(SPI_OK==true)
    {
        ui->textEdit->append("SPI OK");
       // ui->SPI_STATE->setStyleSheet(button_blue);ui->SPI_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("SPI ERROR");
    //    ui->SPI_STATE->setStyleSheet(button_red);ui->SPI_STATE->setText("NG");
    }



    //release data
    vector<string>().swap(data_get);
    //stop viec nhan debug
    DEBUG_get_data_on = false;

    //change button color
   // ui->SPI->setStyleSheet(button_set);
}

void MainWindow::on_SPI_clicked()
{
    if(LOCK_BUTTON==true) return;
    SPI_check_run();
}
//##############################################################################################




//UART
//##############################################################################################

void MainWindow::uart_setting(){
    char cmd[1000];
    sprintf(cmd, "%suart_setting -b 115200", file_conf.cmd_dir.c_str());
    uart_send_shell(cmd, PRINT_NO);
}

void MainWindow::uart_send_loop(QString cmd_send)
{
    char cmd[1000];
    sprintf(cmd, "%suart_send %s loop", file_conf.cmd_dir.c_str(), cmd_send.toUtf8().constData());
    uart_send_shell(cmd, PRINT_YES);
}

void MainWindow::UART_check_run()
{
    QThread::msleep(200);// doi 1s de khoi dong cho chac an
    if(UART_check==false) return;
    ui->textEdit->append(" - UART :");

    UART_OK = false;

    //change button color
    //ui->UART->setStyleSheet(button_process);
    //ui->UART_STATE->setStyleSheet(button_state_process);ui->UART_STATE->setText("NG");

    //Tao 1 loop de doi du lieu tra ve tu UART_debug
    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));


    //SPI send xong nhan lai 2 line of data
    DEBUG_num_data_get = 2;
    DEBUG_get_data_on = true; // =true de ben receiver uart bat dau save get data

    //settting uart
    uart_setting();

    //send data
    uart_send_loop(uart_loop_check_cmd_send);

    //set thoi gian doi du lieu toi da
    timer_loop.start(time_out_check);
    //loop start. Wait cho den khi nhan duoc data
    loop.exec();


    //FINISH

    //check time
    if(data_get.size()<DEBUG_num_data_get)
    {
        ui->textEdit->append("Time out");

        //ui->UART_STATE->setStyleSheet(button_red);ui->UART_STATE->setText("NG");
        goto quit;
    }


    //check data
    if(data_get[1].compare(0, strlen(uart_loop_check_cmd_get), uart_loop_check_cmd_get)==0)
    {
        UART_OK = true;
    }
    else
    {
        UART_OK = false;
    }


quit:
    if(UART_OK==true)
    {
        ui->textEdit->append("UART OK");
        //ui->UART_STATE->setStyleSheet(button_blue);ui->UART_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("UART ERROR");
        //ui->UART_STATE->setStyleSheet(button_red);ui->UART_STATE->setText("NG");
    }


    //release data
    vector<string>().swap(data_get);
    //stop viec nhan debug
    DEBUG_get_data_on = false;

    //change button color
    //ui->UART->setStyleSheet(button_set);
}


void MainWindow::on_UART_clicked()
{
    if(LOCK_BUTTON==true) return;
    UART_check_run();
}


//##############################################################################################


//I2C
//##############################################################################################

void MainWindow::i2c_setting()//cmd chua gia tri can goi va doc
{
    char cmd[1000];
    sprintf(cmd, "%si2c_setting -s %s -d %s", file_conf.cmd_dir.c_str(), file_conf.I2C_slave_addr.c_str(), file_conf.I2C_data_byte.c_str());
    uart_send_shell(cmd, PRINT_NO);
}


void MainWindow::i2c_send(QString cmd_send)//cmd chua gia tri can goi va doc
{
    char cmd[1000];
    sprintf(cmd, "%si2c_send %s %s", file_conf.cmd_dir.c_str(), file_conf.I2C_register_addr.c_str(), cmd_send.toUtf8().constData());
    uart_send_shell(cmd, PRINT_NO);
}


void MainWindow::i2c_read()
{
    char cmd[1000];
    sprintf(cmd, "%si2c_recv -a %s", file_conf.cmd_dir.c_str(), file_conf.I2C_register_addr.c_str());
    uart_send_shell(cmd, PRINT_YES);
}


void MainWindow::I2C_check_run()
{
    QThread::msleep(200);// doi 1s de khoi dong cho chac an
    if(I2C_check==false) return;
    ui->textEdit->append(" - I2C :");

    //change button color
   // ui->I2C->setStyleSheet(button_process);
   // ui->I2C_STATE->setStyleSheet(button_state_process);ui->I2C_STATE->setText("NG");

    //Tao 1 loop de doi du lieu tra ve tu UART_debug
    QEventLoop loop;
    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
    QTimer timer_loop;
    timer_loop.setSingleShot(true);
    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));




    //settting uart
    i2c_setting();

    bool I2C_check_rslt = true;

    for(int sodata_send=0;sodata_send<3;sodata_send++){
        //I2C send xong nhan lai 1 line of data
        DEBUG_num_data_get = 1;
        DEBUG_get_data_on = true; // =true de ben receiver uart bat dau save get data

        //send data
        i2c_send(data_check[sodata_send]);
        //read data
        i2c_read();

        //set thoi gian doi du lieu toi da
        timer_loop.start(time_out_check);
        //loop start. Wait cho den khi nhan duoc data
        loop.exec();


        //FINISH

        //check time
        if(data_get.size()<DEBUG_num_data_get)
        {
            ui->textEdit->append("Time out");

            //ui->I2C_STATE->setStyleSheet(button_red);ui->I2C_STATE->setText("NG");
            goto quit;
        }


        //check data
        if(data_get[DEBUG_num_data_get-1].compare(0, data_check[sodata_send].size(), data_check[sodata_send].toUtf8().constData())==0)
        {
            I2C_check_rslt = true;
        }
        else
        {
            I2C_check_rslt = false; //check duoc 1 lan false thi thoat ra luon
            break;
        }


        //release data
        vector<string>().swap(data_get);
    }

    if(I2C_check_rslt==true)
    {
        ui->textEdit->append("I2C OK");
      //  ui->I2C_STATE->setStyleSheet(button_blue);ui->I2C_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("I2C ERROR");
     //   ui->I2C_STATE->setStyleSheet(button_red);ui->I2C_STATE->setText("NG");
    }



quit:
    //release data
    vector<string>().swap(data_get);
    //stop viec nhan debug
    DEBUG_get_data_on = false;

    //change button color
    //ui->I2C->setStyleSheet(button_set);
}


void MainWindow::on_I2C_clicked()
{
    if(LOCK_BUTTON==true) return;
    I2C_check_run();
}

//##############################################################################################









//UVC
//##############################################################################################

bool CAMERA_OPEN_OK;
bool switch_camera = true;
bool need_emit_UVC_OK = false;


void MainWindow::Process(){


   Mat image;
    bool UVC_OK_1 = GetCamera(image);
   // cout << UVC_OK_1 << endl;

    if(UVC_OK_1==false)
        {
        cap->release();
        cap->~VideoCapture();
        cap = new VideoCapture();
        ui->screen_cam->setText("   No Camera");
        if(dem_time==0) {timer_check.start(); dem_time = 1;}
        // check camera every 0.3s
        int gettime_for_camera_check = timer_check.elapsed();
        if((gettime_for_camera_check>300))
        {
        if(Camera_WIFI_start==false)
        {
                    //thay doi camera id cho den khi check thanh cong
                    if(switch_camera==true)
                    {
                        camera_number_id++;
                        if(camera_number_id==6) camera_number_id = 0;
                    }
                    //open camera
                    cap->open(CAP_DSHOW + camera_number_id);
                    ui->line_message->setText("Open device " + QString::number(camera_number_id));
                    cap->set(CAP_PROP_FRAME_WIDTH, size_wid);
                    cap->set(CAP_PROP_FRAME_HEIGHT, size_hei);
                }
                else if(wifi_link.length()!=0)
                {
                    cout << "wifi_link.c_str = " << wifi_link.c_str() << endl ;
                    cap->open(wifi_link.c_str());
                }
                    dem_time = 0;
        }
        CAMERA_OPEN_OK = false;
    }
    //cout << "image.empty() " << image.empty() << endl;

    if(image.empty()) {
        ui->screen_cam->setText("    No Camera");
        return;
    }
    //camera not work

    //cout << image.cols << "  " << image.rows << endl;
    if(UVC_OK_1==true)
    {
        CAMERA_OPEN_OK = true;
        //doi sang man hinh mo camera
        ui->stackedWidget->setCurrentIndex(1);
        //bao cao la da mo camera OK
        if(need_emit_UVC_OK==true){
            need_emit_UVC_OK = false;
            emit UVC_open_OK();
        }
        //cout << image.cols << "  " << image.rows << endl;
    }

    //cout  << "GMSM_run = " << GMSM_run <<endl;
    if(GMSM_run==true)
    {
        int dem = 0;
        Mat grayxx = store_blance.clone();
        for(int mode = 0; mode < 3; mode++)
        {
            process_screen_3pic(image, grayxx, mode, dem);
        }
    }
    Display(image);

}



bool MainWindow::GetCamera(Mat &image)
{
    image.release();

     cap->read(image);
    if (image.empty()) return false;

    //update lai
//    cout << size_wid << "  " << size_hei << endl;
//    cout << image.cols << "  " << image.rows << endl;

    if((size_wid!=image.cols)||(size_hei!=image.rows))
    {
        size_wid = image.cols;
        size_hei = image.rows;

        Setting_gomisimi_parameter();
    }


    return true;
}

void MainWindow::Display(Mat image)
{

    //resize de ra kich thuoc chuan
    bool resize = true;
    if(resize == true){
    int x, y;
    // convert gia tri
    if((size_hei*ui->screen_cam->width())>(size_wid*ui->screen_cam->height())){
        y = ui->screen_cam->height();
        x = y*size_wid/size_hei;
    }
    else{
        x = ui->screen_cam->width();
        y = x*size_hei/size_wid;
    }

    cv::resize(image,image,Size(x,y));
    }

    QPixmap p = Mat2QPixmap(image);
    ui->screen_cam->setPixmap(p);
}

QPixmap MainWindow::Mat2QPixmap(cv::Mat const& _frame)
{
    switch( _frame.type())
    {
    case CV_8UC4:
    {
        QPixmap p = QPixmap::fromImage(QImage(_frame.data,_frame.cols,_frame.rows,_frame.step,QImage::Format_RGB32));

        return p;
    }
    case CV_8UC3:
    {
        QPixmap p = QPixmap::fromImage(QImage(_frame.data,_frame.cols,_frame.rows,_frame.step,QImage::Format_RGB888).rgbSwapped());
        return p;
    }
    case CV_8UC1:
    {
        QPixmap p = QPixmap::fromImage(QImage(_frame.data,_frame.cols,_frame.rows,_frame.step,QImage::Format_Indexed8));
        return p;
    }
        break;
    }
    return QPixmap();

}

void MainWindow::Reset_Camera()
{
    cap->release();
    cap->~VideoCapture();
    ui->screen_cam->setText("     CHECK CAMERA");
}


void MainWindow::uvc_stream_start()
{
      cout << "uvc_stream_start "  << endl;

    CAMERA_OPEN_OK = false;

    Camera_UVC_start = true;

    //send uart
    char cmd[1000];
    sprintf(cmd, "%suvc_stream", file_conf.cmd_dir.c_str());
    uart_send(cmd);

    //send uart truoc de khoi dong camera, tu do tim ra so camera co the mo duoc
    QThread::sleep(1);// doi 1s de uvc khoi dong cho chac an

    //set camera number from sensor_id_file
    camera_number_id = 5;
    switch_camera = true;

    //camera setting
    cap = new VideoCapture();

    //open camera

    cout << "CAP_DSHOW + camera_number_id " << CAP_DSHOW + camera_number_id << endl;

    cap->open(CAP_DSHOW + camera_number_id);

    ui->line_message->setText("Open device " + QString::number(camera_number_id));

    cap->set(CAP_PROP_FRAME_WIDTH, size_wid);
    cap->set(CAP_PROP_FRAME_HEIGHT, size_hei);

    //setting gmsm para
    Setting_gomisimi_parameter();


    Timer_get_camera.start(100);
    UVC_had_run = true; // -> camera da tung run

}

void MainWindow::uvc_stream_stop()
{

  cout <<  "uvc_stream_stop " << endl;


//    Camera_UVC_start = false;
    //stop gmsm cho chac
    GMSM_run = false;

    //stop get camera
    if(Timer_get_camera.isActive()==true) Timer_get_camera.stop();

    //release camera
    Reset_Camera();


    //bat dau check xem UVC da stop ok chua(at uart get data)
    UVC_check_STOP = true;

    //send uart
    uart_send("quit");

    ui->stackedWidget->setCurrentIndex(0);


//    ui->textEdit->append("Stopping UVC stream . . . ");
    QEventLoop loop;
    connect(this, &MainWindow::UVC_cmd_EXIT_finished,  &loop, &QEventLoop::quit);


    //bat dau count_display
    //setting connect de out khoi loop khi timeout
    connect(this,SIGNAL(count_display_timeout()),  &loop, SLOT(quit()));
    //bat dau lai count
    Count_start = false;
    //set dem len or xuong. Dem xuong thi khi den 0 se tu dong stop
    Count_up = false;
    //set gia tri ban dau
    Count_start_num = 7;
    //print some infor
    ui->textEdit->append("Stopping UVC stream :  ");
    //start
    count_display_timer.start(1000);

    //start loop
    loop.exec();

    //tu thoat ra ngoai ok -> stop luon time_count
    count_display_timer.stop();


    //gia tri nay van la true -> time out. Neu out ok thi se emit UVC_cmd_EXIT_finished tai ham get uart debug
    if(UVC_check_STOP==true)
    {
//        ui->textEdit->moveCursor (QTextCursor::End);
//        ui->textEdit->insertPlainText ("Time out, force stop UVC stream");
        ui->textEdit->append("Time out, force stop UVC stream");
    }
    //set lai cho an toan neu trong truong hop out of time
    UVC_check_STOP = false;//set cai nay se anh huong den get uart_debug -> change start and end

    ui->textEdit->moveCursor (QTextCursor::End);
}

void MainWindow::UVC_message_check()
{

    cout << "UVC_message_check "  << endl;

//    WIFI_OK = send_message_yes_no("Cheking", "Please choose OK or NG");

    QMessageBox message_check_result;
    message_check_result.setWindowTitle("UVC stream check");
    message_check_result.setText("Please choose OK or NG");

    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    /*QPushButton *result_NG = */message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);


    //add spinbox
    QSpinBox* b_sw_cam = new QSpinBox();

    cout << "camera_number_id "  << camera_number_id << endl;

    b_sw_cam->setValue(camera_number_id);
    if (message_check_result.layout())
        message_check_result.layout()->addWidget(b_sw_cam);
    QPushButton *change_cam = message_check_result.addButton(tr("SW CAM"), QMessageBox::ActionRole);


    //message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->UVC->y() - 50);

    message_check_result.exec();

    while(message_check_result.clickedButton() == change_cam){
        camera_number_id = b_sw_cam->value();
        switch_camera = false; //ngan chan viec thay doi camera tu dong
        Reset_Camera();

        message_check_result.exec();
    }

    if (message_check_result.clickedButton() == result_OK) {
        cout << "press uvc ok " << endl;
        UVC_OK = true;
    }
    else UVC_OK = false;


//    uvc_stream_stop();
}

void MainWindow::UVC_wait_for_camera_number_appear()
{
    cv::VideoCapture* cap1;
    cap1 = new VideoCapture();
    cap1->open(CAP_DSHOW + camera_number_id);

    if(cap1->isOpened()==0) return;



    emit UVC_camera_number_appear();
}

void MainWindow::UVC_check_run()
{

    cout << "UVC_check_run" << endl;
    if(UVC_check==false) return;

    //Check if camera is turning on now
    if(Camera_UVC_start==true) return;


    //press cho chac an
    if(UVC_confirm==true)
    {
        QMessageBox messageBox1(QMessageBox::Warning,
                     tr("UVC confirm"),
                     tr("Press to continue"));
        messageBox1.exec();
    }



    //set button
    ui->textEdit->append(" - UVC stream :");

    //start uvc
    uvc_stream_start();

    //Check UVC open
    QEventLoop loop;
    connect(this, &MainWindow::UVC_open_OK,  &loop, &QEventLoop::quit);
    QTimer timer_loop;

    //bat dau count_display
    //setting connect de out khoi loop khi timeout
    connect(this,SIGNAL(count_display_timeout()),  &loop, SLOT(quit()));

    //bat dau lai count
    Count_start = false;

    //set dem len or xuong. Dem xuong thi khi den 0 se tu dong stop
    Count_up = false;
    //set gia tri ban dau
    Count_start_num = 7;


    //print some infor
    ui->textEdit->append("Opening UVC stream : ");
    //start
    count_display_timer.start(1000);

    //start loop
    need_emit_UVC_OK = true;
    loop.exec();

    //tu thoat ra ngoai ok -> stop luon time_count
    count_display_timer.stop();


    //time out ma van khong mo duoc camera
    if(CAMERA_OPEN_OK==false)
    {
        UVC_OK=false;
        ui->textEdit->append("Time out, can not open UVC camera");
        goto quit;
    }


    //send message check camera
    UVC_message_check();




    //check GMSM sau khi check UVC
    if((GMSM_check==true)&&(UVC_OK==true))
    {
        GMSM_run = true;

        ui->GMSM->setStyleSheet(button_process);
       //ui->GMSM_STATE->setStyleSheet(button_state_process);ui->UVC_STATE->setText("NG");

//        Send_message_check("GOMISIMI check", this->y() + this->ui->GMSM->y() - 50, GMSM_OK);
        GMSM_message_check();

        GMSM_run = false;

    }





quit:


    cout << "uvc_stream_stop" << endl;
    uvc_stream_stop();

    //FINISH
    if(UVC_OK==true)
    {
        ui->textEdit->append("UVC stream OK");
    }
    else
    {
        ui->textEdit->append("UVC stream ERROR");
        //ui->UVC_STATE->setStyleSheet(button_red);ui->UVC_STATE->setText("NG");
    }

    //FINISH
    if(GMSM_OK==true)
    {
        ui->textEdit->append("GOMISIMI OK");
        ui->GMSM_STATE->setStyleSheet(button_blue);ui->GMSM_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("GOMISIMI ERROR");
        ui->GMSM_STATE->setStyleSheet(button_red);ui->GMSM_STATE->setText("NG");
    }

    //setbutton

    ui->GMSM->setStyleSheet(button_set);


    //stop button
    Camera_UVC_start = false;

    return;
}

void MainWindow::on_UVC_clicked()
{
    if(LOCK_BUTTON==true) return;

    UVC_check_run();
}

void MainWindow::on_UVC_confirm_clicked(bool checked)
{
    UVC_confirm = checked;
}

//##############################################################################################



//GOMISIMI
//##############################################################################################

void MainWindow::Send_message_check(QString messenger, int pos_y, bool &Data_check)
{
//    WIFI_OK = send_message_yes_no("Cheking", "Please choose OK or NG");

    Send_message_check_result.setWindowTitle(messenger);
    Send_message_check_result.setText("Please choose OK or NG");


    QPushButton *result_OK = Send_message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    QPushButton *result_NG = Send_message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);

    Send_message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, pos_y);

    Send_message_check_result.exec();

    if (Send_message_check_result.clickedButton() == result_OK) {
        Data_check = true;
    }
    else Data_check = false;



    Send_message_check_result.removeButton(result_OK);
    Send_message_check_result.removeButton(result_NG);

//    uvc_stream_stop();
}

void MainWindow::GMSM_message_check()
{
////    WIFI_OK = send_message_yes_no("Cheking", "Please choose OK or NG");

//    QMessageBox message_check_result;
//    message_check_result.setWindowTitle("GOMISIMI check");
//    message_check_result.setText("Please choose OK or NG");

//    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
//    /*QPushButton *result_NG = */
//    message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);


//    //add spinbox
//    QSpinBox* b_sw_cam = new QSpinBox();

//    b_sw_cam->setValue(camera_number_id);

//    if (message_check_result.layout())        message_check_result.layout()->addWidget(b_sw_cam);


//    QPushButton *change_cam = message_check_result.addButton(tr("SW CAM"), QMessageBox::ActionRole);

//    message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->GMSM->y() - 50);

//    message_check_result.exec();

//    while(message_check_result.clickedButton() == change_cam)
//    {
//        camera_number_id = b_sw_cam->value();
//        switch_camera = false; //ngan chan viec thay doi camera tu dong
//        Reset_Camera();

//        message_check_result.exec();
//    }



//    if (message_check_result.clickedButton() == result_OK) {
//        GMSM_OK = true;
//    }
//    else GMSM_OK = false;





    QMessageBox message_check_result;
    message_check_result.setWindowTitle("GMSM check");
    message_check_result.setText("Please choose OK or NG");

    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    /*QPushButton *result_NG = */message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);



    message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->WIFI->y() - 50);

    message_check_result.exec();



//    if (message_check_result.clickedButton() == result_OK) {
//        LED_OK = true;
//    }
//    else LED_OK = false;


        if (message_check_result.clickedButton() == result_OK) {
            GMSM_OK = true;
        }
        else GMSM_OK = false;



}






// Get 3 hinh

void MainWindow::process_screen_3pic(Mat &mat_image, Mat &mat_output, int mode, int &dem){


    Mat mat_image1 = mat_image.clone();

    if(mat_image.type() != CV_8UC1)
    cvtColor(mat_image1,mat_image1, COLOR_BGR2GRAY);

    //Doi mau line cuoi cung
    if(1==1){
    for(int x = 0; x < size_wid; x++){
          for(int y = 0; y < size_hei; y++){
              if((y<size_hei)&&(y>=mat_image1.rows-1)) mat_image1.at<uchar>(y, x) = mat_image1.at<uchar>(y-1, x);
            }
        }
    }


    Mat dst1 = mat_image1.clone(); // PP1
    cv::GaussianBlur(dst1, dst1, cv::Size(15,15),7);

   //Get hinh nhi phan cho tung pp
    adaptiveThreshold(dst1,dst1,255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, h_thre[mode][0]*2+1,3);

//    mat_image = dst1.clone(); return;

    Mat dst2 = mat_image1.clone(); // PP2
    cv::threshold(dst2,dst2,h_thre[mode][1],255, THRESH_BINARY_INV);

    //load hinh cho tung mode
    store_blance.copyTo(dst1,store_h[mode]);
    store_blance.copyTo(dst2,store_h[mode]);


    //PP1
    vector<vector<Point> > contours;
    findContours( dst1, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    std::sort(contours.begin(), contours.end(), contour_sorter());

    //vi findcontours uu tien boc mau trang hon, nen chon che do inv, roi bo di nhung boc mau den se cho ket qua chinh xac hon
    int num_check_noise = 50;
    if(show_full==true) num_check_noise = contours.size();
    for(int i = 0;i<min((int)contours.size(),num_check_noise);i++)
    {
        //loai bo nhung hinh mau trang
        if (contourArea(contours[i],true) > 0) continue;

        //lenh dieu kien
        Rect mr = boundingRect(Mat(contours[i]));
        Point center = (mr.br() + mr.tl())*0.5;




        // loai nhung doan qua ngan
//        if(((mr.width<size_hei/30)&&(mr.height<size_hei/30))||
//           ((mr.width<size_hei/20)&&(mr.height<size_hei/40))||((mr.width<size_hei/40)&&(mr.height<size_hei/20)))
//        {continue;}

        if(((mr.width<10)&&(mr.height<10))){continue;}

        //loai nhung doan gan bien
        if((abs(center.y)<15)||(abs(center.y-size_hei)<15))
        {if((mr.height<size_hei/30)||(mr.width<size_hei/30))continue;}
        if((abs(center.x)<15)||(abs(center.x-size_wid)<15))
        {if((mr.width<size_hei/30)||(mr.height<size_hei/30)) continue;}

        if( ((abs(center.y)<65)||(abs(center.y-size_hei)<65))
        && ((abs(center.x)<65)||(abs(center.x-size_wid)<65)) )
        {
            int area = contourArea(contours[i]);
            if (area<600) continue;
        }

        drawContours( mat_output, contours, i, CV_RGB(255,255,255), FILLED);
    }



    //PP2
    // Find contours
    vector<vector<Point> > contours1;
    findContours( dst2, contours1, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    std::sort(contours1.begin(), contours1.end(), contour_sorter());
    if(show_full==true) num_check_noise = contours1.size();

    for(int i = 0; i< min((int)contours1.size(),num_check_noise);i++)
    {
        if (contourArea(contours1[i],true) > 0) continue;
        drawContours( mat_output, contours1, i, CV_RGB(255,255,255), FILLED);
    }


    if((mode==2)||(test_mode_on==true)){



        //xu ly nhieu nhap nhay, + hinh truoc de check
        if(1==1){
            if((!pre_noise_image.empty())&&(pre_noise_image.rows==mat_output.rows)){

                //luu trang thai cu
                Mat temp_pre_image; temp_pre_image = pre_noise_image.clone();
                pre_noise_image = mat_output.clone();


                //copy vo
                for(int x = 0; x < size_wid; x++){
                    for(int y = 0; y < size_hei; y++){
                        int a1 = mat_output.at<uchar>(y, x);
                        int a2 = temp_pre_image.at<uchar>(y, x);

                        if((a1==0)||(a2==0))  {mat_output.at<uchar>(y, x) = 0;}
                    }
                }
            }
            else pre_noise_image = mat_output.clone();
        }


        vector<vector<Point> > contours2;
        vector<Vec4i> hierarchy2;
        findContours( mat_output, contours2, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );


        std::sort(contours2.begin(), contours2.end(), contour_sorter());

        num_check_noise = 10;
        if(show_full==true) num_check_noise = contours2.size();

        for(int i = 0; i < min((int)contours2.size(), num_check_noise); i++)
        {
            if (contourArea(contours2[i],true) > 0) continue;

            drawContours( mat_image, contours2, i, CV_RGB(255,0,0), 2, 8, hierarchy2, 0, Point() );
        }
        dem = contours2.size();
        return;
    }

}


void MainWindow::Setting_gomisimi_parameter(){

    set_hig = size_hei;
    set_wid = size_wid;



    Mat test(cv::Size(set_wid, set_hig), CV_8U, Scalar(0,0,0));
    store_blance = test.clone();


//    cout << size_wid << "   " << size_hei << endl;

    //Setting default value

    for(int i=0; i<2; i++){
        store_h[i].release();
        temp_store[i].release();
    }




    h_thre[0][0] = 25;
    h_thre[0][1] = 30;

    h_thre[1][0] = 43;
    h_thre[1][1] = 69;

    h_thre[2][0] = 70;
    h_thre[2][1] = 95;


    wk1 = 90;
    wk2_num1 = 210; wk2_num2 = 1440; wk2_num3 = 90;
    wk3_num1 = 325; wk3_num2 = 165; wk3_num3 = 110;

    Create_base_waku_1(90);
    Create_base_waku_2(210, 1440, 90);
    Create_base_waku_3(325, 165, 110);
}



void MainWindow::Create_base_waku_1(double wk1){
    store_h[0] = store_blance.clone(); // nho nhat

    for(int x = 0; x < set_wid; x++){
        for(int y = 0; y < set_hig; y++){
            if(( x < (-y + wk1))) store_h[0].at<uchar>(y, x) = 255;

            if(( x < (y - (set_hig - wk1)))) store_h[0].at<uchar>(y, x) = 255;

            if(( x > (y + (set_wid - wk1)))) store_h[0].at<uchar>(y, x) = 255;

            if(( x > (-y + (set_wid + set_hig - wk1)))) store_h[0].at<uchar>(y, x) = 255;
        }
    }

    temp_store[0] = store_h[0].clone();

    if(!temp_store[1].empty()){
        Mat invSrc =  cv::Scalar::all(255) - temp_store[1];
        invSrc.copyTo(store_h[0], invSrc);
    }


}


void MainWindow::Create_base_waku_2(double wk2_num1, double wk2_num2, double wk2_num3){
    store_h[1] = store_blance.clone(); // o giua


    double tyle1 = wk2_num2/wk2_num3;
    double tyle2 = (wk2_num2*set_hig/set_wid)/wk2_num3;


    for(int x = 0; x < set_wid; x++){
        for(int y =0; y < set_hig; y++){

            if(( x < (-y*tyle1 + wk2_num2))) store_h[1].at<uchar>(y, x) =255;
            if(( x < (-y/tyle2 + wk2_num3))) store_h[1].at<uchar>(y, x) =255;
            if(( x < (-y + wk2_num1))) store_h[1].at<uchar>(y, x) =255;


            if(( x < (y*tyle1 - (set_hig*tyle1 - wk2_num2)))) store_h[1].at<uchar>(y, x) =255;
            if(( x < (y/tyle2 - (set_hig/tyle2 - wk2_num3)))) store_h[1].at<uchar>(y, x) =255;
            if(( x < (y - (set_hig-wk2_num1)))) store_h[1].at<uchar>(y, x) =255;

            if(( x > (y*tyle1 + (set_wid - wk2_num2)))) store_h[1].at<uchar>(y, x) =255;
            if(( x > (y/tyle2 + (set_wid - wk2_num3)))) store_h[1].at<uchar>(y, x) =255;
            if(( x > (y + (set_wid - wk2_num1)))) store_h[1].at<uchar>(y, x) =255;


            if(( x > (-y*tyle1 + (set_wid + set_hig*tyle1 - wk2_num2)))) store_h[1].at<uchar>(y, x) =255;
            if(( x > (-y/tyle2 + (set_wid + set_hig/tyle2 - wk2_num3)))) store_h[1].at<uchar>(y, x) =255;
            if(( x > (-y + (set_wid + set_hig - wk2_num1)))) store_h[1].at<uchar>(y, x) =255;
        }
    }

    temp_store[1] = store_h[1].clone();


    if(!temp_store[1].empty()){
        store_h[0] = temp_store[0].clone();
        Mat invSrc =  cv::Scalar::all(255) - temp_store[1];
        invSrc.copyTo(store_h[0], invSrc);
    }

    if(!store_h[2].empty()){
        Mat invSrc1 =  cv::Scalar::all(255) - store_h[2];
        invSrc1.copyTo(store_h[1], invSrc1);
    }
}


void MainWindow::Create_base_waku_3(double wk3_num1, double wk3_num2, double wk3_num3){
    store_h[2] = store_blance.clone(); // to nhat

    double heso = 2*(wk3_num2-wk3_num3)/set_hig;
    double heso2 = 2*(wk3_num2-wk3_num3)/set_wid;

    for(int x = 0; x < set_wid; x++){
        for(int y = 0; y < set_hig; y++){

        if(( x < (-y*heso + wk3_num2))) store_h[2].at<uchar>(y, x) = 255;
        if(( y < (-x*heso2 + wk3_num2))) store_h[2].at<uchar>(y, x) = 255;
        if(( x < (-y + wk3_num1))) store_h[2].at<uchar>(y, x) = 255;


        if(( x < (y*heso - (set_hig*heso - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( y > (x*heso2 + (set_hig - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( x < (y - (set_hig - wk3_num1)))) store_h[2].at<uchar>(y, x) = 255;


        if(( x > (y*heso + (set_wid - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( y < (x*heso2 - (set_wid*heso2 - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( x > (y + (set_wid - wk3_num1)))) store_h[2].at<uchar>(y, x) = 255;


        if(( x > (-y*heso + (set_wid + set_hig*heso - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( y > (-x*heso2 + (set_wid*heso2 + set_hig - wk3_num2) ))) store_h[2].at<uchar>(y, x) = 255;
        if(( x > (-y + (set_wid + set_hig - wk3_num1)))) store_h[2].at<uchar>(y, x) = 255;
        }
    }


    if(!store_h[2].empty()){
        store_h[1] = temp_store[1].clone();
        Mat invSrc1 =  cv::Scalar::all(255) - store_h[2];
        invSrc1.copyTo(store_h[1], invSrc1);
    }
}


void MainWindow::draw_wk(Mat &mat_image){

//    if(show_waku==false) return;

        //H1
    if(1){
    cv::line(mat_image, Point(wk1,0), Point(0,wk1), Scalar(0,120,0), 3, LINE_AA);
    cv::line(mat_image, Point(set_wid-wk1,0), Point(set_wid,wk1), Scalar(0,120,0), 3, LINE_AA);
    cv::line(mat_image, Point(wk1,set_hig), Point(0,set_hig-wk1), Scalar(0,120,0), 3, LINE_AA);
    cv::line(mat_image, Point(set_wid-wk1,set_hig), Point(set_wid,set_hig-wk1), Scalar(0,120,0), 3, LINE_AA);
    }

        //H2
    if(1){
    //        tim giao diem
        double number1 = wk2_num2;
        double tyle1 = wk2_num2/wk2_num3;
        double tyle2 = (wk2_num2*set_hig/set_wid)/wk2_num3;

        double point_y1 = (wk2_num1-wk2_num2)/(1-tyle1);;
        double point_x1 = wk2_num2 - point_y1*wk2_num2/wk2_num3;

        double point_y2 = (wk2_num1-wk2_num3)*tyle2/(tyle2-1);
        double point_x2 = -point_y2 + wk2_num1;





        if(point_x1<point_x2){
            double point_y3 = (wk2_num2-wk2_num3)*tyle2/(tyle1*tyle2-1);
            double point_x3 = -point_y3*tyle1 + wk2_num2;

            point_y1 = point_y3;
            point_x1 = point_x3;

            point_y2 = point_y3;
            point_x2 = point_x3;

    //            circle(mat_image, Point(point_x3,point_y3), 5, Scalar(255,0,0), 5, 8, 0);

        }


        double point_half = number1;
        double point_y_half = 0;

        double point_xhalf = 0;

        if(number1>set_wid/2) {
            point_half = set_wid/2;

            point_y_half = (2*number1 - set_wid)/2/tyle1;

            point_xhalf = -set_hig/2/tyle2 + wk2_num3;
        }


        cv::line(mat_image, Point(point_half,point_y_half), Point(point_x1,point_y1), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(point_x1,point_y1), Point(point_x2,point_y2), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(point_x2,point_y2), Point(point_xhalf,point_half*set_hig/set_wid), Scalar(255, 85, 0), 3, LINE_AA);

        cv::line(mat_image, Point(set_wid-point_half,point_y_half), Point(set_wid-point_x1,point_y1), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_x1,point_y1), Point(set_wid-point_x2,point_y2), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_x2,point_y2), Point(set_wid-point_xhalf,point_half*set_hig/set_wid), Scalar(255, 85, 0), 3, LINE_AA);

        cv::line(mat_image, Point(point_half,set_hig-point_y_half), Point(point_x1,set_hig-point_y1), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(point_x1,set_hig-point_y1), Point(point_x2,set_hig-point_y2), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(point_x2,set_hig-point_y2), Point(point_xhalf,set_hig-point_half*set_hig/set_wid), Scalar(255, 85, 0), 3, LINE_AA);

        cv::line(mat_image, Point(set_wid-point_half,set_hig-point_y_half), Point(set_wid-point_x1,set_hig-point_y1), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_x1,set_hig-point_y1), Point(set_wid-point_x2,set_hig-point_y2), Scalar(255, 85, 0), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_x2,set_hig-point_y2), Point(set_wid-point_xhalf,set_hig-point_half*set_hig/set_wid), Scalar(255, 85, 0), 3, LINE_AA);

    }

        //H3
    if(1){

    //        tim giao diem
        double heso = 2*(wk3_num2-wk3_num3)/set_hig;
        double heso2 = 2*(wk3_num2-wk3_num3)/set_wid;

        double point_Y = (wk3_num2-wk3_num1)/(heso-1);
        double point_X = -point_Y + wk3_num1;

        double point_Y2 = (wk3_num1*heso2-wk3_num2)/(heso2-1);
        double point_X2 = -point_Y2 + wk3_num1;


        double XX = wk3_num2*(heso-1)/(heso*heso2-1);

//        circle(mat_image, Point(XX,-XX*heso2 + wk3_num2), 5, Scalar(255,0,0), 5, 8, 0);
        if(XX >= wk3_num1/2){
            point_X = XX; point_Y = -XX*heso2 + wk3_num2;
            point_X2 = XX; point_Y2 = point_Y;
        }


        // 1/2 tren
        cv::line(mat_image, Point(wk3_num3,set_hig/2), Point(point_X,point_Y), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(point_X,point_Y), Point(point_X2,point_Y2), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(point_X2,point_Y2), Point(set_wid/2,wk3_num3), Scalar(255, 0, 170), 3, LINE_AA);

        cv::line(mat_image, Point(set_wid/2,wk3_num3), Point(set_wid-point_X2,point_Y2), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_X2,point_Y2), Point(set_wid-point_X,point_Y), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_X,point_Y), Point(set_wid-wk3_num3,set_hig/2), Scalar(255, 0, 170), 3, LINE_AA);

        // 1/2 duoi
        cv::line(mat_image, Point(wk3_num3,set_hig/2), Point(point_X,set_hig-point_Y), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(point_X,set_hig-point_Y), Point(point_X2,set_hig-point_Y2), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(point_X2,set_hig-point_Y2), Point(set_wid/2,set_hig-wk3_num3), Scalar(255, 0, 170), 3, LINE_AA);

        cv::line(mat_image, Point(set_wid/2,set_hig-wk3_num3), Point(set_wid-point_X2,set_hig-point_Y2), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_X2,set_hig-point_Y2), Point(set_wid-point_X,set_hig-point_Y), Scalar(255, 0, 170), 3, LINE_AA);
        cv::line(mat_image, Point(set_wid-point_X,set_hig-point_Y), Point(set_wid-wk3_num3,set_hig/2), Scalar(255, 0, 170), 3, LINE_AA);
    }


}


//##############################################################################################





//WIFI
//##############################################################################################
#define USE_BROWSER 0
void MainWindow::wifi_stream_start()
{
    WIFI_LINK_FOUND_ON = true;
    Camera_WIFI_start = true;

    //send uart
    char cmd[1000];
    char quit_cmd [1000];

    sprintf (cmd, "%shttp_stream", file_conf.cmd_dir.c_str());
    sprintf (quit_cmd, "%sstream_connect -q", file_conf.cmd_dir.c_str());

    cout << "file_conf = " <<  file_conf.cmd_dir.c_str() << endl;
    cout << "cmd = " << cmd << endl;
    cout << "quit_cmd = " << quit_cmd << endl;

//    uart_send(quit_cmd);
//    QThread::sleep(5); // 1 second

    uart_send(cmd);


}

void MainWindow::wifi_stream_stop()
{
    WIFI_LINK_FOUND_ON = false;

    uart_send("/hnbin/cmd/bin/stream_connect -q");
}

void MainWindow::WIFI_message_check()
{
//    WIFI_OK = send_message_yes_no("Cheking", "Please choose OK or NG");

    QMessageBox message_check_result;
    message_check_result.setWindowTitle("WiFi stream check");
    message_check_result.setText("Please choose OK or NG");

    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    /*QPushButton *result_NG = */message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);

#if USE_BROWSER
    QPushButton *reload = message_check_result.addButton(tr("Reload"), QMessageBox::ActionRole);
#endif

    message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->WIFI->y() - 50);

    message_check_result.exec();

#if USE_BROWSER
    while(message_check_result.clickedButton() == reload)
    {
        webview->reload();
        message_check_result.exec();
    }
#endif

    if (message_check_result.clickedButton() == result_OK) {
        WIFI_OK = true;
    }
    else WIFI_OK = false;


//    wifi_stream_stop();
}

//thay doi trang thai cuar auto set STATION mode
void MainWindow::on_STATION_mode_auto_clicked(bool checked)
{
    WIFI_STATION_check_EN = checked;
}

#include <QProcess>


void MainWindow::WIFI_check_run()
{






    cout << "WIFI_check_run " << endl;

    if(WIFI_STATION_check_EN==true)
    {
        cout << "WIFI_check_run " << endl;

        WIFI_STATION_check = true;
        //set station_wifi first
        uart_send(QString::fromStdString(file_conf.cmd_dir) + "wifi_setting -m 1");
        //set thoi gian de wifi OK -> 8s
        QEventLoop loop_x;
        connect(this, &MainWindow::UVC_cmd_EXIT_finished,  &loop_x, &QEventLoop::quit);

        //bat dau count_display
        //setting connect de out khoi loop khi timeout
        connect(this,SIGNAL(count_display_timeout()),  &loop_x, SLOT(quit()));
        //bat dau lai count
        Count_start = false;
        //set dem len or xuong. Dem xuong thi khi den 0 se tu dong stop
        Count_up = true;
        //set gia tri ban dau
        Count_start_num = 0;
        //print some infor
        ui->textEdit->append("Setting Wifi STATION mode :  ");
        //start
        count_display_timer.start(1000);

        //start loop
        loop_x.exec();

        //tu thoat ra ngoai ok -> stop luon time_count
        count_display_timer.stop();
    }


    //set button
    ui->WIFI->setStyleSheet(button_process);
    ui->WIFI_STATE->setStyleSheet(button_state_process);
    ui->WIFI_STATE->setText("NG");

    DEBUG_num_data_get = 1;
    DEBUG_get_data_on = true;

    // Check http_stream
    // wifi_stream_start();

    //load by opencv

//    QString stop_stream = "/hnbin/cmd/bin/stream_connect -q";
//    uart_send(stop_stream);
//    QThread::sleep(3);
//    QString start_stream = "/hnbin/cmd/bin/stream_connect -c http_stream";
//    uart_send(start_stream);


    ui->screen_cam->setText("No Camera");
    cap = new VideoCapture();
    //open camera
    String a = "http://192.168.1.10:8080/?action=stream";       // wifi_link.c_str();
    cap->open(a);
    // cap->open(wifi_link.c_str());

    ui->line_message->setText(QString::fromStdString(a));
    Timer_get_camera.start(0);

    //FINISH
//    if(WIFI_OK==true)
//    {
//       ui->textEdit->append("WiFi stream OK");
//       ui->WIFI_STATE->setStyleSheet(button_blue);
//       ui->WIFI_STATE->setText("OK");
//    }
//    else
//    {
//       ui->textEdit->append("WiFi stream ERROR 111 ");
//       ui->WIFI_STATE->setStyleSheet(button_red);ui->WIFI_STATE->setText("NG");
//    }

    //setbutton
//    ui->WIFI->setStyleSheet(button_set);

//    //Tao 1 loop de doi du lieu tra ve tu UART_debug
//    QEventLoop loop, loop1;
//    connect(this, &MainWindow::receive_uart_data_ok,  &loop, &QEventLoop::quit);
//    QTimer timer_loop;
//    timer_loop.setSingleShot(true);
//    connect(&timer_loop,SIGNAL(timeout()),  &loop, SLOT(quit()));


//    //Get link
//    //set thoi gian check
//    timer_loop.start(time_out_check);
//    loop.exec();

//    cout << "WIFI_check_run 44 " << endl;

//    //check time. Neu out khoi wifi ma khong co du lieu -> time out
////    if(data_get.size()<DEBUG_num_data_get)
////    {
////        ui->textEdit->append("Time out, can not open WiFi stream");
//////      goto quit;
////    }

//    cout << "WIFI_check_run 55 " << endl;

//    //cho them 2s de http load ok
//    timer_loop.start(500);//wait 0.5s
//    loop.exec();
////    Open link WIFI
//    cout << "WIFI_check_run 66 " << endl;

//    ui->textEdit->append("Link for cheking WiFi stream :");//QString::fromStdString(start_key));
//    cout << "WIFI_check_run 66 " << endl;

//    wifi_link = data_get[DEBUG_num_data_get-1];
//    cout << "WIFI_check_run 77 " << endl;
//    ui->textEdit->append(QString::fromStdString(wifi_link));
//    timer_loop.start(1500);         //wait 1.5s de hien thi link
//    loop.exec();

//    cout << "WIFI_check_run 88 " << endl;


    //send message
    if (just_check_GMSI == false)    WIFI_message_check();
        //FINISH
    if(WIFI_OK==true)
    {
        ui->textEdit->append("WiFi stream OK");
        ui->WIFI_STATE->setStyleSheet(button_blue);ui->WIFI_STATE->setText("OK");
    }
    else
    {
        ui->textEdit->append("WiFi stream ERROR 111");
        ui->WIFI_STATE->setStyleSheet(button_red);ui->WIFI_STATE->setText("NG");
    }
    //setbutton
    ui->WIFI->setStyleSheet(button_set);
//    cout << "line_message 555 " << endl;

    // add
    if (just_check_wifi == false )
    {
                Setting_gomisimi_parameter();
                //check GMSM sau khi check camera
                if((GMSM_check==true)&&(WIFI_OK==true))
                    {
                        GMSM_run = true;
                        ui->GMSM->setStyleSheet(button_process);
                        GMSM_message_check();
                        GMSM_run = false;
                    }

                //end

                //FINISH
                if(GMSM_OK==true)
                {
                    ui->textEdit->append("GOMISIMI OK");
                    ui->GMSM_STATE->setStyleSheet(button_blue);
                    ui->GMSM_STATE->setText("OK");
                }
                else if (GMSM_check==false)
                {
                    ui->textEdit->append("GOMISIMI NOT CHECK");
                    ui->GMSM_STATE->setStyleSheet(button_red);
                }
                else
                {
                    ui->textEdit->append("GOMISIMI ERROR");
                    ui->GMSM_STATE->setStyleSheet(button_red);
                    ui->GMSM_STATE->setText("NG");
                }
                ui->GMSM->setStyleSheet(button_set);
    }
    Timer_get_camera.stop();
    ui->stackedWidget->setCurrentIndex(0);

//    QThread::sleep(5);

quit:
    //time out thi cung nen thoat ntn cho chac
    cout << "run to quit "  << endl;
//    wifi_stream_stop();
    //bat dau count_display
    //setting connect de out khoi loop khi timeout
//    connect(this,SIGNAL(count_display_timeout()),  &loop, SLOT(quit()));

    //bat dau lai count
    Count_start = false;

    //set dem len or xuong. Dem xuong thi khi den 0 se tu dong stop
    Count_up = false;
    //set gia tri ban dau
    Count_start_num = 1;
    //print some infor
    ui->textEdit->append("Stopping WiFi stream :  ");
    //start
    count_display_timer.start(1000);
//    loop.exec();
    //doan nay chi cho 3s thoi nen khong can stop count

//  release data
    vector<string>().swap(data_get);
    //stop
    Camera_WIFI_start = false;

    return;
}


// LED check

void MainWindow::LED_message_check()
{

//    WIFI_OK = send_message_yes_no("Cheking", "Please choose OK or NG");
    QMessageBox message_check_result;
    message_check_result.setWindowTitle("LED check");
    message_check_result.setText("Please choose OK or NG");

    QPushButton *result_OK = message_check_result.addButton(tr("OK"), QMessageBox::ActionRole);
    /*QPushButton *result_NG = */message_check_result.addButton(tr("NG"), QMessageBox::ActionRole);

#if USE_BROWSER
    QPushButton *reload = message_check_result.addButton(tr("Reload"), QMessageBox::ActionRole);
#endif

    message_check_result.move(this->x() + ui->verticalFrame_2->x() + 10, this->y() + this->ui->WIFI->y() - 50);

    message_check_result.exec();

#if USE_BROWSER
    while(message_check_result.clickedButton() == reload)
    {
        webview->reload();
        message_check_result.exec();
    }
#endif

    if (message_check_result.clickedButton() == result_OK) {
        LED_OK = true;
    }
    else LED_OK = false;
//    wifi_stream_stop();

}


void MainWindow::LED_check_run()
{
    QString LED_BR_cmd = "/hnbin/cmd/bin/shell-ele_PWM -p" ;

    QString LED_BR =  ui->LED_Brightness->toPlainText();

    QString LED_BR_SET = LED_BR_cmd + " " + LED_BR ;

    cout << "LED_BR  " << LED_BR_SET.toUtf8().data() << endl ;

    uart_send(LED_BR_SET);

    QString pwm_cmd = "/hnbin/cmd/bin/shell-ele_PWM -e 1" ;

    uart_send(pwm_cmd );

    LED_message_check();

    //FINISH

    if(LED_OK==true)
    {
        ui->textEdit->append("LED OK");

        ui->LED_STATE->setStyleSheet(button_blue);

        ui->LED_STATE->setText("OK");
    }
    else
    {
//        ui->textEdit->append("WiFi stream ERROR");

        ui->LED_STATE->setStyleSheet(button_red);

        ui->LED_STATE->setText("NG");
    }

    QString pwm_cmd_off = "/hnbin/cmd/bin/shell-ele_PWM -e 0" ;
    uart_send(pwm_cmd_off );

    return;
}


void MainWindow::on_WIFI_clicked()
{
    just_check_wifi = true;

    ui->WIFI_STATE->setText("NG");

    WIFI_check_run();

    just_check_wifi = false;


    bool ALL_STATE_CHECK = true;
    string ALL_state_test;

    ALL_state_test = ui->WIFI_STATE->text().toUtf8().constData();
    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(WIFI_check==true)) ALL_STATE_CHECK = false;
    cout << "ALL_state_test 11   "  << ALL_state_test << endl;

    ALL_state_test = ui->GMSM_STATE->text().toUtf8().constData();
    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(GMSM_check==true)) ALL_STATE_CHECK = false;
    cout << "ALL_state_test 22  "  << ALL_state_test << endl;

    ALL_state_test = ui->LED_STATE->text().toUtf8().constData();
    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(check_LED==true)) ALL_STATE_CHECK = false;
    cout << "ALL_state_test 33   "  << ALL_state_test << endl;


    if(ALL_STATE_CHECK==true)
    {
        ui->START_STATE->setStyleSheet(button_blue);
        ui->START_STATE->setText("OK");
    }
    else
    {
        ui->START_STATE->setStyleSheet(button_red);ui->START_STATE->setText("NG");
    }





}

//##############################################################################################

//WEB_init
//##############################################################################################
void MainWindow::adjustLocation()
{
    //locationEdit->setText(webview->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl::fromUserInput(locationEdit->text());
//    webview->load(url);
//    webview->setFocus();
}
//##############################################################################################



//SETTING
//##############################################################################################
void MainWindow::on_actionSetting_triggered()
{
    ui->stackedWidget->setCurrentIndex(2);
    SETTING_ON = true;
}

void MainWindow::get_setting_para()
{
    int ret = file_conf.load_from_conf();

    if(ret==-1)
    {
        SETTING_CONF_OK = false;
        return;
    }
    else
    {
        SETTING_CONF_OK = true;
    }


    //Sau khi get xong thi bat dau apply cac data do vao mainwindow
//#######################################################################################
    ui->line_DEBUG_PORT->setText(QString::fromStdString(file_conf.DEBUG_PORT));
    ui->line_cmd->setText(QString::fromStdString(file_conf.cmd_shell));
    ui->line_cmd_dir->setText(QString::fromStdString(file_conf.cmd_dir));


//    cai nay khong chen len tren ui duoc vi build e bi loi nen chen vao day
//    ui->lineEdit_9->setText("[1;35m/dev/mmcblk0p1[0m");


    //choose check
    on_SD_check_clicked(atoi(file_conf.SD_CARD_check.c_str()));
    on_GPIO_check_clicked(atoi(file_conf.GPIO_check.c_str()));
    on_SPI_check_clicked(atoi(file_conf.SPI_check.c_str()));
    on_UART_check_clicked(atoi(file_conf.UART_check.c_str()));
    on_I2C_check_clicked(atoi(file_conf.I2C_check.c_str()));
    on_UVC_check_clicked(atoi(file_conf.UVC_check.c_str()));
    on_GMSM_check_clicked(atoi(file_conf.GMSM_check.c_str()));
    on_WIFI_check_clicked(atoi(file_conf.WIFI_check.c_str()));

    //on_LED_check_clicked(atoi(file_conf.LED_check.c_str()));

    on_Check__LED_clicked(atoi(file_conf.LED_check.c_str()));


    //GPIO conf
    on_GPIO_mode_clicked(atoi(file_conf.GPIO_mode.c_str()));

 //   ui->line_GPIO_port->setText(QString::fromStdString(file_conf.GPIO_port));
//#######################################################################################

    //tinh toan lai GPIO_conf
    GPIO_NUM_PORT = 0;

    stringstream space_line_stream(file_conf.GPIO_port);
    int get_element_int;


    QString save_for_test = "";
    while(space_line_stream>>get_element_int)
    {
        for(unsigned int i=0; i < GPIO_NUM_PORT; i++)
        {
            //kiem tra xe co bi trung hay khong
            if(GPIO_DEF_POS[i]==get_element_int)
            {
//                SETTING_CONF_OK = false; return;
//                update lai file_conf
                file_conf.GPIO_port = save_for_test.toUtf8().constData();
                //ui->line_GPIO_port->setText(save_for_test);

                goto exit_while;
            }
        }
        save_for_test += QString::number(get_element_int) + " ";
        GPIO_DEF_POS[GPIO_NUM_PORT] = get_element_int;
        GPIO_NUM_PORT++;
        continue;
        exit_while:
//        ui->textEdit->append("GPIO_port error, fixed");
        break;
    }



    return;
}

void MainWindow::SETTING_load_from_line_edit()
{
    file_conf.DEBUG_PORT = ui->line_DEBUG_PORT->text().toUtf8().constData();


    file_conf.cmd_shell = ui->line_cmd->text().toUtf8().constData();
    file_conf.cmd_dir   = ui->line_cmd_dir->text().toUtf8().constData();

    //file_conf.sd_card_dir_full = ui->line_sd_dir->text().toUtf8().constData();
    //phan tich ra
    if(1)
    {
        stringstream abc(file_conf.sd_card_dir_full);
        abc >> file_conf.sd_card_dir;
        abc >> file_conf.sd_card_dir_mount;
    }

//    setting con fig



    if(ui->GMSM_check->isChecked()) file_conf.GMSM_check = "1";
    else file_conf.GMSM_check = "0";

    cout << "GMSM setting 1 " << endl;

    if(ui->WIFI_check->isChecked()) file_conf.WIFI_check = "1";
    else file_conf.WIFI_check = "0";


    if(ui->Check__LED->isChecked()) file_conf.LED_check = "1";
    else file_conf.LED_check = "0";


    //tinh toan lai GPIO_conf
    GPIO_NUM_PORT = 0;

    stringstream space_line_stream(file_conf.GPIO_port);
    int get_element_int;


    QString save_for_test = "";
    while(space_line_stream>>get_element_int)
    {
        for(unsigned int i=0; i < GPIO_NUM_PORT; i++)
        {
            //kiem tra xe co bi trung hay khong
            if(GPIO_DEF_POS[i]==get_element_int)
            {
//                SETTING_CONF_OK = false; return;

                //update lai file_conf
                file_conf.GPIO_port = save_for_test.toUtf8().constData();
//                ui->line_GPIO_port->setText(save_for_test);

                goto exit_while;
            }
        }
        save_for_test += QString::number(get_element_int) + " ";
        GPIO_DEF_POS[GPIO_NUM_PORT] = get_element_int;
        GPIO_NUM_PORT++;
        continue;
        exit_while:
//        ui->textEdit->append("GPIO_port error, fixed");
        break;
    }

    return;

}

void MainWindow::on_SETTING_apply_clicked()
{
    SETTING_load_from_line_edit();

    //Set name again
    this->setWindowTitle(this->windowTitle() + "  " + QString::fromStdString(file_conf.DEBUG_PORT));

    ui->textEdit->clear();
    ui->textEdit->append("Setting DEBUG_PORT = " + QString::fromStdString(file_conf.DEBUG_PORT));

    //stop cac qtimer and start again
    timer_uart_setting_wait.stop();
    count_display_timer.stop();

    timer_uart_setting_wait.start(1000);

    ui->stackedWidget->setCurrentIndex(0);
    SETTING_ON = false;
}


void MainWindow::on_SETTING_load_clicked()
{
    get_setting_para();
//    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_SETTING_save_clicked()
{
    SETTING_load_from_line_edit();
    file_conf.save_to_conf();

//    ui->textEdit->clear();
//    ui->textEdit->setText("Save SETTING_CONF");
//    ui->stackedWidget->setCurrentIndex(0);
    SETTING_ON = false;
}


void MainWindow::on_SETTING_exit_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    SETTING_ON = false;
}

//##############################################################################################







//Control
//##############################################################################################

#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QTreeWidgetItem>


void MainWindow::on_START_clicked()
{

    QProcess process;
//    process.setWorkingDirectory("D:\\MyWork\\Temp\\source");
    process.start("netsh wlan show profile");


    QNetworkConfiguration cfg;
    QNetworkConfigurationManager ncm;
    auto nc = ncm.allConfigurations();

    for (auto &x : nc)
    {
        if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
        {
            qDebug() << x.name() << " is a WIFI";
            if (x.name() == "Desired Network")
                qDebug() <<"Connecting to "<< x.name();
                cfg = x;
        }
        else
            qDebug() << x.name() << "\n";
    }

    return ;



//    QNetworkConfigurationManager mgr;
//       QNetworkConfiguration ap = mgr.defaultConfiguration();
//       QNetworkSession *session = new QNetworkSession(ap); //code activates session
//       QString ident = session->sessionProperty("ActiveConfiguration").toString();
//       if ( ap.type() == QNetworkConfiguration::ServiceNetwork ) {
//           Q_ASSERT( ap.identifier() != ident );
//           Q_ASSERT( ap.children().contains( mgr.configurationFromIdentifier(ident) ) );
//       } else if ( ap.type() == QNetworkConfiguration::InternetAccessPoint ) {
//           Q_ASSERT( ap.identifier() == ident );
//       }


     // test code
        QNetworkConfigurationManager mgr;
        QList<QNetworkConfiguration> activeConfigs = mgr.allConfigurations(QNetworkConfiguration::Active);

        if (activeConfigs.count() > 0)
        {
            cout << "count > 0" << endl;
            Q_ASSERT(mgr.isOnline());
        }
        else
            Q_ASSERT(!mgr.isOnline());


        return;





//        QNetworkConfiguration cfg;
//        QNetworkConfigurationManager ncm;
//        auto nc = ncm.allConfigurations();

//        for (auto &x : nc)
//        {
//            if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
//            {
//                cout << "WiFi = " << x.name().toStdString() << endl;
//                if (x.name() == "YouDesiredNetwork")
//                    cfg = x;
//            }
//        }

//        auto session = new QNetworkSession(cfg, this);
//        session->open();

//        return;



//    QList<QNetworkConfiguration> netcfgList;
//    int foundCount;
//    QNetworkConfiguration netcfg;
//    QStringList WiFisList;

//    QNetworkConfigurationManager ncm;
//    netcfgList = ncm.allConfigurations();

//    WiFisList.clear();
//        for (auto &x : netcfgList)
//        {
//            if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
//            {
//                if(x.name() == "")
//                    WiFisList << "Unknown(Other Network)";
//                else
//                   {
//                        cout << "wifi name" << endl;
//                        cout << x.name().toStdString() << endl;
//                        WiFisList << x.name();
//                    }
//                qDebug() << x.type();
//            }
//        }
//        qDebug() << WiFisList[4];

//        for(int i=0; i<WiFisList.size(); i++)
//        {
//            bool exist = false;
//            QTreeWidgetItem * item = new QTreeWidgetItem();
////            for(int j=0; j<ui->treeWidgetWiFis->topLevelItemCount(); j++)
////            {
////                QTreeWidgetItem *index = ui->treeWidgetWiFis->topLevelItem(j);
////                QString str = index->text(1);
////                if(str == WiFisList[i])
////                {
////                    exist = true;
////                    break;
////                }
////            }
//            if(!exist)
//            {
//                item->setTextAlignment(0,Qt::AlignVCenter);
//                item->setTextAlignment(1,Qt::AlignHCenter);
//                item->setText(0,QString::number(++foundCount));
//                item->setText(1,WiFisList[i]);
////                ui->treeWidgetWiFis->addTopLevelItem(item);
//            }
//        }
//    return;


    const QString IP ="192.168.1.10";
    QProcess pingProcess;
    QString exec = "ping";
    QStringList params;
    params << "-n" << "1" << IP;
    pingProcess.start(exec,params,QIODevice::ReadOnly);
    pingProcess.waitForFinished(-1);

    QString p_stdout = pingProcess.readAllStandardOutput();
    QString p_stderr = pingProcess.readAllStandardError();
    string  ping_eror = "Destination host unreachable";

    if (p_stdout.toStdString().find(ping_eror) != std::string::npos)
    {
       cout << "WiFi is not connected!!" << endl;
       ui->textEdit->setText("Please, Connect the PC's WiFi to the streaming board ");
       return;
    }
    else
    {
        cout << "WiFi is connect " << endl;
    }




    if(DEBUG_OPEN_OK==false) return;

    if(START_RUNNING==true) return;

    START_RUNNING = true;
    //reset all
    on_actionReset_triggered();

    //set button
    ui->START->setStyleSheet("background-color: rgb(100,160,132)");
    ui->START_STATE->setStyleSheet(button_state_process);ui->WIFI_STATE->setText("NG");



    //do
    ui->textEdit->append("### IO CHECK : START");

    string ALL_state_test;
    string ALL_GMSM_test;

    bool ALL_STATE_CHECK = true;

    cout << "check WiFi......." << endl;
    // check WiFi
    WIFI_check_run();
    ALL_state_test = ui->WIFI_STATE->text().toUtf8().constData();
    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(WIFI_check==true)) ALL_STATE_CHECK = false;

    ALL_state_test = ui->GMSM_STATE->text().toUtf8().constData();
    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(GMSM_check==true)) ALL_STATE_CHECK = false;


    cout << "check LED ...... " << endl;
    // check LED
    LED_check_run ();
    ALL_state_test = ui->LED_STATE->text().toUtf8().constData();

    if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(check_LED==true)) ALL_STATE_CHECK = false;

    ui->textEdit->append("### IO CHECK : STOP");




    // check DUTY PWM
    QString duty_cmd = "/hnbin/cmd/bin/shell-ele_PWM -p ";
    QString ON_LED = "/hnbin/cmd/bin/shell-ele_PWM -e 1";
    QString OFF_LED = "/hnbin/cmd/bin/shell-ele_PWM -e 0";
    QString run_cmd ;


    uart_send(ON_LED);
//    QThread::sleep(5);
//    uart_send(OFF_LED);

//    int i = 40;
//    while (1)
//    {
//        i = i + 5;
//        run_cmd = duty_cmd + QString::number(i);
////        uart_send(run_cmd);
//        if ( i >= 95 ) break ;
//        cout << "sleep 1" << endl;
//        QThread::sleep(1);

//    }




////    uart_send(LED_STA + QString::number(1));

//    for(int i = 30 ; i < 100; i + 5 )
//    {
//        cout << "i = " << i << endl;
//        duty_cmd = duty_cmd + QString::number(i);
//        uart_send(duty_cmd);

//    }

//    uart_send(LED_STA + QString::number(0));



    //set button
    ui->START->setStyleSheet(button_START);


    if(ALL_STATE_CHECK==true)
    {
        ui->START_STATE->setStyleSheet(button_blue);
        ui->START_STATE->setText("OK");
    }
    else
    {
        ui->START_STATE->setStyleSheet(button_red);ui->START_STATE->setText("NG");
    }

    START_RUNNING = false;



}

//Lock and unlock
void MainWindow::on_actionLock_button_triggered()
{
    LOCK_BUTTON = true;
    ui->actionLock_button->setVisible(false);
    ui->actionUnlock_button->setVisible(true);


    button_set = button_lock;

    on_actionReset_triggered();

}

void MainWindow::on_actionUnlock_button_triggered()
{
    if(DEBUG_OPEN_OK==false) return;

    LOCK_BUTTON = false;
    ui->actionLock_button->setVisible(true);
    ui->actionUnlock_button->setVisible(false);

    button_set = button_unlock;

    on_actionReset_triggered();

}

//Reset
void MainWindow::on_actionReset_triggered()
{
    //reset all
    ui->textEdit->clear();

    //reset giao dien


    //ui->UART->setStyleSheet(button_set);

    //ui->UVC->setStyleSheet(button_set);
    ui->GMSM->setStyleSheet(button_lock); //thang nay khong cho xai, luc nao cung lock het
    ui->WIFI->setStyleSheet(button_set);
    ui->LED->setStyleSheet(button_set);

    ui->WIFI_STATE->setStyleSheet(button_gray);
    ui->WIFI_STATE->setText("NG");


    ui->START->setStyleSheet(button_START);
    ui->START_STATE->setStyleSheet(button_gray);
    ui->WIFI_STATE->setText("NG");


    //reset timer
    //stop gmsm cho chac
    GMSM_run = false;

    //stop get camera
    if(Timer_get_camera.isActive()==true) Timer_get_camera.stop();


    //stop count_display
    if(count_display_timer.isActive()==true) count_display_timer.stop();

    //chuyen sang man hinh chinh
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_SD_check_clicked(bool checked)
{
    SD_check = checked;
}

//GPIO SPI PAIR
void MainWindow::on_GPIO_check_clicked(bool checked)
{
    GPIO_check = checked;

    //Neu ca SPI va GPIO cung on thi chi co the set GPIO blink
    if((GPIO_check==true)&&(SPI_check==true)&&(SETTING_ON==true))
    {
        on_GPIO_mode_clicked(false);
//        ui->line_GPIO_port->setText("22");

    }
    if((GPIO_check==true)&&(SPI_check==false)&&(SETTING_ON==true))
    {
        on_GPIO_mode_clicked(true);
//        ui->line_GPIO_port->setText("0 1 2 3 22");
    }
}

void MainWindow::on_SPI_check_clicked(bool checked)
{
    SPI_check = checked;


    //Neu ca SPI va GPIO cung on thi chi co the set GPIO blink
    if((GPIO_check==true)&&(SPI_check==true)&&(SETTING_ON==true))
    {
        on_GPIO_mode_clicked(false);
//        ui->line_GPIO_port->setText("22");

    }
    if((GPIO_check==true)&&(SPI_check==false)&&(SETTING_ON==true))
    {
        on_GPIO_mode_clicked(true);
//        ui->line_GPIO_port->setText("0 1 2 3 22");
    }
}

//UART I2C PAIR
void MainWindow::on_UART_check_clicked(bool checked)
{
    UART_check = checked;
    //ui->UART_check->setChecked(checked);
    //ui->UART->setHidden(!checked);
    //ui->UART_STATE->setHidden(!checked);

    //cout << "UART_check_clicked" << endl;

    if(checked==true)
    {
        I2C_check = !checked;

    }
}

void MainWindow::on_I2C_check_clicked(bool checked)
{
    I2C_check = checked;

    if(checked==true)
    {
        UART_check = !checked;
        //ui->UART_check->setChecked(!checked);
        //ui->UART->setHidden(checked);
        //ui->UART_STATE->setHidden(checked);
    }
}

void MainWindow::on_UVC_check_clicked(bool checked)
{
    UVC_check = checked;
    //ui->UVC_check->setChecked(checked);
    //ui->UVC->setHidden(!checked);
    //ui->UVC_STATE->setHidden(!checked);

    //if(UVC_check==false) on_GMSM_check_clicked(false);
}


void MainWindow::on_GMSM_check_clicked(bool checked)
{
    GMSM_check = checked;

    ui->GMSM_check->setChecked(checked);
    ui->GMSM->setHidden(!checked);
    ui->GMSM_STATE->setHidden(!checked);

}

void MainWindow::on_WIFI_check_clicked(bool checked)
{
    WIFI_check = checked;

    ui->WIFI_check->setChecked(checked);

    ui->WIFI->setHidden(!checked);

    ui->WIFI_STATE->setHidden(!checked);
}
//##############################################################################################

void MainWindow::on_Check__LED_clicked(bool checked)
{
    check_LED = checked;

    ui->Check__LED->setChecked(checked);

    ui->LED->setHidden(!checked);

    ui->LED_STATE->setHidden(!checked);

}


void MainWindow::on_SSID_PW_CHANGE_clicked()
{
    QString SSID =  ui->SSID_text->toPlainText();

    QString Pass =  ui->PassWord_text->toPlainText();

    QString SetWifi = "wifi_setting -m 1 -s " + SSID + " " + Pass;

    //cout << "SetWifi " << SetWifi.toUtf8().data() << endl;

    uart_send(SetWifi);


    //QString Station_mode = "wifi_setting -m 0 -d 1 -s HTC-network r686i4ktkbtbu" ;


    //uart_send(Station_mode );



     QEventLoop loop;

    cout << "run to quit "  << endl;
    wifi_stream_stop();

    //bat dau count_display
    //setting connect de out khoi loop khi timeout
    connect(this,SIGNAL(count_display_timeout()),  &loop, SLOT(quit()));

    //bat dau lai count
    Count_start = false;

    //set dem len or xuong. Dem xuong thi khi den 0 se tu dong stop
    Count_up = false;

    //set gia tri ban dau
    Count_start_num = 3;

    //print some infor
    ui->textEdit->append("Wait for change to AP mode :  ");

    //start
    count_display_timer.start(1000);
    loop.exec();
    //doan nay chi cho 3s thoi nen khong can stop count





}

void MainWindow::on_LED_clicked()
{

        LED_check_run ();

        bool ALL_STATE_CHECK = true;
        string ALL_state_test;

        ALL_state_test = ui->WIFI_STATE->text().toUtf8().constData();
        if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(WIFI_check==true)) ALL_STATE_CHECK = false;
        cout << "ALL_state_test 11   "  << ALL_state_test << endl;

        ALL_state_test = ui->GMSM_STATE->text().toUtf8().constData();
        if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(GMSM_check==true)) ALL_STATE_CHECK = false;
        cout << "ALL_state_test 22  "  << ALL_state_test << endl;

        ALL_state_test = ui->LED_STATE->text().toUtf8().constData();
        if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(check_LED==true)) ALL_STATE_CHECK = false;
        cout << "ALL_state_test 33   "  << ALL_state_test << endl;


        if(ALL_STATE_CHECK==true)
        {
            ui->START_STATE->setStyleSheet(button_blue);
            ui->START_STATE->setText("OK");
        }
        else
        {
            ui->START_STATE->setStyleSheet(button_red);ui->START_STATE->setText("NG");
        }



}


void MainWindow::on_GMSM_clicked()
{
    cout << "on_GMSM_clicked "<< endl;

//    //neu setting file khong OK, return
//    if(SETTING_CONF_OK==false) return;

//    serialPort->close();
//    serialPort->setPortName(QString::fromStdString(file_conf.DEBUG_PORT));
//    serialPort->open(QIODevice::ReadWrite);
//    if (!serialPort->isOpen()) {
//        //bat dau count_display
//        if(count_display_timer.isActive()==false) {
//            //bat dau lai count
//            Count_start = false;
//            //set dem len or xuong
//            Count_up = true;
//            //set gia tri ban dau
//            Count_start_num = 1;
//            //print some infor
//            ui->textEdit->append("UART DEBUG CONNECT ERROR   ");
//            //start
//            count_display_timer.start(1000);
//        }
//        DEBUG_OPEN_OK = false;
//        return;
//    }
//    //stop count_display
//    if(count_display_timer.isActive()==true) count_display_timer.stop();

//    //uart debug config
//    serialPort->setBaudRate(QSerialPort::Baud115200/*, QSerialPort::AllDirections*/);
//    serialPort->setDataBits(QSerialPort::Data8);
//    serialPort->setParity(QSerialPort::NoParity);//
//    serialPort->setStopBits(QSerialPort::OneStop);

//    serialPort->setFlowControl(QSerialPort::NoFlowControl);
//    connect(serialPort,SIGNAL(readyRead()),this, SLOT(receive_data()));

//    cout << "connect to recive data " << endl;
//    //remove_last_line_textedit(ui->textEdit);
//    ui->textEdit->append("UART DEBUG CONNECT OK");
//    //stop qua trinh check uart debug
//    timer_uart_setting_wait.stop();
//    DEBUG_OPEN_OK = true;

////               QSerialPort *serial = new QSerialPort();
//               serial->close();
//               serial->setPortName("COM9");
//               if(!serial->setBaudRate(QSerialPort::Baud115200))
//                   qDebug() << serial->errorString();
//               if(!serial->setDataBits(QSerialPort::Data8))
//                   qDebug() << serial->errorString();
//               if(!serial->setParity(QSerialPort::NoParity))
//                   qDebug() << serial->errorString();
//               if(!serial->setFlowControl(QSerialPort::NoFlowControl))
//                   qDebug() << serial->errorString();
//               if(!serial->setStopBits(QSerialPort::OneStop))
//                   qDebug() << serial->errorString();

//////               if(!serial.open(QIODevice::ReadWrite))
//////                   qDebug() << serial.errorString();
////               cout << "setting is done " << endl;


//               if (serial->open(QIODevice::ReadWrite))
//               {
//                       cout << "Connectedd" << endl;
//               }
//               else
//               {
//                      cout <<"Open error"<< endl;
//               }
//               connect(serial,SIGNAL(readyRead()),this, SLOT(receive_data()));
//               QString cmd = "/hnbin/cmd/bin/http_stream";
//               serial->write(cmd.toUtf8());
//               serial->write("\n");


////               string DBG_input;
////               QByteArray databyte = serialPort->readAll();
////               QString data = databyte;
////               //    cout << "pre time = " << input << endl;
////               DBG_input += data.toUtf8().constData();
////               cout << "DBG_input  = " << DBG_input << endl;

////               QObject::connect(&serial, &QSerialPort::readyRead, [&]
////               {
////                   cout << "readyRead " << endl;
////                   //this is called when readyRead() is emitted
////                   qDebug() << "New data available: " << serial->bytesAvailable();
////                   QByteArray datas = serial->readAll();
////                   qDebug() << datas;
////               });
////               QObject::connect(&serial,
////                                static_cast<void(QSerialPort::*)(QSerialPort::SerialPortError)>
////                                (&QSerialPort::error),
////                                [&](QSerialPort::SerialPortError error)
////               {
////                   //this is called when a serial communication error occurs
////                   qDebug() << "An error occured: " << error;
////               });




       just_check_GMSI = true;
       ui->GMSM_STATE->setText("NG");
       WIFI_check_run();
       just_check_GMSI = false;
       bool ALL_STATE_CHECK = true;
       string ALL_state_test;
       ALL_state_test = ui->WIFI_STATE->text().toUtf8().constData();
       if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(WIFI_check==true)) ALL_STATE_CHECK = false;
       ALL_state_test = ui->GMSM_STATE->text().toUtf8().constData();
       if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(GMSM_check==true)) ALL_STATE_CHECK = false;

       ALL_state_test = ui->LED_STATE->text().toUtf8().constData();
       if((ALL_state_test.compare("OK")!=0)&&(ALL_STATE_CHECK==true)&&(check_LED==true)) ALL_STATE_CHECK = false;
       if(ALL_STATE_CHECK==true)
       {
           ui->START_STATE->setStyleSheet(button_blue);
           ui->START_STATE->setText("OK");
       }
       else
       {
           ui->START_STATE->setStyleSheet(button_red);ui->START_STATE->setText("NG");
       }





}

void MainWindow::on_horizontalSlider_actionTriggered(int action)
{
//    QString ON_LED = "/hnbin/cmd/bin/shell-ele_PWM -e 1";
//    uart_send(ON_LED);

//    QString Change_LED = "/hnbin/cmd/bin/shell-ele_PWM -e ";
//    Change_LED = Change_LED ;
//    uart_send();

}

bool LED_STA = false;
int value_before = 0;
int count_led = 0;
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (LED_STA == false )
    {
         QString ON_LED = "/hnbin/cmd/bin/shell-ele_PWM -e 1";
         uart_send(ON_LED);
         LED_STA = true;
    }

    QString Change_LED = "/hnbin/cmd/bin/shell-ele_PWM -p ";
    Change_LED = Change_LED +  QString::number(value) ;
    cout << "Change_LED  = " << Change_LED.toStdString() << endl;
    uart_send(Change_LED);

    ui->duty_pwm->setText(QString::number(value) + "%");

    cout << "value " <<value << endl;

    if ( value_before == value )
    {
        QThread::sleep(1);
        count_led = count_led +  1;
        if (count_led == 2)
        {
            QString OFF_LED = "/hnbin/cmd/bin/shell-ele_PWM -e 0";
            uart_send(OFF_LED);
            LED_STA = false;


        }

        // stop change
    }

    value_before = value;


}

bool power_led = true;
void MainWindow::on_power_led_clicked()
{
    QString ON_LED  = "echo out > /sys/class/gpio/GPIO26/direction";
    QString OFF_LED = "echo in > /sys/class/gpio/GPIO26/direction";

    if (power_led == false )
    {
        uart_send(ON_LED);
        power_led = true;
        ui->power_led_sta->setText("ON");
    }
    else
    {
        uart_send(OFF_LED);
        power_led = false;
        ui->power_led_sta->setText("OFF");

    }
}

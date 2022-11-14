#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QTextEdit>

#include <QKeyEvent>

#include <string>
#include <iostream>
#include <sstream>


#include <QThread>
#include <QTimer>
#include <QObject>

#include <QCloseEvent>
#include <QMessageBox>

#include <QElapsedTimer>

#include <QFileDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <QDesktopServices>
#include <QFileInfo>

#include "file_process.h"



//web
//#include <QtWebEngineWidgets/QWebEngineView>
//#include <QtWebEngineWidgets/QWebEnginePage>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLineEdit>
#include <QToolBar>

#include <QNetworkProxy>

#include <QSpinBox>


#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

using namespace std;
using namespace cv;




namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

    //UART
private:
    Ui::MainWindow *ui;
    bool DEBUG_SHOW = false;
    bool DEBUG_OPEN_OK = false;//dat tai START, neu khong open debug OK thi khong chay duoc gi het

    QSerialPort *serialPort = new QSerialPort();
    QTimer timer_uart_setting_wait;


    QTimer count_display_timer;

signals:
    void receive_uart_data_ok();
    void uart_debug_setting_ok();

private: //function
    //void keyPressEvent( QKeyEvent * event );
    void uart_send_shell(QString cmd, int mode);
    void uart_send(QString cmd);


    bool send_message_yes_no(QString title, QString mess);

private slots:
    void receive_data();
    void uart_debug_setting();
    void qtext_edit_count_display_timer();

signals:
    void count_display_timeout();



    //Setting
 private:
     bool SETTING_CONF_OK = false;
     bool SETTING_ON = false;

     void get_setting_para();
     void remove_last_line_textedit(QTextEdit *text_edit);

     void SETTING_load_from_line_edit();
 private slots:

    void on_actionSetting_triggered();
    void on_SETTING_load_clicked();
    void on_SETTING_save_clicked();
    void on_SETTING_exit_clicked();
    void on_SETTING_apply_clicked();



    //FILE
private:
    file_process file_conf;
//    void load_file();



    //SD_CARD
private:
    bool SD_CARD_CHECK_MOUNT = false;
    bool SD_CARD_CHECK_LS = false;

    bool Mount_OK = false;
    bool Insert_OK = false;

    bool SD_CARD_OK = false;

    int check_insert();
    int check_mount();
    int check_write();

    void SD_CARD_check_run();
signals:
    void SD_card_check();

private slots:
    void on_SD_CARD_clicked();






    //GPIO
private:
    bool DEBUG_get_data_on = false; // = true -> cho phep get data ben trong uart_receive
    unsigned int DEBUG_num_data_get; // so luong data se get, data nay tra ve tu ham test_shell

    bool GPIO_OK = false;

    vector<string> data_get;

    void GPIO_setdir(int GPIO, int dir);
    void GPIO_setvalue(int GPIO, int value);
    void GPIO_getvalue(int GPIO);
    void GPIO_setting_dir(unsigned int GPIO_set);
    void GPIO_setting_send(unsigned int GPIO_set, int value);

    void GPIO_check_run();

signals:

private slots:
    void on_GPIO_clicked();

    void gpio_blink_();
    void on_GPIO_BLINK_test_clicked();

private:
    bool GPIO_BLINK_ON = false;
    int GPIO_port_blink_test;

    QTimer timer_blink;

    void GPIO_check_run_messenger(int GPIO_set);
signals:
    void GPIO_blink_test_sig();

    //SPI
private:

    bool SPI_OK = false;

    void spi_setting();
    void spi_send(QString cmd_send);

    void SPI_check_run();


private slots:
    void on_SPI_clicked();


    //UART
private:
    bool UART_OK = false;

    void uart_setting();
    void uart_send_loop(QString cmd_send);

    void UART_check_run();

private slots:
   void on_UART_clicked();



   //I2C
private:
   void i2c_setting();
   void i2c_send(QString cmd_send);
   void i2c_read();

   void I2C_check_run();

private slots:
   void on_I2C_clicked();



    //UVC
private:
//   bool Camera_start = false;
   bool Camera_UVC_start = false;
   bool Camera_WIFI_start = false;


   bool UVC_had_run = false; // check xem camera tung chay hay chua de release

   bool UVC_OK = false;
   bool UVC_check_STOP = false;

   cv::VideoCapture* cap;
   int camera_number_id;

   QTimer Timer_get_camera;
   QElapsedTimer timer_check;
   int dem_time;

   bool UVC_confirm = false;

//   QMessageBox *UVC_stream_message_check_result, *GMSM_message_check_result;

   static   QPixmap Mat2QPixmap(cv::Mat const& _frame);
   void Display(cv::Mat image);
   bool GetCamera(cv::Mat &image);

   void uvc_stream_start();
   void uvc_stream_stop();
   void Reset_Camera();

   void UVC_message_check();

   void UVC_check_run();

private slots:
   void Process();
   void UVC_wait_for_camera_number_appear();

signals:
    void UVC_open_OK();
    void UVC_cmd_EXIT_finished();

    void UVC_camera_number_appear();

private slots:
   void on_UVC_clicked();

   void on_UVC_confirm_clicked(bool checked);


   //GOMISIMI
private:
   bool GMSM_check = false;
   bool GMSM_run = true;  // fasle
   bool GMSM_OK = false;

   int size_wid = 1920, size_hei = 1088;
   int set_wid = size_wid, set_hig = size_hei;

   double wk1 = 0,
   wk2_num1 = 0, wk2_num2 = 0, wk2_num3 = 0,
   wk3_num1 = 0, wk3_num2 = 0, wk3_num3 = 0;

   cv::Mat mat_image;
   cv::Mat pre_noise_image;

   int h_thre[3][2];
   cv::Mat store_blance, temp_store[3];
   cv::Mat store_h[3];

   bool show_full = false;//show 50 thoi
   bool test_mode_on = false;//no test



   //sap xep
   struct contour_sorter // 'less' for contours
   {
       bool operator ()( const vector<cv::Point>& a, const vector<cv::Point> & b )
       {
           double i = fabs( contourArea(cv::Mat(a)) );
               double j = fabs( contourArea(cv::Mat(b)) );
               return ( i > j );
       }
   };


   void process_screen_3pic(cv::Mat &mat_image, cv::Mat &mat_output, int mode, int &dem);

   void Create_base_waku_1(double wk1);
   void Create_base_waku_2(double wk2_num1, double wk2_num2, double wk2_num3);
   void Create_base_waku_3(double wk3_num1, double wk3_num2, double wk3_num3);
   void draw_wk(Mat &mat_image);
   //ham nay neu nhu moi lan thay doi do phan giai can phai goi de tao lai. Con neu chi co 1 do phan giai thi chi can chay 1 lan ma thoi
   void Setting_gomisimi_parameter();

   void GMSM_message_check();

private slots:
   void on_GMSM_check_clicked(bool checked);





    //WIFI
private:
   bool WIFI_LINK_FOUND_ON = false;
   bool WIFI_OK = false;
   bool LED_OK = false;

   bool WIFI_STATION_check_EN = false;
   bool WIFI_STATION_check = false;


   string wifi_link;


   void wifi_stream_start();
   void wifi_stream_stop();
   void WIFI_check_run();
   void LED_check_run();
   void WIFI_message_check();
   void LED_message_check();
private slots:
   void on_WIFI_clicked();

    //WEB
private:
  // QWebEngineView *webview;
   QMainWindow * webmainWindow;
   QToolBar *toolBar;
   QLineEdit *locationEdit;
   int progress;
//   QAction *rotateAction;
//   QString jQuery;

private slots:
   void adjustLocation();
   void changeLocation();
//   void adjustTitle();
//   void setProgress(int p);
//   void finishLoading(bool);

   void on_STATION_mode_auto_clicked(bool checked);









   //Control
private:

   bool LOCK_BUTTON = true;
   bool START_RUNNING = false;
   bool SD_check   = false, GPIO_check = false, SPI_check = false,
        UART_check = false, I2C_check  = false, UVC_check = false,
        WIFI_check = false, LED_check  = false, check_LED = false;

//   bool FOUND_NEW_BOARD_SIGNAL = false;
   enum FOUND_NEW_BOARD_enum{
       FOUND_NEW_BOARD_SIGNAL = 0,
       FOUND_NEW_BOARD_SIGNAL_FINISHED = 1,
       FOUND_NEW_BOARD_FINISHED = 2,
   };

//   bool FOUND_NEW_BOARD = false;
   int NEW_BOARD_STATE = FOUND_NEW_BOARD_FINISHED;

   QMessageBox Send_message_check_result;
   void Send_message_check(QString messenger, int pos_y, bool &Data_check);


private slots:
   void on_SD_check_clicked(bool checked);
   void on_GPIO_check_clicked(bool checked);
   void on_SPI_check_clicked(bool checked);
   void on_UART_check_clicked(bool checked);
   void on_I2C_check_clicked(bool checked);
   void on_UVC_check_clicked(bool checked);
   void on_WIFI_check_clicked(bool checked);

 ///  void on_LED_check_clicked(bool checked);

   void on_GPIO_mode_clicked(bool checked);

   void on_START_clicked();


   void on_actionLock_button_triggered();
   void on_actionUnlock_button_triggered();
   void on_actionReset_triggered();

   void on_Check__LED_clicked( bool checked );

   void on_SSID_PW_CHANGE_clicked();
   void on_LED_clicked();
   void on_GMSM_clicked();
   void on_horizontalSlider_actionTriggered(int action);
   void on_horizontalSlider_valueChanged(int value);
   void on_power_led_clicked();



// connect to WiFi

public:
    int foundCount;
    QNetworkConfiguration netcfg;
    QStringList WiFisList;
    QList<QNetworkConfiguration> netcfgList;

public slots:
    void findActiveWirelesses();


private:
    QTimer *findTimer;
    QStandardItemModel* listModel;
    QNetworkSession *session;



};




#endif // MAINWINDOW_H

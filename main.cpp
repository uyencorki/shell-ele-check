#include "mainwindow.h"

#include <QApplication>


#if 1
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#else
#include <iostream>

cv::Mat image;
int main(int, char**) {
    cv::VideoCapture vcap;



    const std::string videoStreamAddress = "http://192.168.0.42:8080/?action=stream";

    const string h264_video = "C:/Users/Hino_Thanh/Desktop/h264/h264_30fps.h264";

    if(!vcap.open(videoStreamAddress)) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    QString save_link;
    for(;;) {
        if(!vcap.read(image)) {
            std::cout << "No frame" << std::endl;
            cv::waitKey();
        }
        cv::imshow("Output Window", image);

        if(cv::waitKey(1) >= 0) break;
    }

}

#endif

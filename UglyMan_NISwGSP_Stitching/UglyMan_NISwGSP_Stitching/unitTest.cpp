#include <iostream>
#include <opencv2/opencv.hpp>

#include "./Util/Networks/tcp/include/tcp_server.h"
#include "./Util/Networks/udp/udpNode.h"
#include "./Util/VideoCapture.h"

///////////////////////////////////////////////////// Tcp test

TcpServer server;
void printMenu() {
    std::cout << "\n\nselect one of the following options: \n" <<
              "1. send all clients a message\n" <<
              "2. print list of accepted clients\n" <<
              "3. send message to a specific client\n" <<
              "4. close server and exit\n";
}

int getMenuSelection() {
    int selection = 0;
    std::cin >> selection;
    if (!std::cin) {
        throw std::runtime_error("invalid menu input. expected a number, but got something else");
    }
    std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
    return selection;
}

/**
 * handle menu selection and return true in case program should terminate
 * after handling selection
 */
bool handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 4;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
                  ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
    }
    switch (selection) {
        case 1: { // send all clients a message
            std::string msg;
            std::cout << "type message to send to all connected clients:\n";
            getline(std::cin, msg);
            pipe_ret_t sendingResult = server.sendToAllClients(msg.c_str(), msg.size());
            if (sendingResult.isSuccessful()) {
                std::cout << "sent message to all clients successfully\n";
            } else {
                std::cout << "failed to sent message: " << sendingResult.message() << "\n";
            }
            break;
        }
        case 2: { // print list of accepted clients
            server.printClients();
            break;
        }
        case 3: { // send message to a specific client
            std::cout << "enter client IP:\n";
            std::string clientIP;
            std::cin >> clientIP;
            std::cout << "enter message to send:\n";
            std::string message;
            std::cin >> message;
            pipe_ret_t result = server.sendToClient(clientIP, message.c_str(), message.size());
            if (!result.isSuccessful()) {
                std::cout << "sending failed: " << result.message() << "\n";
            } else {
                std::cout << "sending succeeded\n";
            }
            break;
        };
        case 4: { // close server
            pipe_ret_t sendingResult = server.close();
            if (sendingResult.isSuccessful()) {
                std::cout << "closed server successfully\n";
            } else {
                std::cout << "failed to close server: " << sendingResult.message() << "\n";
            }
            return true;
        }
        default: {
            std::cout << "invalid selection: " << selection <<
                      ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        }
    }
    return false;
}

bool TcpTest()
{
    
    pipe_ret_t startRet = server.start(65123);
    if (startRet.isSuccessful()) {
        std::cout << "Server setup succeeded\n";
    } else {
        std::cout << "Server setup failed: " << startRet.message() << "\n";
        return EXIT_FAILURE;
    }

    try {
        std::cout << "waiting for incoming client...\n";
        std::string clientIP = server.acceptClient(0);
        std::cout << "accepted new client with IP: " << clientIP << "\n" <<
                  "== updated list of accepted clients ==" << "\n";
        server.printClients();
    } catch (const std::runtime_error &error) {
        std::cout << "Accepting client failed: " << error.what() << "\n";
    }

    bool shouldTerminate = false;
    while(!shouldTerminate) {
        printMenu();
        int selection = getMenuSelection();
        shouldTerminate = handleMenuSelection(selection);
    }



    return true;
}
///////////////////////////////////////////////////// Tcp End

bool UdpTest()
{

    return true;
}

bool GetVideoTest()
{
    std::cout<< "getImageViaSDK in"<<std::endl;
    //std::string strDeviceName = "UB3300 USB"; 
    std::string strDeviceName = "MZ0380 PCI";
    int nInputType = 2; 
    int nImgW = 1920; 
    int nImgH = 1080;
    //int nImgW = 1280; 
    //int nImgH = 1024;
    int nRGBDataLen = nImgW * nImgH * 3;
    unsigned char* pRGBData = new unsigned char[nRGBDataLen];

    CVideoCapture::getInstance().CreateDevice(strDeviceName, nInputType,  nImgW, nImgH);
    int nFrameCount = 100 * 60 * 10;
    cv::Mat imgRGB = cv::Mat(nImgH, nImgW, CV_8UC3, pRGBData);
    long long lastFrameTime = getCurrentTimeMs();

    while(nFrameCount--)
    {
        long long t1 = getCurrentTimeMs();
        int realDataLen = 0;
        CVideoCapture::getInstance().CopyRGBData(pRGBData, nRGBDataLen, &realDataLen);
        if(nRGBDataLen != realDataLen)
        {
            std::cout<<"nRGBDataLen != realDataLen"<<std::endl;
            break;
        }
        long long t2 = getCurrentTimeMs();

        cv::Mat img;
        cv::cvtColor(imgRGB, img, cv::COLOR_RGB2BGR);
        //std::cout<<"w:"<<img.cols<<" h:"<<img.rows<<std::endl;
        cv::Mat resized;
        //cv::resize(img, resized, cv::Size(640, 480));
        //cv::resize(img, resized, cv::Size(), 0.5, 0.5);
        //cv::imshow("image", resized);
        cv::imshow("image", img);
        int key = cv::waitKey(1);
        long long t3 = getCurrentTimeMs();
        if(key == 'q')
        {
            break;
        }
        if(nFrameCount % 100 ==0 )
        {
            long long curTime = getCurrentTimeMs();
            std::cout<< curTime - lastFrameTime<< "ms per 100 frame"<<std::endl;
            std::cout<< "copy data "<< t2 - t1<<"ms, resize:"<<t3 - t2<< "ms"<<std::endl;
            lastFrameTime = curTime;
        }

        
    }

    delete[] pRGBData;
    std::cout<< "getImageViaSDK out"<<std::endl;

    return true;
}













int main(int argc, char* argv[])
{
    //TcpTest();
    GetVideoTest();


    return 0;
}
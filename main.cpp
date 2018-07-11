#include <iostream>
#include "opencv2/opencv.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <thread>

#define FIFO_NAME "/tmp/mkfifo_opencv"

void readAll(int fdPipe, char * buf, int len)
{
    int total = 0;
    while (total < len)
    {
        int bytes = read(fdPipe, buf+total, len);
        if (bytes < 0)
        {
            perror("read");
            return;
        }
        total += bytes;
    }
}

void sendAll(int fdPipe, char * buf, int len) {
    int total = 0;
    while (total < len) {
        int sent = write(fdPipe, buf, len - total);
        if (sent < 0) {
            perror("write");
            return;
        }
        total += sent;
    }
}

int main() {
    /*if (mkfifo(FIFO_NAME, (S_IRWXO | S_IRWXG | S_IRWXU)))
    {
        perror("mkfifo");
        return 1;
    }*/

    /*int fdPipe;
    if ((fdPipe = open(FIFO_NAME, O_RDWR)) <= 0)
    //if ((fdPipe = open(FIFO_NAME, O_RDONLY)) <= 0)
    {
        perror("open");
        return 1;
    }*/
    std::fstream pipe;//(FIFO_NAME, std::ios::in | std::ios::out | std::ios::binary);
    while(!pipe.is_open())
    {
        pipe.open(FIFO_NAME, std::ios::in | std::ios::out | std::ios::binary);
    }


    //send sync
    char * forSync = new char[2];

    do {
        strcpy(forSync, "C");
        pipe << forSync;
        pipe.flush();
        std::cout << "sent" << std::endl;
        sleep(1);
    } while (pipe.readsome(forSync, 1) < 1 || forSync[0] == 'C');


//    return 0;

//    sendAll(fdPipe, forSync, strlen(forSync));

        while(true)
        {
            int rows = 0;
            int cols = 0;
            int type = 0;
            int dataSize = 0;
            char * buf;// = (char*)&rows;
//        readAll(fdPipe, buf, sizeof(int));
            pipe.read((char*)&rows, sizeof(int));
//        buf = (char*)&cols;
//        readAll(fdPipe, buf, sizeof(int));
            pipe.read((char*)&cols, sizeof(int));
//        buf = (char*)&type;
//        readAll(fdPipe, buf, sizeof(int));
            pipe.read((char*)&type, sizeof(int));
//        buf = (char*)&dataSize;
//        readAll(fdPipe, buf, sizeof(int));
            pipe.read((char*)&dataSize, sizeof(int));
            buf = new char[dataSize];
//        readAll(fdPipe, buf, dataSize);
            pipe.read(buf, dataSize);

            cv::Mat tmpMat(rows, cols, type, buf);
            if (tmpMat.empty())
            {
                delete[] buf;
                break;
            }
            cv::imshow("Video", tmpMat);
            char c=(char)cv::waitKey(25);
            if(c==27)
                break;
            delete [] buf;
        }
    //read answer
    remove(FIFO_NAME);
    return 0;//*/
}
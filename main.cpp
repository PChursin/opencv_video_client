#include <iostream>
#include "opencv2/opencv.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FIFO_NAME "/tmp/opencv_video"

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

    int fdPipe;
    if ((fdPipe = open(FIFO_NAME, O_RDWR)) <= 0)
    //if ((fdPipe = open(FIFO_NAME, O_RDONLY)) <= 0)
    {
        perror("open");
        return 1;
    }

    //send sync
    sendAll(fdPipe, "S", 1);

    while(true)
    {
        int rows = 0;
        int cols = 0;
        int type = 0;
        int dataSize = 0;
        char * buf = (char*)&rows;
        readAll(fdPipe, buf, sizeof(int));
        buf = (char*)&cols;
        readAll(fdPipe, buf, sizeof(int));
        buf = (char*)&type;
        readAll(fdPipe, buf, sizeof(int));
        buf = (char*)&dataSize;
        readAll(fdPipe, buf, sizeof(int));
        buf = new char[dataSize];
        readAll(fdPipe, buf, dataSize);

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

    return 0;
}
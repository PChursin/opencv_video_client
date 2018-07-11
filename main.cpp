#include <iostream>
#include "opencv2/opencv.hpp"
#include <sys/stat.h>
#include <thread>

#define FIFO_NAME "/tmp/mkfifo_opencv"

int main() {
    if (mkfifo(FIFO_NAME, (S_IRWXO | S_IRWXG | S_IRWXU)) && errno != EEXIST)
    {
        perror("mkfifo");
        return 1;
    }

    std::ifstream pipe(FIFO_NAME, std::ios::in | std::ios::out | std::ios::binary);
    if (!pipe.is_open()) {
        std::cout << "Could not open pipe at " << FIFO_NAME << std::endl;
        return 1;
    }
    while (true) {
        int rows = 0;
        int cols = 0;
        int type = 0;
        int dataSize = 0;
        pipe.read((char *) &rows, sizeof(int));
        pipe.read((char *) &cols, sizeof(int));
        pipe.read((char *) &type, sizeof(int));
        pipe.read((char *) &dataSize, sizeof(int));
        char *buf = new char[dataSize];
        pipe.read(buf, dataSize);

        cv::Mat tmpMat(rows, cols, type, buf);
        if (tmpMat.empty()) {
            delete[] buf;
            break;
        }
        cv::imshow("Video", tmpMat);
        //important to show the frame
        char c = (char) cv::waitKey(25);
        if (c == 27) {
            delete[] buf;
            break;
        }
        delete[] buf;
    }
    remove(FIFO_NAME);
    return 0;
}
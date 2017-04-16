/* C++ headers */
#include <iostream>
#include <string>
/* C headers */
#include <stdio.h>
/* library headers */
#include <opencv2/opencv.hpp>


/* Global variables */
std::string winname = "Window";

int main(int argc, char** argv) {

  cv::namedWindow(winname);
  cv::VideoCapture cap;
  cv::Mat frame;

  if(!cap.open(0)) {
    printf("[!] Error: couldn't load camera");
    return 0;
  }

  for (;;) {
    cap >> frame;
    if(frame.empty()) break;
    imshow(winname, frame);
    if(cv::waitKey(10) == 27) break;
    if(cv::getWindowProperty(winname, 0) < 0) break;
  }

  
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();

  return 0;
}

/* C++ headers */
#include <iostream>
#include <string>
#include <vector>
/* C headers */
#include <stdio.h>
/* library headers */
#include <opencv2/opencv.hpp>

/* Global variables */
std::string winname = "Window";

int main(int argc, char** argv) {

  cv::namedWindow(winname); // main window
  cv::VideoCapture cap; // webcam capture device
  cv::Mat frame; // image frame
  std::vector<cv::Point2f> corners;
  cv::Size patternsize(8,6);

  if(!cap.open(0)) {
    // no webcam found
    printf("[!] Error: couldn't load camera");
    return 0;
  }

  for (;;) {
    cap >> frame; // read from webcam into frame
    if(frame.empty()) break; // no data found in frame
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY); // convert frame to greyscale
    imshow(winname, frame); // show frame in window
    if(cv::waitKey(10) == 27) break; // wait for escape key
    if(cv::getWindowProperty(winname, 0) < 0) break; // break if window is closed
    bool patternFound = cv::findChessboardCorners(frame, patternsize, corners);
    if(patternFound) cv::cornerSubPix(frame, corners, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    cv::drawChessboardCorners(frame, patternsize, cv::Mat(corners), patternFound);
  }

  // release memory
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();

  return 0;
}

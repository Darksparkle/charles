/* C++ headers */
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
/* C headers */
#include <stdio.h>
/* library headers */
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

/* Global variables */
std::string winname = "Window";

std::ofstream file;

int main(int argc, char** argv) {

  cv::namedWindow(winname); // main window
  cv::VideoCapture cap; // webcam capture device
  cv::Mat frame; // image frame
  cv::Mat gray;
  std::vector<cv::Point2f> corners;
  cv::Size patternsize;
  if(argc > 1) {
    patternsize = {atoi(argv[1]), atoi(argv[2])};
  } else {
    patternsize = {6,6};
  }
  bool patternFound;

  cv::Mat intrinsics, distortion;
  std::vector<cv::Point3f> boardPoints;

  file.open("points");

  if(!cap.open(0)) {
    // no webcam found
    printf("[!] Error: couldn't load camera");
    return 0;
  }

  for (;;) {
    cap >> frame; // read from webcam into frame
    if(frame.empty()) break; // no data found in frame
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // convert frame to greyscale
    if(cv::waitKey(10) == 27) break; // wait for escape key
    if(cv::getWindowProperty(winname, 0) < 0) break; // break if window is closed
    patternFound = cv::findChessboardCorners(gray, patternsize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    if(patternFound) cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    cv::drawChessboardCorners(frame, patternsize, cv::Mat(corners), patternFound);
    if(patternFound) file << corners << '\n' << '\n';
    imshow(winname, frame); // show frame in window
  }

  // release memory
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();

  file.close();

  return 0;
}

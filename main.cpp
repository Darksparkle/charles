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
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/highgui.hpp>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* Global variables */
std::string winname = "Window";
std::ofstream file;

const int win_width = 800;
const int win_height = 640;

int main(int argc, char** argv) {

  cv::namedWindow(winname, cv::WINDOW_OPENGL);
  cv::resizeWindow(winname, win_width, win_height);

  cv::VideoCapture cap; // webcam capture device

  cv::Mat frame; // image frame
  cv::Mat gray; // gray frame
  cv::Mat thresh;
  cv::Mat blur;

  std::vector<cv::Point2f> corners;
  cv::Size patternsize;
  if(argc > 1) {
    patternsize = {atoi(argv[1]), atoi(argv[2])};
  } else {
    patternsize = {6,6};
  }

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
    if(frame.empty()) break; // break on no data found in frame
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // convert frame to greyscale
    cv::blur(gray, blur, cv::Size(5,5));
    cv::threshold(blur, thresh, 128.0, 255.0, cv::THRESH_OTSU);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(thresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    cv::Scalar color(0,255,0);
    std::vector<std::vector<cv::Point> > squares;
    for(auto contour : contours) {
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, cv::arcLength(cv::Mat(contour), true)*0.02, true);
      if(approx.size() == 4 && fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
        squares.push_back(approx);
      }
    }
    cv::drawContours(frame, squares, -1, color);
    imshow(winname, frame);
    if(cv::waitKey(10) == 27) break; // break on escape key pressed
    if(cv::getWindowProperty(winname, 0) < 0) break; // break if window is closed
  }

  // release memory
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();
  file.close();

  return 0;
}

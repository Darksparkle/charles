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

// http://www.jera.com/jbrewer/2014/01/computer-vision-challenge-1-augmented-reality.html

/* Global variables */
std::string winname = "Window";

const int win_width = 800;
const int win_height = 640;

void drawQuad(cv::Mat image, cv::Mat points, cv::Scalar color) {
  std::cout << points.at<cv::Point2f>(0,0) << " " << points.at<cv::Point2f>(0,2) << " " << points.at<cv::Point2f>(0,3) << '\n';
  cv::line(image, points.at<cv::Point2f>(0,0), points.at<cv::Point2f>(0,1), color);
  cv::line(image, points.at<cv::Point2f>(0,1), points.at<cv::Point2f>(0,2), color);
  cv::line(image, points.at<cv::Point2f>(0,2), points.at<cv::Point2f>(0,3), color);
  cv::line(image, points.at<cv::Point2f>(0,3), points.at<cv::Point2f>(0,0), color);
}

int main(int argc, char** argv) {

  cv::namedWindow(winname, cv::WINDOW_OPENGL);
  cv::resizeWindow(winname, win_width, win_height);

  cv::VideoCapture cap; // webcam capture device

  cv::Mat frame; // image frame
  cv::Mat gray; // gray frame
  cv::Mat thresh;
  cv::Mat blur;

  cv::Scalar   red(255, 0, 0);
  cv::Scalar green(0, 255, 0);
  cv::Scalar  blue(0, 0, 255);

  cv::FileStorage fs("data.xml", cv::FileStorage::READ);
  cv::Mat intrinsics, distortion;
  fs["Camera_Matrix"] >> intrinsics;
  fs["Distortion_Coefficients"] >> distortion;

  if (intrinsics.rows != 3 || intrinsics.cols != 3 || distortion.rows != 5 || distortion.cols != 1) {
    std::cout << "Run calibration (in ../calibrate/) first!" << '\n';
    return 1;
  }

  if (!cap.open(0)) {
    // no webcam found
    printf("[!] Error: couldn't load camera");
    return 0;
  }

  for (;;) {
    cap >> frame; // read from webcam into frame
    if (frame.empty()) break; // break on no data found in frame

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // convert frame to greyscale
    cv::blur(gray, blur, cv::Size(5,5));
    cv::threshold(blur, thresh, 128.0, 255.0, cv::THRESH_OTSU);

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(thresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    std::vector<cv::Mat> squares;
    for(auto contour : contours) {
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, cv::arcLength(cv::Mat(contour), true)*0.02, true);
      if(approx.size() == 4 && fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
        cv::Mat squareMat;
        cv::Mat(approx).convertTo(squareMat, CV_32FC3);
        squares.push_back(squareMat);
      }
    }

    if (squares.size() > 0) {
      std::vector<cv::Point3f> objectPoints = {cv::Point3f(-1, -1, 0), cv::Point3f(-1, 1, 0), cv::Point3f(1, 1, 0), cv::Point3f(1, -1, 0)};
      cv::Mat objectPointsMat(objectPoints);

      std::cout << "objectPointsMat: " << objectPointsMat.rows << ", " << objectPointsMat.cols << '\n';
      std::cout << "squares[0]: " << squares[0] << '\n';

      cv::Mat rvec, tvec;
      cv::solvePnP(objectPointsMat, squares[0], intrinsics, distortion, rvec, tvec);

      std::cout << "rvec: " << rvec << '\n' << "tvec: " << tvec << '\n';

      drawQuad(frame, squares[0], green);

      std::vector<cv::Point3f> line3d = {{0, 0, 0}, {0, 0, -4}};
      std::vector<cv::Point2f> line2d;

      cv::projectPoints(line3d, rvec, tvec, intrinsics, distortion, line2d);
      std::cout << "line2d: " << line2d << '\n';
      cv::line(frame, line2d[0], line2d[1], red);
    }

    // cv::drawContours(frame, squares, -1, color);

    imshow(winname, frame);

    if (cv::waitKey(10) == 27)
      break; // break on escape key pressed

    if (cv::getWindowProperty(winname, 0) < 0)
      break; // break if window is closed
  }

  // release memory
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();

  return 0;
}

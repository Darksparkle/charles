/* C++ headers */
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
/* C headers */
#include <stdio.h>
#include <math.h>
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

#define PI (4*atan(1))

// http://www.jera.com/jbrewer/2014/01/computer-vision-challenge-1-augmented-reality.html

/* Global variables */
std::string winname = "Window";

float r = 1;

const int win_width = 800;
const int win_height = 640;

void drawQuad(cv::Mat image, cv::Mat points, cv::Scalar color) {
  // std::cout << points.at<cv::Point2f>(0,0) << " " << points.at<cv::Point2f>(0,2) << " " << points.at<cv::Point2f>(0,3) << '\n';
  cv::line(image, points.at<cv::Point2f>(0,0), points.at<cv::Point2f>(0,1), color);
  cv::line(image, points.at<cv::Point2f>(0,1), points.at<cv::Point2f>(0,2), color);
  cv::line(image, points.at<cv::Point2f>(0,2), points.at<cv::Point2f>(0,3), color);
  cv::line(image, points.at<cv::Point2f>(0,3), points.at<cv::Point2f>(0,0), color);
}

cv::Point3f rotate(cv::Point3f base, std::vector<cv::Point3f> rotation) {
  cv::Point3f result = {
    (base.x*rotation[0].x + base.y*rotation[0].y + base.z*rotation[0].z),
    (base.x*rotation[1].x + base.y*rotation[1].y + base.z*rotation[1].z),
    (base.x*rotation[2].x + base.y*rotation[2].y + base.z*rotation[2].z)
  };
  return result;
}

std::vector<cv::Point3f> tesseract[] = {
  // outer cube
  {{-1, -1, 1}, {1, -1, 1}},
  {{1, -1, 1}, {1, 1, 1}},
  {{1, 1, 1}, {-1, 1, 1}},
  {{-1, 1, 1}, {-1, -1, 1}},
  {{-1, -1, 1}, {-1, -1, -1}},
  {{-1, 1, 1}, {-1, 1, -1}},
  {{1, 1, 1}, {1, 1, -1}},
  {{1, -1, 1}, {1, -1, -1}},
  {{-1, -1, -1}, {1, -1, -1}},
  {{1, -1, -1}, {1, 1, -1}},
  {{1, 1, -1}, {-1, 1, -1}},
  {{-1, 1, -1}, {-1, -1, -1}},
  // inner cube
  {{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}},
  {{0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}},
  {{0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}},
  {{-0.5, 0.5, -0.5}, {-0.5, -0.5, -0.5}},
  {{-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}},
  {{-0.5, 0.5, -0.5}, {-0.5, 0.5, 0.5}},
  {{0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}},
  {{0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}},
  {{-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}},
  {{0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}},
  {{0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}},
  {{-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}},
  // connecting lines
  {{-1, -1, 1}, {-0.5, -0.5, 0.5}},
  {{1, -1, 1}, {0.5, -0.5, 0.5}},
  {{1, 1, 1}, {0.5, 0.5, 0.5}},
  {{-1, 1, 1}, {-0.5, 0.5, 0.5}},
  {{-1, -1, -1}, {-0.5, -0.5, -0.5}},
  {{1, -1, -1}, {0.5, -0.5, -0.5}},
  {{1, 1, -1}, {0.5, 0.5, -0.5}},
  {{-1, 1, -1}, {-0.5, 0.5, -0.5}}
};
std::vector<cv::Point2f> line2d[32];

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

      // std::cout << "objectPointsMat: " << objectPointsMat.rows << ", " << objectPointsMat.cols << '\n';
      // std::cout << "squares[0]: " << squares[0] << '\n';

      cv::Mat rvec, tvec;
      cv::solvePnP(objectPointsMat, squares[0], intrinsics, distortion, rvec, tvec);

      // std::cout << "rvec: " << rvec << '\n' << "tvec: " << tvec << '\n';
      std::cout << "rotation: " << (int)(r*180/PI)%180 << '\n';

      std::vector<cv::Point3f> rotx = {
        {1, 0, 0},
        {0, (float)cos(r), (float)-1*sin(r)},
        {0, (float)sin(r), (float)cos(r)}
      };

      std::vector<cv::Point3f> roty = {
        {(float)cos(r), 0, (float)sin(r)},
        {0, 1, 0},
        {(float)-1*sin(r), 0, (float)cos(r), }
      };

      std::vector<cv::Point3f> rotz = {
        {(float)cos(r), (float)-1*sin(r), 0},
        {(float)sin(r), (float)cos(r), 0},
        {0, 0, 1}
      };

      drawQuad(frame, squares[0], green);

      for(int l = 0; l < 32; l++) {
        // cv::Point3f r1 = rotate(rotate(rotate(tesseract[l][0], rotx), roty), rotz);
        // cv::Point3f r2 = rotate(rotate(rotate(tesseract[l][1], rotx), roty), rotz);
        std::vector<cv::Point3f> sq = {
          (cv::Point3f)rotate(rotate(rotate(tesseract[l][0], rotx), roty), rotz),
          (cv::Point3f)rotate(rotate(rotate(tesseract[l][1], rotx), roty), rotz)
        };
        cv::projectPoints(
          (std::vector<cv::Point3f>){
            (cv::Point3f){sq[0].x,sq[0].y,sq[0].z - 2},
            (cv::Point3f){sq[1].x,sq[1].y,sq[1].z - 2}
          },
          rvec, tvec, intrinsics, distortion, line2d[l]);
        cv::line(frame, line2d[l][0], line2d[l][1], blue);
      }
    }

    // (std::vector<cv::Point3f>){rotate((cv::Point3f)tesseract[l], (cv::Point3f)rotz);

    // cv::drawContours(frame, squares, -1, color);

    imshow(winname, frame);
    r+=0.01;

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

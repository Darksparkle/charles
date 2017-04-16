/* C++ headers */
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
/* C headers */
#include <stdio.h>
/* library headers */
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

/* Global variables */
std::string winname = "Window";

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int main(int argc, char** argv) {

  cv::namedWindow(winname); // main window
  cv::VideoCapture cap; // webcam capture device
  cv::Mat frame; // image frame
  cv::Mat gray;
  std::vector<cv::Point2f> corners;
  cv::Size patternsize(6,6);

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
    bool patternFound = cv::findChessboardCorners(gray, patternsize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    if(patternFound) cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    cv::drawChessboardCorners(frame, patternsize, cv::Mat(corners), patternFound);
    if(patternFound) std::cout << corners << '\n';
    imshow(winname, frame); // show frame in window

  }

  // release memory
  cap.release();
  cv::destroyWindow(winname);
  frame.deallocate();

  return 0;
}

void findSquares(cv::Mat& image, std::vector<std::vector<cv::Point> >& squares) {
  cv::Mat blurred(image);
  cv::medianBlur(image, blurred, 9);

  cv::Mat gray0(blurred.size(), CV_8U), gray;
  std::vector<std::vector<cv::Point> > contours;

  for(int c = 0; c < 3; c++) {
    int ch[] = {c, 0};

    cv::mixChannels(&blurred, 1, &gray0, 1, ch, 1);

    const int threshhold_level = 2;

    for (int l = 0; l < threshhold_level; l++) {
      if(l == 0) {
        cv::Canny(gray0, gray, 10, 20, 3);
        cv::dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
      } else {
        gray = gray0 >= (l+1) * 255 / threshhold_level;
      }
    }

    cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> approx;
    for (size_t i = 0; i < contours.size(); i++) {
      cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

      if(approx.size() == 4 && fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
        double maxCosine = 0;

        for (int j = 2; j < 5; j++) {
          double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
          maxCosine = MAX(maxCosine, cosine);
        }

        if(maxCosine < 0.3) squares.push_back(approx);
      }
    }

  }
}

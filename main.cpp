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

struct DrawData
{
    cv::ogl::Arrays arr;
    cv::ogl::Texture2D tex;
    cv::ogl::Buffer indices;
};

void draw(void* userdata)
{
  DrawData* data = static_cast<DrawData*>(userdata);
  glRotated(0.6, 0, 1, 0);
  cv::ogl::render(data->arr, data->indices, cv::ogl::TRIANGLES);
}

int main(int argc, char** argv) {

  // glutInit( &argc, argv );
  // glutInitContextVersion(2,1);
  // glutInitDisplayMode(GLUT_DOUBLE);

  cv::Mat img = cv::imread("portal.gif");

  cv::namedWindow(winname, cv::WINDOW_OPENGL); // main window
  cv::resizeWindow("OpenGL", win_width, win_height);

  cv::Mat_<cv::Vec2f> vertex(1, 4);
  vertex << cv::Vec2f(-1, 1), cv::Vec2f(-1, -1), cv::Vec2f(1, -1), cv::Vec2f(1, 1);

  cv::Mat_<cv::Vec2f> texCoords(1, 4);
  texCoords << cv::Vec2f(0, 0), cv::Vec2f(0, 1), cv::Vec2f(1, 1), cv::Vec2f(1, 0);

  cv::Mat_<int> indices(1, 6);
  indices << 0, 1, 2, 2, 3, 0;

  DrawData data;

  data.arr.setVertexArray(vertex);
  data.arr.setTexCoordArray(texCoords);
  data.indices.copyFrom(indices);
  data.tex.copyFrom(img);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double)win_width / win_height, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

  glEnable(GL_TEXTURE_2D);
  data.tex.bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDisable(GL_CULL_FACE);

  cv::setOpenGlDrawCallback(winname, draw, &data);

  // cv::VideoCapture cap; // webcam capture device
  // cv::Mat frame; // image frame
  // cv::Mat gray;
  // std::vector<cv::Point2f> corners;
  // cv::Size patternsize;
  // if(argc > 1) {
  //   patternsize = {atoi(argv[1]), atoi(argv[2])};
  // } else {
  //   patternsize = {6,6};
  // }
  // bool patternFound;

  // cv::Mat intrinsics, distortion;
  // std::vector<cv::Point3f> boardPoints;

  // file.open("points");

  // if(!cap.open(0)) {
  //   // no webcam found
  //   printf("[!] Error: couldn't load camera");
  //   return 0;
  // }

  for (;;) {
    // cap >> frame; // read from webcam into frame
    // if(frame.empty()) break; // no data found in frame
    // cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // convert frame to greyscale
    if(cv::waitKey(10) == 27) break; // wait for escape key
    if(cv::getWindowProperty(winname, 0) < 0) break; // break if window is closed
    // patternFound = cv::findChessboardCorners(gray, patternsize, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    // if(patternFound) cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    // cv::drawChessboardCorners(frame, patternsize, cv::Mat(corners), patternFound);
    // if(patternFound) file << corners << '\n' << '\n';
    // imshow(winname, frame); // show frame in window
    cv::updateWindow(winname);
  }

  // release memory
  cv::setOpenGlDrawCallback(winname, 0, 0);
  // cap.release();
  cv::destroyWindow(winname);
  // frame.deallocate();

  file.close();

  return 0;
}

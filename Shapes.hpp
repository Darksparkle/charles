      std::vector<cv::Point3f> octahedron[] = {
        {{-1, 0, 0}, {0, 1, 0}},
        {{0, 1, 0}, {1, 0, 0}},
        {{1, 0, 0}, {0, -1, 0}},
        {{0, -1, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {0, 0, 1}},
        {{0, 0, 1}, {1, 0, 0}},
        {{1, 0, 0}, {0, 0, -1}},
        {{0, 0, -1}, {-1, 0, 0}},
        {{0, 0, -1}, {0, 1, 0}},
        {{0, 0, 1}, {0, 1, 0}},
        {{0, 0, -1}, {0, -1, 0}},
        {{0, 0, 1}, {0, -1, 0}}
      };
      
      std::vector<cv::Point3f> pyramid[] = {
        {{-1, 0, 0}, {0, 0, 1}},
        {{0, 0, 1}, {1, 0, 0}},
        {{1, 0, 0}, {0, 0, -1}},
        {{0, 0, -1}, {-1, 0, 0}},
        {{0, 1, 0}, {-1, 0, 0}},
        {{0, 1, 0}, {0, 0, 1}},
        {{0, 1, 0}, {1, 0, 0}},
        {{0, 1, 0}, {0, 0, -1}}
      };
     
     std::vector<cv::Point3f> octogonal_prism[] = {
        {{1, 0, 0}, {2, 0, 0}},
        {{2, 0, 0}, {3, 0, 1},
        {{3, 0, 1}, {3, 0, 2},
        {{3, 0, 2}, {2, 0, 3},
        {{2, 0, 3}, {1, 0, 3}},
        {{1, 0, 3}, {0, 0, 2},
        {{0, 0, 2}, {0, 0, 1}},
        {{0, 0, 1}, {1, 0, 0}},
        
        {{1, 3, 0}, {2, 3, 0}},
        {{2, 3, 0}, {3 3, 1},
        {{3, 3, 1}, {3 3, 2},
        {{3, 3, 2}, {2, 3, 3},
        {{2, 3 3}, {1 3, 3}},
        {{1, 3, 3}, {0 3, 2},
        {{0, 3, 2}, {0, 3, 1}},
        {{0, 3, 1}, {1, 3, 0}},
        
        {{1, 0, 0}, {1, 3, 0}},
        {{2, 0, 0}, {2, 3, 0}},
        {{3, 0, 1}, {3, 3, 1}},
        {{3, 0, 2}, {3, 3, 2}},
        {{2, 0, 3}, {2, 3, 3}},
        {{1, 0, 3}, {1, 3, 3}},
        {{0, 0, 2}, {0, 3, 2}},
        {{0, 0, 1}, {0, 3, 1}}
      };
      
     std::vector<cv::Point3f> blehblahbloop[] = {
        
      }; 
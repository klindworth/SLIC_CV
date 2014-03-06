#ifndef SLIC_ADAPTOR_H
#define SLIC_ADAPTOR_H

#include <opencv2/core/core.hpp>

cv::Mat convertBGRtoARGB(const cv::Mat& input);
cv::Mat convertARGBtoBGR(const cv::Mat& input);
void drawLabelContours(cv::Mat& image, cv::Mat& labels_cv);
int slicSuperpixels(cv::Mat bgr_input, cv::Mat& labels, int superpixelsize, double compactness);

#endif // SLIC_ADAPTOR_H

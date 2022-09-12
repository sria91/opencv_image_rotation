#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <iostream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

cv::Mat rotate_image(const cv::Mat& input_image, const int angle) {
    int rows = input_image.rows;
    int cols = input_image.cols;
    double angle_in_radians = angle * M_PI / 180;
    double alpha = cos(angle_in_radians);
    double beta = sin(angle_in_radians);
    double xc = rows / 2;
    double yc = cols / 2;

    int new_rows = static_cast<int>(rows * abs(alpha) + cols * abs(beta));
    int new_cols = static_cast<int>(rows * abs(beta) + cols * abs(alpha));

    cv::Mat M = cv::Mat(2, 3, CV_64F);
    M.at<double>(0, 0) = alpha;
    M.at<double>(0, 1) = beta;
    M.at<double>(0, 2) = (1 - alpha) * yc - beta * xc + new_cols / 2 - yc;
    M.at<double>(1, 0) = -beta;
    M.at<double>(1, 1) = alpha;
    M.at<double>(1, 2) = beta * yc + (1 - alpha) * xc + new_rows / 2 - xc;

    cv::Mat output_image;
    cv::warpAffine(input_image, output_image, M, cv::Size(new_cols, new_rows));

    return output_image;
}

int main() {
    cv::Mat input_image = cv::imread("sria91.png");
    cv::Mat output_image = rotate_image(input_image, 23);
    cv::imshow("Output", output_image);
    cv::waitKey(10000);
    return 0;
}

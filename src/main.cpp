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

    cv::Mat rotation_matrix = cv::Mat::eye(3, 3, CV_64F);
    rotation_matrix.at<double>(0, 0) = cos(angle_in_radians);
    rotation_matrix.at<double>(0, 1) = -sin(angle_in_radians);
    rotation_matrix.at<double>(1, 0) = sin(angle_in_radians);
    rotation_matrix.at<double>(1, 1) = cos(angle_in_radians);

    cv::Mat translation_matrix = cv::Mat::eye(3, 3, CV_64F);
    translation_matrix.at<double>(0, 2) = - rows / 2;
    translation_matrix.at<double>(1, 2) = - cols / 2;

    cv::Mat corners = cv::Mat::ones(3, 4, CV_64F);
    corners.at<double>(0, 0) = 0;
    corners.at<double>(1, 0) = 0;
    corners.at<double>(0, 1) = 0;
    corners.at<double>(1, 1) = cols - 1;
    corners.at<double>(0, 2) = rows - 1;
    corners.at<double>(1, 2) = cols - 1;
    corners.at<double>(0, 3) = rows - 1;
    corners.at<double>(1, 3) = 0;

    // cout << rotation_matrix << endl;
    // cout << translation_matrix << endl;
    cv::Mat new_corners = (rotation_matrix * translation_matrix) * corners;

    // cout << rows << ", " << cols << endl;
    // cout << corners << endl;
    // cout << new_corners << endl;

    double *p_new_corners;
    p_new_corners = new_corners.row(0).ptr<double>();
    int max_rows = *max_element(p_new_corners, p_new_corners+4);
    int min_rows = *min_element(p_new_corners, p_new_corners+4);
    p_new_corners = new_corners.row(1).ptr<double>();
    int max_cols = *max_element(p_new_corners, p_new_corners+4);
    int min_cols = *min_element(p_new_corners, p_new_corners+4);

    int new_rows = max_rows - min_rows + 1;
    int new_cols = max_cols - min_cols + 1;
    // cout << new_rows << ", " << new_cols << endl;

    cv::Mat new_translation_matrix = cv::Mat::eye(3, 3, CV_64F);
    new_translation_matrix.at<double>(0, 2) = new_rows / 2;
    new_translation_matrix.at<double>(1, 2) = new_cols / 2;

    cv::Mat transformation_matrix = (new_translation_matrix * rotation_matrix * translation_matrix);
    // cout << transformation_matrix << endl;
    cv::Mat inverse_transformation_matrix = transformation_matrix.inv();
    // cout << inverse_transformation_matrix << endl;

    cv::Mat output_image(new_rows, new_cols, input_image.type());
    cv::Mat new_point = cv::Mat::ones(3, 1, CV_64F);
    for (int new_row=0; new_row<new_rows; new_row++) {
        for (int new_col=0; new_col<new_cols; new_col++) {
            new_point.at<double>(0, 0) = new_row;
            new_point.at<double>(1, 0) = new_col;
            cv::Mat point = inverse_transformation_matrix * new_point;
            int row = point.at<double>(0, 0);
            int col = point.at<double>(1, 0);
            if (row >= 0 && row < rows && col >= 0 && col < cols) {
                output_image.at<cv::Vec3b>(cv::Point(new_col, new_row)) = input_image.at<cv::Vec3b>(cv::Point(col, row));
            }
        }
    }
    return output_image;
}

int main() {
    cv::Mat input_image = cv::imread("sria91.png");
    cv::Mat output_image = rotate_image(input_image, 23);
    cv::imshow("Output", output_image);
    cv::waitKey(10000);
    return 0;
}

# Correctly rotating an image to an arbitrary angle in OpenCV

## Introduction

Have you ever tried to rotate an image in OpenCV? It's damn simple, right?

1. You include the OpenCV headers:

```cpp
#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
```

2. You load the image:

```cpp
cv::Mat image = cv::imread("sria91.png");
```

Here's how my `image` looks: 

![input.png](input.png)

3. You call `cv::rotate`. For example to rotate an image clockwise by 90 degrees:

```cpp
cv::Mat rotated_image = cv::rotate(image, cv::ROTATE_90_CLOCKWISE);
```

4. And then you can display the image:

```cpp
cv::imshow("Output", output_image);
cv::waitKey(10000);
```

Here's how the `output_image` looks: 

![cv_rotate_-90.png](cv_rotate_-90.png)

But what if you wanted to rotate the image by an arbitrary angle say 23 degrees anti-clockwise? How would you go about that?

## Naive approach

If you look at the OpenCV's official C++ tutorials you would be out of luck. But, if you dig deeper you might come across OpenCV's official Python tutorials and find [this](https://docs.opencv.org/4.x/da/d6e/tutorial_py_geometric_transformations.html). Adapting the example on that page for rotation to C++ you could write the following:

```cpp
cv::Mat input_image = cv::imread("sria91.png");
int rows = input_image.rows;
int cols = input_image.cols;
cv::Mat transformation_matrix = cv::getRotationMatrix2D(cv::Point2f((cols-1)/2.0, (rows-1)/2.0), 23, 1);
cv::Mat output_image;
cv::warpAffine(input_image, output_image, transformation_matrix, cv::Size(cols, rows));
cv::imshow("Output", output_image);
cv::waitKey(10000);
```

The result looks like this: 

![cv_warpAffine_rotate_23.png](cv_warpAffine_rotate_23.png)

You would notice that something is not right. The output image is cropped from all around. How would you get an image that is rotated and also not cropped? I've got you covered. Follow along to find out.

## The correct approach

This approach is based on the first principles.

1. The first thing you have to do is to construct the rotation matrix.

```cpp
double angle = 23;  // in degrees
double angle_in_radians = angle * M_PI / 180;
cv::Mat rotation_matrix = cv::Mat::eye(3, 3, CV_64F);
rotation_matrix.at<double>(0, 0) = cos(angle_in_radians);
rotation_matrix.at<double>(0, 1) = -sin(angle_in_radians);
rotation_matrix.at<double>(1, 0) = sin(angle_in_radians);
rotation_matrix.at<double>(1, 1) = cos(angle_in_radians);
```

2. Then you construct the translation matrix.

```cpp
cv::Mat translation_matrix = cv::Mat::eye(3, 3, CV_64F);
translation_matrix.at<double>(0, 2) = - rows / 2;
translation_matrix.at<double>(1, 2) = - cols / 2;
```

3. Collect the coordinates for corners of the `image`.

```cpp
cv::Mat corners = cv::Mat::ones(3, 4, CV_64F);
corners.at<double>(0, 0) = 0;
corners.at<double>(1, 0) = 0;
corners.at<double>(0, 1) = 0;
corners.at<double>(1, 1) = cols - 1;
corners.at<double>(0, 2) = rows - 1;
corners.at<double>(1, 2) = cols - 1;
corners.at<double>(0, 3) = rows - 1;
corners.at<double>(1, 3) = 0;
```

4. Find the coordinates of the corners of the `image` when its center is translated to the origin and rotated by `angle`.

```cpp
cv::Mat new_corners = (rotation_matrix * translation_matrix) * corners;
```

5. Calculate the rows and columns for the output image.

```cpp
double *p_new_corners;
p_new_corners = new_corners.row(0).ptr<double>();
int max_rows = *max_element(p_new_corners, p_new_corners+4);
int min_rows = *min_element(p_new_corners, p_new_corners+4);
p_new_corners = new_corners.row(1).ptr<double>();
int max_cols = *max_element(p_new_corners, p_new_corners+4);
int min_cols = *min_element(p_new_corners, p_new_corners+4);
int new_rows = max_rows - min_rows + 1;
int new_cols = max_cols - min_cols + 1;
```

6. Construct the `new_translation_matrix` to translate the image's center from the origin.

```cpp
cv::Mat new_translation_matrix = cv::Mat::eye(3, 3, CV_64F);
new_translation_matrix.at<double>(0, 2) = new_rows / 2;
new_translation_matrix.at<double>(1, 2) = new_cols / 2;
```

7. Compute the `inverse_transformation_matrix` to map the coordinates from `output_image` to `input_image`.

```cpp
cv::Mat inverse_transformation_matrix = (new_translation_matrix * rotation_matrix * translation_matrix).inv();
```

8. Now you allocate the memory for the `output_image`.

```cpp
cv::Mat output_image(new_rows, new_cols, input_image.type());
```

9. Finally rotate and display the image.

```cpp
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
cv::imshow("Output", input_image);
cv::waitKey(10000);
```

And the result looks like this: 

![correct_rotate_23.png](correct_rotate_23.png)

[Link to the code](https://github.com/sria91/opencv_image_rotation.git)

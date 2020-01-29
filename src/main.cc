#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

int main() {
    cv::Mat image;
    std::string path = "/home/horky/Desktop/image.jpg";
    std::string gray_path = "/home/horky/Desktop/gray_image.jpg";
    image = cv::imread(path);

    //If image not found
    if (!image.data) {
        std::cerr << "No image data \n";
        return -1;
    }

    //Converting the Image into GrayScale and Storing it in a Matrix 'img_gray'
    cv::Mat img_gray;
    cv::cvtColor(image, img_gray, cv::COLOR_RGB2GRAY);

    //Display the original image
    // cv::namedWindow("Display Original Image", cv::WINDOW_NORMAL);
    // cv::imshow("Display Original Image", image);

    //Display the grayscale image
    // cv::namedWindow("Display Grayscale Image", cv::WINDOW_NORMAL);
    // cv::imshow("Display Grayscale Image", img_gray);

    //Save the grayscale image with a name 'gray.jpg'
    cv::imwrite(gray_path, img_gray);

    cv::waitKey(0);
}

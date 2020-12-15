#pragma once

#ifndef UTILS_H
#define UTILS_H
#include <algorithm>
#include <tuple>
#include <execution>
#include <ranges>
#include <iterator>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>

#include "Range.hpp"

#include "Math.hpp"

#include "Grid.hpp"

namespace utils
{
    namespace opencv
    {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename inner_type, int size>
        void printVector(const cv::Vec<inner_type, size>& vector)
        {
            if constexpr (size == 0)
            {
                return;
            }
            else
            {
                std::string acc = std::to_string(vector.val[0]);

                for (int i = 1; i < size; i++)
                {
                    acc += std::string("\t") + std::to_string(vector.val[i]);
                }
                std::cout << acc << "\n";
            }
        }

        template<typename inner_type, int size>
        inner_type length(const cv::Vec<inner_type, size>& vector)
        {
            return std::sqrt(vector.val[0] * vector.val[0] + vector.val[1] * vector.val[1] + vector.val[2] * vector.val[2]);
        }

        template<typename inner_type, int size>
        cv::Vec<inner_type, size> div(const cv::Vec<inner_type, size>& vector, const inner_type& divisor)
        {
            if constexpr (size == 0)
            {
                return cv::Vec<inner_type, 0>();
            }
            else
            {
                return { vector.val[0] / divisor, vector.val[1] / divisor, vector.val[2] / divisor };
            }
        }

        template<typename inner_type, int size>
        cv::Vec<inner_type, size> div(const cv::Vec<inner_type, size>& vector, const cv::Vec<inner_type, size>& divisor)
        {
            if constexpr (size == 0)
            {
                return cv::Vec<inner_type, 0>();
            }
            else
            {
                return { vector.val[0] / divisor.val[0], vector.val[1] / divisor.val[1], vector.val[2] / divisor.val[2] };
            }
        }

        template<typename inner_type, int size>
        cv::Vec<inner_type, size> normalize(const cv::Vec<inner_type, size>& vector)
        {
            if constexpr (size == 0)
            {
                return cv::Vec<inner_type, 0>();
            }
            else
            {
                const inner_type l = utils::opencv::length(vector);
                return utils::opencv::div(vector, l == 0.0 ? 1.0 : l);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////// open cv related functions & wrappers //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        using cv::WindowFlags; using cv::ImreadModes; using cv::ColorConversionCodes; using cv::BorderTypes; using cv::InterpolationFlags;
        using cv::Mat;
        using cv::Scalar; using cv::Point; using cv::Size;
        using cv::imread; using cv::waitKey; using cv::cvtColor; using cv::warpAffine;
        using cv::waitKey; using cv::destroyAllWindows;

        static bool SKIP_ALL_INPUT = false;
        static int MAX_KERNEL_LENGTH = 4;

        inline void getUserInput()
        {
            if (SKIP_ALL_INPUT)
                return;
            waitKey();
        }

        inline void display(const Mat& image, const std::string window_name = "Display image", const int flag = WindowFlags::WINDOW_NORMAL, const int wx = 900, const int wy = 600)
        {
            cv::namedWindow(window_name, flag);// Create a window for display.
            if (flag != WindowFlags::WINDOW_AUTOSIZE) // same as flag != WindowFlags::WINDOW_FULLSCREEN)
            {
                cv::resizeWindow(window_name, wx, wy);
            }
            imshow(window_name, image);                   // Show our image inside it.
        }

        inline Mat loadImage(std::string image_path, std::string image_name, int image_read_mode = ImreadModes::IMREAD_COLOR)
        {
            Mat image = imread(image_path + image_name, image_read_mode);   // Read the file
            if (!image.data)                                                // Check for invalid input
            {
                std::cout << "Could not open or find the image" << std::endl;
                exit(-1);
            }
            return image;
        }

        inline void saveImage(Mat image, std::string image_path, std::string image_name)
        {
            std::cout << "Saving image at: " << image_path << image_name << std::endl;
            cv::imwrite(image_path + image_name,
                        image); //no ideas why it is working
        }

        inline Mat addImage(Mat src, Mat added, cv::InputArray mask = Mat())
        {
            Mat result;
            cv::add(src, added, result, mask);
            return result;
        }

        inline Mat resize(Mat image, float fx, float fy, InterpolationFlags interpolation_flag)
        {
            Mat result;
            cv::resize(image, result, Size(), fx, fy, interpolation_flag);
            return result;
        }

        inline Mat blur(const Mat& src, const size_t size = MAX_KERNEL_LENGTH)
        {
            Mat result;
            cv::blur(src, result, cv::Size2l(size, size));
            return result;
        }

        inline Mat medianBlur(Mat src, size_t size = MAX_KERNEL_LENGTH)
        {
            Mat result;
            for (int i = 1; i < size; i = i + 2)
            {
                cv::medianBlur(src, result, i);
            }
            return result;
        }

        inline Mat noise(Mat image, float m = 10, float sigma = 5)
        {
            Mat result(image);
            Mat noise(image.size(), image.type());
            cv::randn(noise, m, sigma); //mean and variance
            result += noise;
            return result;
        }

    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////// utility functions for general use /////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    inline T clamp(T value, T min, T max)
    {
        return std::max(min, std::min(value, max));
    }

    template<typename Discrete>
    inline Discrete to1dInd(const Discrete x, const Discrete y, const Discrete xSize) noexcept
    {
        return y * xSize + x;
    }

    template<typename Discrete>
    inline std::pair<Discrete, Discrete> from1dInd(const Discrete ind, const Discrete xSize) noexcept
    {
        return { ind % xSize, ind / xSize };
    }

    namespace opencv
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////// custom for each with parallel support /////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //returns [masked]
        template<typename vec_inner_type, int size, int masksCount>
        inline cv::Mat_<cv::Vec<vec_inner_type, size>> maskChannels(const cv::Mat_<cv::Vec<vec_inner_type, size>>& image, const std::array<size_t, masksCount>& channelsToMask)
        {
            cv::Mat_<cv::Vec<vec_inner_type, size>> result = cv::Mat_<cv::Vec<vec_inner_type, size>>::zeros(image.size());

            auto range = Range(0, result.cols * result.rows - 1);
            std::for_each(std::execution::par, range.begin(), range.end(),
                            [&image, &result, &channelsToMask](const int ind) -> void
                            {
                                const auto [x, y] = from1dInd(ind, image.cols);
                                const cv::Vec<vec_inner_type, size> oldPixel = image.at<cv::Vec<vec_inner_type, size>>(x, y);

                                cv::Vec<vec_inner_type, size> newPixel = oldPixel;

                                for (size_t c = 0; c < size; c++)
                                {
                                    bool shouldRemain = true;
                                    for (size_t m = 0; m < masksCount; m++)
                                    {
                                        if (channelsToMask[m] == c)
                                        {
                                            shouldRemain = false;
                                            break;
                                        }
                                    }
                                    newPixel.val[c] = shouldRemain ? newPixel.val[c] : 0.0;
                                }
                                result.at<cv::Vec<vec_inner_type, size>>(x, y) = newPixel;
                            });
            return result;
        }

        //returns [diffMap]
        template<typename vec_inner_type, int size>
        inline cv::Mat_<cv::Vec<vec_inner_type, size>> diff(const cv::Mat_<cv::Vec<vec_inner_type, size>>& image,
                                                            const cv::Mat_<cv::Vec<vec_inner_type, size>>& example)
        {
            cv::Mat_<cv::Vec<vec_inner_type, size>> result = cv::Mat_<cv::Vec<vec_inner_type, size>>::zeros(image.size());

            auto range = Range(0, result.cols * result.rows - 1);
            std::for_each(std::execution::par, range.begin(), range.end(),
                            [&image, &example, &result](const int ind) -> void
                            {
                                const auto [x, y] = from1dInd(ind, image.cols);
                                const cv::Vec<vec_inner_type, size> oldPixel = image.at<cv::Vec<vec_inner_type, size>>(x, y);
                                const cv::Vec<vec_inner_type, size> examplePixel = example.at<cv::Vec<vec_inner_type, size>>(x, y);

                                cv::Vec<vec_inner_type, size> newPixel = oldPixel;

                                for (size_t c = 0; c < size; c++)
                                {
                                    newPixel.val[c] -= examplePixel.val[c];
                                }
                                result.at<cv::Vec<vec_inner_type, size>>(x, y) = newPixel;
                            });
            return result;
        }

    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // !UTILS_H

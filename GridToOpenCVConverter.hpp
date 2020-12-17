#pragma once

#ifndef _GRID_TO_OPEN_CV_CONVERTER_HPP_
#define _GRID_TO_OPEN_CV_CONVERTER_HPP_

#include <functional>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "Grid.hpp"

namespace converter
{
    template<typename inner_type>
    inline cv::Mat1d to_Mat1d_image (const Grid<inner_type>& grid, const inner_type& min_value, const inner_type& max_value,
                                     std::function<double (inner_type)> convert_function = [](inner_type value)
                                     {
                                         return (double)value;
                                     })
    {
        cv::Mat1d result (grid.get_x_size (), grid.get_y_size ());
        grid.for_each ([&result, min_value, max_value, convert_function](const uint32_t x, const uint32_t y, const inner_type& value)
                       {
                           const double diapason = convert_function (max_value - min_value);
                           const double value_to_norm = convert_function (value - min_value) / diapason;
                           result.at<double> (x, y) = value_to_norm;
                       }
        );
        return result;
    }

    template<typename inner_type>
    inline cv::Mat2d to_Mat2d_image (const Grid<inner_type>& grid, const inner_type& min_value, const inner_type& max_value)
    {
        cv::Mat2d result (grid.get_x_size (), grid.get_y_size ());
        grid.for_each ([&result, min_value, max_value](const uint32_t x, const uint32_t y, const inner_type& value)
                    {
                        for ( auto c = 0; c < 2; c++ )
                        {
                            result.at<cv::Vec2d> (x, y)[c] = (value[c] - min_value[c]) * 1.0 / (max_value[c] - min_value[c]);
                        }
                    }
        );
        return result;
    }

    template<typename inner_type>
    inline cv::Mat3d to_Mat3d_image (const Grid<inner_type>& grid, const inner_type& min_value, const inner_type& max_value)
    {
        cv::Mat3d result (grid.get_x_size (), grid.get_y_size ());
        grid.for_each ([&result, min_value, max_value](const uint32_t x, const uint32_t y, const inner_type& value)
                    {
                        for ( auto c = 0; c < 3; c++ )
                        {
                            result.at<cv::Vec3d> (x, y)[c] = (value[c] - min_value[c]) * 1.0 / (max_value[c] - min_value[c]);
                        }
                    }
        );
        return result;
    }

    template<typename inner_type>
    inline cv::Mat4d to_Mat4d_image (const Grid<inner_type>& grid, const inner_type& min_value, const inner_type& max_value)
    {
        cv::Mat4d result (grid.get_x_size (), grid.get_y_size ());
        grid.for_each ([&result, min_value, max_value](const uint32_t x, const uint32_t y, const inner_type& value)
                    {
                        for ( auto c = 0; c < 4; c++ )
                        {
                            result.at<cv::Vec4d> (x, y)[c] = (value[c] - min_value[c]) * 1.0 / (max_value[c] - min_value[c]);
                        }
                    }
        );
        return result;
    }

    template<typename inner_type, size_t channels>
    inline cv::Mat_<cv::Vec<double, channels>> to_MatNd_image (const Grid<inner_type>& grid, const inner_type& min_value, const inner_type& max_value)
    {
        cv::Mat_<cv::Vec<double, channels>> result (grid.get_x_size (), grid.get_y_size ());
        grid.for_each ([&result, min_value, max_value](const uint32_t x, const uint32_t y, const inner_type& value)
                    {
                        for ( auto c = 0; c < channels; c++ )
                        {
                            result[x][y][c] = (value[c] - min_value[c]) / (max_value[c] - min_value[c]);
                        }
                    }
        );
        return result;
    }



    inline cv::Mat3d flipChannels (const cv::Mat3d& image)
    {
        cv::Mat3d result = cv::Mat3d::zeros(image.rows, image.cols);
        for ( auto col = 0; col < result.cols; col++ )
        {
            for ( auto row = 0; row < result.rows; row++ )
            {
                for ( auto c = 0; c < 3; c++ )
                {
                    result.at<cv::Vec3d> (row, col)[c] = image.at<cv::Vec3d> (row, col)[2-c];
                }
            }
        }
        return result;
    }



    inline cv::Mat1d remap (const cv::Mat1d& image, const double old_min_value, const double old_max_value, const double new_min_value, const double new_max_value)
    {
        cv::Mat1d result = image.clone ();
        for ( auto col = 0; col < result.cols; col++ )
        {
            for ( auto row = 0; row < result.rows; row++ )
            {
                const auto val = result.at<double> (row, col);
                const double normed = (val - old_min_value) / (old_max_value - old_max_value);
                result.at<double> (row, col) = (normed) * (new_max_value - new_min_value) + new_min_value;
            }
        }
        return result;
    }

    inline cv::Mat2d remap (const cv::Mat2d& image, const double old_min_value, const double old_max_value, const double new_min_value, const double new_max_value)
    {
        cv::Mat2d result = image.clone();
        for ( auto col = 0; col < result.cols; col++ )
        {
            for ( auto row = 0; row < result.rows; row++ )
            {
                for ( auto c = 0; c < 2; c++ )
                {
                    const auto val = result.at<cv::Vec2d> (row, col)[c];
                    const double normed = (val - old_min_value) / (old_max_value - old_max_value);
                    result.at<cv::Vec2d> (row, col)[c] = (normed) * (new_max_value - new_min_value) + new_min_value;
                }
            }
        }
        return result;
    }

    inline cv::Mat3d remap (const cv::Mat3d& image, const double old_min_value, const double old_max_value, const double new_min_value, const double new_max_value)
    {
        cv::Mat3d result = image.clone ();
        for ( auto col = 0; col < result.cols; col++ )
        {
            for ( auto row = 0; row < result.rows; row++ )
            {
                for ( auto c = 0; c < 3; c++ )
                {
                    const double val = result.at<cv::Vec3d> (row, col)[c];
                    const double diff = old_max_value - old_min_value;
                    const double normed = diff  == 0.0 ? 1.0 : ((val - old_min_value) / diff);
                    result.at<cv::Vec3d> (row, col)[c] = (normed)*(new_max_value - new_min_value) + new_min_value;
                }
            }
        }
        return result;
    }



    inline cv::Mat3d prepareNormal (const cv::Mat3d& normal)
    {
        return converter::remap (converter::flipChannels (normal), -1.0, 1.0, 0.0, 1.0);
    }

    inline cv::Mat3d prepareNormal (const Grid<glm::f64vec3>& normal)
    {
        return converter::flipChannels (converter::to_Mat3d_image<glm::f64vec3> (normal, glm::f64vec3 (-1), glm::f64vec3 (1)));
    }
}

#endif // !_GRID_TO_OPEN_CV_CONVERTER_HPP_

#pragma once

#include <concepts>
#include <opencv2/core.hpp>

namespace FairyCam
{

template <typename T, typename... Args>
concept IsAnyCamera = requires(T v, cv::Mat m, cv::UMat um) {
    {
        v.open(int{}, int{}, std::vector<int>{})
    } -> std::convertible_to<bool>;
    {
        v.isOpened()
    } -> std::convertible_to<bool>;
    {
        v.grab()
    } -> std::convertible_to<bool>;
    {
        v.retrieve(cv::Mat(), int{})
    } -> std::convertible_to<bool>;
    {
        v.set(int{}, double{})
    } -> std::convertible_to<bool>;
    {
        v.get(int{})
    } -> std::convertible_to<double>;
    {
        v.setExceptionMode(bool{})
    };
    {
        v >> m
    } -> std::convertible_to<T &>;
    {
        v >> um
    } -> std::convertible_to<T &>;
};

}
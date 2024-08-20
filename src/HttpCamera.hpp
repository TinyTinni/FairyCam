#pragma once

#include <memory>
#include <opencv2/core.hpp>
#include <vector>

namespace FairyCam
{

class HttpCamera
{
  public:
    struct Options
    {
        uint16_t port = 5001;
        int grabTimeOutMs = 10000;
    };

    HttpCamera() : HttpCamera(Options{}) {}
    HttpCamera(Options opts);
    ~HttpCamera();
    bool open(int idx, int apiPreference, const std::vector<int> &params);
    bool isOpened() const noexcept;
    void release() noexcept;
    bool grab();
    bool retrieve(cv::OutputArray image, int flag = 0);
    bool read(cv::OutputArray image);
    bool set([[maybe_unused]] int propId, [[maybe_unused]] double value)
    {
        return false;
    }
    double get([[maybe_unused]] int propId) const noexcept { return -1.0; }
    void setExceptionMode([[maybe_unused]] bool enable) {}
    bool getExceptionMode() const noexcept { return false; }
    HttpCamera &operator>>(cv::Mat &image)
    {
        read(image);
        return *this;
    }
    HttpCamera &operator>>(cv::UMat &image)
    {
        read(image);
        return *this;
    }

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace FairyCam

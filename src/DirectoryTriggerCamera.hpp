#pragma once
#include <filesystem>
#include <opencv2/core.hpp>
#include <vector>

namespace FairyCam
{

class DirectoryTriggerCamera
{
  public:
    struct Options
    {
        std::filesystem::path dir = "";
        uint32_t grabTimeOutMs = 10000;
    };

    DirectoryTriggerCamera() : DirectoryTriggerCamera(Options{}) {}
    DirectoryTriggerCamera(Options opts);
    ~DirectoryTriggerCamera();
    bool open(int idx, int apiPreference, const std::vector<int> &params);
    bool isOpened() const;
    void release();
    bool grab();
    bool retrieve(cv::OutputArray image, int flag = 0);
    bool read(cv::OutputArray image);
    bool set(int propId, double value) { return false; }
    double get(int propId) const { return -1.0; }
    void setExceptionMode(bool enable) {}
    bool getExceptionMode() const { return false; }
    DirectoryTriggerCamera &operator>>(cv::Mat &image)
    {
        read(image);
        return *this;
    }
    DirectoryTriggerCamera &operator>>(cv::UMat &image)
    {
        read(image);
        return *this;
    }

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace FairyCam

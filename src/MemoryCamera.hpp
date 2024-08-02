#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <vector>

namespace FairyCam
{

class MemoryCamera
{
  public:
    using ImageContainer = std::vector<cv::Mat>;
    struct Options
    {

        ImageContainer images = {};
        bool circular = false;
    };

  private:
    Options m_opts;
    bool m_is_open = false;
    ImageContainer::const_iterator m_current;
    ImageContainer::const_iterator m_next;

  public:
    MemoryCamera() : MemoryCamera(Options{}) {}
    MemoryCamera(Options opts)
        : m_opts{std::move(opts)}, m_current{m_opts.images.cend()},
          m_next{m_opts.images.cend()}
    {
    }
    bool open([[maybe_unused]] int idx, [[maybe_unused]] int apiPreference,
              [[maybe_unused]] const std::vector<int> &params)
    {
        m_is_open = true;
        m_next = m_opts.images.cbegin();
        return true;
    }
    bool isOpened() const { return m_is_open; }
    void release()
    {
        m_is_open = false;
        m_current = m_opts.images.cend();
        m_next = m_opts.images.cend();
    }
    bool grab();
    bool retrieve(cv::OutputArray image, int flag = 0);
    bool read(cv::OutputArray image);
    bool set([[maybe_unused]] int propId, [[maybe_unused]] double value)
    {
        return false;
    }
    double get([[maybe_unused]] int propId) const { return -1.0; }
    void setExceptionMode([[maybe_unused]] bool enable) {}
    bool getExceptionMode() const { return false; }
    MemoryCamera &operator>>(CV_OUT cv::Mat &image)
    {
        read(image);
        return *this;
    }
    MemoryCamera &operator>>(CV_OUT cv::UMat &image)
    {
        read(image);
        return *this;
    }
};

} // namespace FairyCam

#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <vector>

namespace FairyCam
{

class FileCamera
{
  public:
    using FilesContainer = std::vector<std::string>;
    struct Options
    {

        FilesContainer files = {};
        bool circular = false;
    };

  private:
    Options m_opts;
    bool m_is_open = false;
    FilesContainer::const_iterator m_current;
    FilesContainer::const_iterator m_next;

  public:
    FileCamera() : FileCamera(Options{}) {}
    FileCamera(Options opts)
        : m_opts{std::move(opts)}, m_current{m_opts.files.cend()},
          m_next{m_opts.files.cend()}
    {
    }
    bool open(int idx, int apiPreference, const std::vector<int> &params)
    {
        m_is_open = true;
        m_next = m_opts.files.cbegin();
        return true;
    }
    bool isOpened() const { return m_is_open; }
    void release()
    {
        m_is_open = false;
        m_current = m_opts.files.cend();
        m_next = m_opts.files.cend();
    }
    bool grab();
    bool retrieve(cv::OutputArray image, int flag = 0);
    bool read(cv::OutputArray image);
    bool set(int propId, double value) { return false; }
    double get(int propId) const { return -1.0; }
    void setExceptionMode(bool enable) {}
    bool getExceptionMode() const { return false; }
    FileCamera &operator>>(CV_OUT cv::Mat &image)
    {
        read(image);
        return *this;
    }
    FileCamera &operator>>(CV_OUT cv::UMat &image)
    {
        read(image);
        return *this;
    }
};

} // namespace FairyCam

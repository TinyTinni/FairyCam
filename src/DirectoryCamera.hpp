#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace FairyCam
{

class DirectoryCamera
{
  public:
    struct Options
    {
        std::filesystem::path dir = "";
        bool circular = false;
    };

  private:
    Options m_opts;
    bool m_is_open = false;
    bool m_throw = false;
    std::filesystem::directory_iterator m_iter;
    std::filesystem::directory_iterator m_iter_end;
    std::filesystem::path m_file;

  public:
    DirectoryCamera() : DirectoryCamera(Options{}) {}
    DirectoryCamera(Options opts) : m_opts{std::move(opts)} {}
    bool open([[maybe_unused]] int idx, [[maybe_unused]] int apiPreference,
              [[maybe_unused]] const std::vector<int> &params)
    {
        if (!std::filesystem::is_directory(m_opts.dir))
            return false;
        m_is_open = true;
        m_iter = std::filesystem::directory_iterator(m_opts.dir);
        return true;
    }
    bool isOpened() const { return m_is_open; }
    void release() { m_is_open = false; }
    bool grab()
    {
        if (!m_is_open || (m_iter == m_iter_end))
            return false;
        m_file = m_iter->path();
        ++m_iter;
        if (m_opts.circular && m_iter == m_iter_end)
            m_iter = std::filesystem::directory_iterator(m_opts.dir);

        return std::filesystem::exists(m_file);
    }
    bool retrieve(cv::OutputArray image, [[maybe_unused]] int flag = 0)
    {
        auto mat = cv::imread(m_file.string());
        image.assign(mat);
        return !mat.empty();
    }
    bool read(cv::OutputArray image)
    {
        if (!grab())
        {
            image.assign(cv::Mat());
            return false;
        }
        return retrieve(image);
    }
    bool set([[maybe_unused]] int propId, [[maybe_unused]] double value)
    {
        return false;
    }
    double get([[maybe_unused]] int propId) const { return -1.0; }
    void setExceptionMode([[maybe_unused]] bool enable) {}
    bool getExceptionMode() const { return false; }
    DirectoryCamera &operator>>(CV_OUT cv::Mat &image)
    {
        read(image);
        return *this;
    }
    DirectoryCamera &operator>>(CV_OUT cv::UMat &image)
    {
        read(image);
        return *this;
    }
};

} // namespace FairyCam

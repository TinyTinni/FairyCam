#pragma once

#include "../AnyCamera.hpp"
#include "Exceptions.hpp"
#include "Sequence.hpp"
#include <concepts>
#include <memory>
#include <opencv2/core.hpp>

namespace FairyCam
{

class ChaosCamera
{
  private:
    bool m_active_exception = true;
    AnyCamera m_cam;
    std::unique_ptr<Sequence> m_error_sequence;

    bool checkIsOpen()
    {
        try
        {
            m_error_sequence->checkIsOpen();
        }
        catch (...)
        {
            this->release();
            if (!m_active_exception)
                return false;
            throw;
        }
        return true;
    }

    bool checkGrab()
    {
        try
        {
            m_error_sequence->checkGrab();
        }
        catch (...)
        {
            if (!m_active_exception)
                return false;
            throw;
        }
        return true;
    }

    bool checkRetrieve()
    {
        try
        {
            m_error_sequence->checkRetrieve();
        }
        catch (...)
        {
            if (!m_active_exception)
                return false;
            throw;
        }
        return true;
    }

  public:
    template <std::derived_from<Sequence> SeqT>
    ChaosCamera(AnyCamera &&camera, SeqT seq)
        : m_cam{std::move(camera)},
          m_error_sequence{std::make_unique<SeqT>(std::move(seq))}
    {
    }
    bool open(int idx, int apiPreference, const std::vector<int> &params)
    {
        if (!checkIsOpen())
            return false;
        return m_cam.open(idx, apiPreference, params);
    }
    bool isOpened() const
    {
        if (!const_cast<ChaosCamera *>(this)->checkIsOpen())
            return false;
        return m_cam.isOpened();
    }
    void release() { m_cam.release(); }
    bool grab()
    {
        if (!isOpened())
            return false;
        if (!checkGrab())
            return false;
        if (!m_cam.grab())
        {
            if (m_active_exception)
                throw GrabException{};
            return false;
        }
        return true;
    }
    bool retrieve(cv::OutputArray image, int flag = 0)
    {
        if (!isOpened())
            return false;
        if (!checkRetrieve())
            return false;
        if (!m_cam.retrieve(image, flag))
        {
            if (m_active_exception)
                throw RetrieveException{};
            return false;
        }
        return true;
    }
    bool read(cv::OutputArray image)
    {
        if (!grab())
            return false;
        return retrieve(image);
    }
    bool set(int propId, double value) { return false; }
    double get(int propId) const { return -1.0; }
    void setExceptionMode(bool enable) noexcept { m_active_exception = enable; }
    bool getExceptionMode() const noexcept { return m_active_exception; }
    ChaosCamera &operator>>(CV_OUT cv::Mat &image)
    {
        read(image);
        return *this;
    }
    ChaosCamera &operator>>(CV_OUT cv::UMat &image)
    {
        read(image);
        return *this;
    }
};

} // namespace FairyCam

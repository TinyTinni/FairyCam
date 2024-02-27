#pragma once

#include "IsAnyCamera.hpp"
#include <functional>
#include <memory>
#include <opencv2/core.hpp>
#include <optional>
#include <vector>

namespace FairyCam
{

class AnyCamera
{
  private:
    class Base
    {
      public:
        virtual ~Base() = default;
        virtual bool open(int idx, int apiPreference,
                          const std::vector<int> &params) = 0;
        virtual bool isOpened() const = 0;
        virtual void release() = 0;
        virtual bool grab() = 0;
        virtual bool retrieve(cv::OutputArray image, int flag = 0) = 0;
        virtual bool read(cv::OutputArray image) = 0;
        virtual bool set(int propId, double value) = 0;
        virtual double get(int propId) const = 0;
        virtual void setExceptionMode(bool enable) = 0;
        virtual bool getExceptionMode() const = 0;
        virtual Base &operator>>(cv::Mat &image) = 0;
        virtual Base &operator>>(cv::UMat &image) = 0;
    };

    template <IsAnyCamera CameraType> class Model : public Base
    {
      public:
        CameraType m_camera;

        virtual ~Model(){};
        template <typename... Args>
        Model(Args... args) : m_camera{std::forward<Args...>(args...)}
        {
        }
        Model() : m_camera{} {}
        Model(CameraType &&camera) : m_camera{std::forward<CameraType>(camera)}
        {
        }

        bool open(int idx, int apiPreference,
                  const std::vector<int> &params) override final
        {
            return m_camera.open(idx, apiPreference, params);
        }
        bool isOpened() const override final { return m_camera.isOpened(); }
        void release() override final { m_camera.release(); }
        bool grab() override final { return m_camera.grab(); }
        bool retrieve(cv::OutputArray image, int flag) override final
        {
            return m_camera.retrieve(image, flag);
        }
        bool read(cv::OutputArray image) override final
        {
            return m_camera.read(image);
        }
        bool set(int propId, double value) override final
        {
            return m_camera.set(propId, value);
        }
        double get(int propId) const override final
        {
            return m_camera.get(propId);
        }
        void setExceptionMode(bool enable) override final
        {
            m_camera.setExceptionMode(enable);
        }
        bool getExceptionMode() const override final
        {
            // required, as some opencv version do not support it
            // https://github.com/opencv/opencv/pull/25062
            return const_cast<CameraType *>(std::addressof(m_camera))
                ->getExceptionMode();
        }
        Base &operator>>(cv::Mat &image) override final
        {
            m_camera >> image;
            return *this;
        }
        Base &operator>>(cv::UMat &image) override final
        {
            m_camera >> image;
            return *this;
        }
    };

    std::unique_ptr<Base> m_camera;

    template <IsAnyCamera CameraType>
    AnyCamera(std::unique_ptr<Model<CameraType>> cam) : m_camera(std::move(cam))
    {
    }

  public:
    template <IsAnyCamera CameraType>
    AnyCamera(CameraType &&camera)
        : m_camera{std::make_unique<Model<CameraType>>(
              std::forward<CameraType>(camera))}
    {
    }

    template <IsAnyCamera CameraType> static AnyCamera create()
    {
        return AnyCamera(std::make_unique<Model<CameraType>>());
    }

    template <IsAnyCamera CameraType, typename... Args>
    static AnyCamera create(Args... args)
    {
        return AnyCamera(std::make_unique<Model<CameraType>>(
            std::forward<Args...>(args...)));
    }

    template <IsAnyCamera CameraType>
    std::optional<std::reference_wrapper<CameraType>> dynamicCast() noexcept
    {
        auto modelPtr = dynamic_cast<Model<CameraType> *>(m_camera.get());
        if (!modelPtr)
            return std::nullopt;
        return std::optional<std::reference_wrapper<CameraType>>(
            modelPtr->m_camera);
    }

    template <IsAnyCamera CameraType>
    std::optional<const std::reference_wrapper<CameraType>>
    dynamicCast() const noexcept
    {
        auto modelPtr = dynamic_cast<Model<CameraType> *>(m_camera.get());
        if (!modelPtr)
            return std::nullopt;
        return std::optional<const std::reference_wrapper<CameraType>>(
            modelPtr->m_camera);
    }

    bool open(int idx, int apiPreference, const std::vector<int> &params)
    {
        return m_camera->open(idx, apiPreference, params);
    }
    bool isOpened() const { return m_camera->isOpened(); }
    void release() { m_camera->release(); }
    bool grab() { return m_camera->grab(); }
    bool retrieve(cv::OutputArray image, int flag = 0)
    {
        return m_camera->retrieve(image, flag);
    }
    bool read(cv::OutputArray image) { return m_camera->read(image); }
    bool set(int propId, double value) { return m_camera->set(propId, value); }
    double get(int propId) const { return m_camera->get(propId); }
    void setExceptionMode(bool enable) { m_camera->setExceptionMode(enable); }
    bool getExceptionMode() const { return m_camera->getExceptionMode(); }
    AnyCamera &operator>>(cv::Mat &image)
    {
        *m_camera >> image;
        return *this;
    }
    AnyCamera &operator>>(cv::UMat &image)
    {
        *m_camera >> image;
        return *this;
    }
};

} // namespace FairyCam

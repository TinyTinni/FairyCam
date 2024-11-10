#pragma once

#include <AnyCamera.hpp>
#include <algorithm>
#include <array>
#include <iostream>
#include <opencv2/core.hpp>
#include <thread>

enum CameraPosition
{
    LEFT = 0,
    RIGHT = 1,
    FRONT = 2,
    REAR = 3
};

constexpr size_t NUM_CAMERAS = 4;

class ImageAccumulator
{
    std::mutex m_lock;
    using Accumulator = std::array<cv::Mat, NUM_CAMERAS>;
    Accumulator m_images;

  public:
    void addImage(cv::Mat &&mat, CameraPosition pos)
    {
        std::lock_guard m{m_lock};
        m_images[pos] = std::move(mat);
    }

    std::array<cv::Mat, NUM_CAMERAS> reset()
    {
        Accumulator tmpImageAccumulator;
        {
            std::lock_guard m{m_lock};
            std::swap(tmpImageAccumulator, m_images);
        }
        return tmpImageAccumulator;
    }
};

class CameraActor
{
    const CameraPosition m_position;
    FairyCam::AnyCamera m_camera;
    std::shared_ptr<ImageAccumulator> m_accumulator;
    std::thread m_grabberThread;
    bool m_stop = false;

  public:
    CameraActor &operator=(const CameraActor &) = delete;
    CameraActor(const CameraActor &) = delete;

    CameraActor(CameraPosition pos, FairyCam::AnyCamera &&cam,
                std::shared_ptr<ImageAccumulator> accu)
        : m_position{pos}, m_camera{std::move(cam)},
          m_accumulator{std::move(accu)}
    {
        assert(m_accumulator != nullptr);
        m_grabberThread = std::thread(
            [this]
            {
                while (!m_stop)
                {
                    cv::Mat img;
                    if (bool retrieved = m_camera.read(img);
                        retrieved && !img.empty())
                    {
                        m_accumulator->addImage(std::move(img), m_position);
                    }
                }
            });
    }

    void stop() noexcept
    {
        if (m_stop)
            return;

        m_stop = true;
        m_camera.release();
        m_grabberThread.join();
    }

    ~CameraActor() { stop(); }
};

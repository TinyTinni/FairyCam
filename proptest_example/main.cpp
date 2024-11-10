#include "ports.hpp"
#include "system.hpp"
#include <HttpCamera.hpp>
#include <iostream>

std::array<std::unique_ptr<CameraActor>, NUM_CAMERAS>
createHttpCameras(std::shared_ptr<ImageAccumulator> accu)
{
    using namespace FairyCam;
    return std::array<std::unique_ptr<CameraActor>, NUM_CAMERAS>(
        {std::make_unique<CameraActor>(
             CameraPosition::LEFT,
             AnyCamera::create<HttpCamera>(HttpCamera::Options{
                 .port = ports::leftCamera, .grabTimeOutMs = 100}),
             accu),
         std::make_unique<CameraActor>(
             CameraPosition::RIGHT,
             AnyCamera::create<HttpCamera>(HttpCamera::Options{
                 .port = ports::rightCamera, .grabTimeOutMs = 100}),
             accu),
         std::make_unique<CameraActor>(
             CameraPosition::FRONT,
             AnyCamera::create<HttpCamera>(HttpCamera::Options{
                 .port = ports::frontCamera, .grabTimeOutMs = 100}),
             accu),
         std::make_unique<CameraActor>(
             CameraPosition::REAR,
             AnyCamera::create<HttpCamera>(HttpCamera::Options{
                 .port = ports::rearCamera, .grabTimeOutMs = 100}),
             accu)});
}

class System
{
    std::shared_ptr<ImageAccumulator> m_accu;

    using CameraArray = std::array<std::unique_ptr<CameraActor>, NUM_CAMERAS>;
    CameraArray m_cams;

  public:
    System() { start(); }

    struct RunResult
    {
        long images = 0;
    };

    void start()
    {
        m_accu = std::make_shared<ImageAccumulator>();
        m_cams = createHttpCameras(m_accu);
    }
    void stop() { std::fill(m_cams.begin(), m_cams.end(), nullptr); }
    RunResult finishRun()
    {
        if (!m_accu)
            return RunResult{};

        auto acc = m_accu->reset();
        return RunResult{.images = std::count_if(acc.begin(), acc.end(),
                                                 [](const cv::Mat &mat)
                                                 { return !mat.empty(); })};
    }
};

class TestController
{
};

int main()
{
    System system;

    system.start();

    using namespace std::chrono_literals;
    std::cout << "created" << std::endl;
    std::this_thread::sleep_for(2000ms);
    std::cout << "run: " << system.finishRun().images << std::endl;
    std::cout << "done" << std::endl;

    // cameras.clear();
    return 0;
}
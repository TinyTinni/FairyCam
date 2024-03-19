#include <cstdint>
#include <span>

#include <AnyCamera.hpp>
#include <MemoryCamera.hpp>
#include <chaos/BinaryBlobSequence.hpp>
#include <chaos/ChaosCamera.hpp>

void my_system(FairyCam::IsAnyCamera auto cam)
{
    while (true)
    {
        try
        {
            if (!cam.isOpened())
                cam.open(0, 0, {});
            cv::Mat m;
            cam.read(m);
        }
        catch (const std::exception &)
        {
            // custom error handling
        }
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FairyCam::MemoryCamera cam(
        {.images = {cv::Mat(1, 1, CV_8UC1)}, .circular = true});

    auto span = std::span(data, data + size);
    auto chaosCam = FairyCam::ChaosCamera(std::move(cam),
                                          FairyCam::BinaryBlobSequence(span));
    try
    {
        my_system(std::move(chaosCam));
    }
    catch (FairyCam::SequenceEndException)
    {
        // this exception indicated, that the program should stop
    }

    return 0;
}

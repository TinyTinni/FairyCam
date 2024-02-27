# FairyCam - OpenCV Camera Stubs

<div align="center">
  <img src="./assets/logo_256.png">
</div>
<br>

-----------------------

[![Build](https://github.com/TinyTinni/FairyCam/actions/workflows/build.yml/badge.svg)](https://github.com/TinyTinni/FairyCam/actions/workflows/build.yml)  

Camera Stubs for OpenCV's camera class [`cv::VideoCapture`](https://docs.opencv.org/4.9.0/d8/dfe/classcv_1_1VideoCapture.html).

This library provides several classes which can replace the VideoCapture class from OpenCV and provides alternative methods in providing images. You can, for example, send an image over TCP/HTTP with the `HttpCamera` class which will provide this image as a next "grabbed" image. All cameras can be replaced with another camera or the original `cv::VideoCapture` at runtime. It is similar to the `cv::CAP_IMAGES` backend, but provides more flexibility.

The intention of this library is to make automatic testing possible without relying on a hardware camera but also to be as close as the final system as possible.

## Camera Types

- `AnyCamera`: can hold any `cv::VideoCapture` interface compatible camera.

- `FileCamera`: grabs images from a provided sequence of paths. Returns an empty image and error when the sequence was reached the end, or, when the `circular` option is enabled, starts from the start of the sequence again.

- `DirectoryCamera` : grabs images from a provided directory. Returns an empty image anderror when the all images in the directory were shown once. When the `circular` option is enabled, the images in the directory are shown endlessly.

- `DirectoryTriggerCamera` : grabs images from a provided directory which were newly added to the directory. Every add/move of an image file into the directory is like the camera was triggered. It queues those images.

- `HttpCamera` : opens a HTTP server where you can connect and can images (.png/.jpg) via "POST /images" \<body containing the encoded image data\>. Queues those images. "DELETE /images" empties the queue.

## Build Requirements

- C++20
- [OpenCV](https://opencv.org/)
- [Poco](https://pocoproject.org/)

## How-To Use

FairyCam provides a new class `AnyCamera` which is an interface of `cv::VideoCapture` without relying on inheritance. It
 can take any Camera which fulfills the `cv::VideoCapture`/`FairyCam::IsAnyCamera`-Concept.

An example using dynamic polymorphism where you can change the camera at runtime:
 ```cpp
#include <FairyCam/AnyCamera.hpp>
#include <FairyCam/FileCamera.hpp>

int startSystem(FairyCam::AnyCamera cam)
{
    bool keepRunning = cam.open(0,0,{});
    while(keepRunning && cam.isOpened())
    {
        cv::Mat input;
        if (cam.read(input))
        {
            keepRunning = process(input);
        }
    }
    return 0;
}

int main()
{
    ///... load config etc.
    
    using namespace FairyCam;
    if (config::isTestingEnabled())
    {
        auto opts = FileCamera::Options{.files={"myFile1.png""differentFile.jpg"}};
        return startSystem(AnyCamera::create<FileCamera>(std::move(opts)));
    }
    return startSystem(AnyCamera::create<cv::VideoCapture>());
}



 ```

 You can also use the `IsAnyCamera` concept for compile time polymorphism. This method generates more code, but will not use any virtual calls or pointer indirections.

```cpp
#include <FairyCam/IsAnyCamera.hpp>
#include <FairyCam/HttpCamera.hpp>

// Template with concept instead of runtime polymorphism.
// It eliminates the virtual calls and indirection of AnyCamera
// and adds extra code size due to duplicating this function.
int startSystem(FairyCamera::IsAnyCamera auto cam)
{
    bool keepRunning = cam.open(0,0,{});
    while(keepRunning && cam.isOpened())
    {
        cv::Mat input;
        if (cam.read(input))
        {
            keepRunning = process(input);
        }
    }
    return 0;
}

int main()
{
    ///... load config etc.
    
    if (config::isTestingEnabled())
    {
        return startSystem(FairyCamera::HttpCamera());
    }
    return startSystem(cv::VideoCapture());
}

```


 For more examples and usage, see [in the tests directory](./tests).


## Get Original Camera Type

Similar to `dynamic_cast`, it should be possible to cast from `AnyCamera` to the original camera type.

For this, the function `AnyCamera::dynamicCast<T>` is provided. If the underlying type matches given `T`, a reference of T is returned. Otherwise, the optional is `std::nullopt`

Example: 
```cpp
using namespace FairyCam;

AnyCamera cam = AnyCamera::create<FileCamera>();
if (auto file_cam_ref = cam.dynamicCast<FileCamera>)
{
    FileCamera& file_cam = *file_cam;
    //.. do something specific
}
```

## License

MIT License © Matthias Möller. Made with ♥ in Germany.

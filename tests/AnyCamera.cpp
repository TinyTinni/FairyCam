#include <doctest.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <filesystem>

import FairyCam;

using namespace FairyCam;

TEST_SUITE("AnyCamera")
{
    TEST_CASE_TEMPLATE("create", T, cv::VideoCapture, DirectoryTriggerCamera,
                       FileCamera, DirectoryTriggerCamera, HttpCamera)
    {
        AnyCamera cam = AnyCamera::create<T>();
    }

    TEST_CASE("dynamicCast")
    {
        AnyCamera cam = AnyCamera::create<FileCamera>();
        REQUIRE_FALSE(cam.dynamicCast<DirectoryCamera>());
        auto fileCam = cam.dynamicCast<FileCamera>();
        REQUIRE(fileCam);
        FileCamera &cam_ref = *fileCam;
        REQUIRE_FALSE(cam.isOpened());
        REQUIRE_FALSE(cam_ref.isOpened());
        cam_ref.open(0, 0, {});
        REQUIRE(cam.isOpened());
        REQUIRE(cam_ref.isOpened());
    }
}

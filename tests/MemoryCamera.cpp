#include <doctest.h>
#include <opencv2/core.hpp>

import FairyCam;

using namespace FairyCam;

TEST_SUITE("MemoryCamera")
{
    TEST_CASE("load image")
    {
        for (const bool circular : std::vector{false, true})
        {
            CAPTURE(circular);
            AnyCamera cam(MemoryCamera(
                {.images = {cv::Mat(1, 1, CV_8UC1)}, .circular = circular}));

            REQUIRE(cam.open(0, 0, {}));

            SUBCASE("close and reopen")
            {
                REQUIRE(cam.isOpened());
                cam.release();
                REQUIRE_FALSE(cam.isOpened());
                cv::Mat m;
                REQUIRE_FALSE(cam.read(m));
                REQUIRE(m.empty());
                cam.open(0, 0, {});
            }

            CHECK(cam.isOpened());
            cv::Mat m;
            REQUIRE(cam.read(m));
            CHECK(!m.empty());
            CHECK(cam.read(m) == circular);
            CHECK(m.empty() != circular);
        }
    }

    TEST_CASE("load not existing image")
    {
        AnyCamera cam(MemoryCamera({.images = std::vector{cv::Mat()}}));

        REQUIRE(cam.open(0, 0, {}));
        CHECK(cam.isOpened());
        cv::Mat m;
        REQUIRE_FALSE(cam.read(m));
        CHECK(m.empty());
    }

    TEST_CASE("load multiple images")
    {
        for (const bool circular : std::vector{false, true})
        {
            CAPTURE(circular);
            AnyCamera cam(MemoryCamera(
                {.images = {cv::Mat(1, 1, CV_8UC1), cv::Mat(1, 1, CV_8UC1),
                            cv::Mat(), cv::Mat(1, 1, CV_8UC1)},
                 .circular = circular}));

            REQUIRE(cam.open(0, 0, {}));
            CHECK(cam.isOpened());
            cv::Mat m;
            REQUIRE(cam.read(m));
            CHECK(!m.empty());
            REQUIRE(cam.read(m));
            CHECK(!m.empty());

            REQUIRE(!cam.read(m));
            CHECK(m.empty());

            REQUIRE(cam.read(m));
            CHECK(!m.empty());

            CHECK(cam.read(m) == circular);
            CHECK(m.empty() != circular);
        }
    }
}

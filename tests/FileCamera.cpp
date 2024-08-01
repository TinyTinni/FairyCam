#include <doctest.h>
#include <opencv2/core.hpp>

#include <vector>

import FairyCam;

using namespace FairyCam;

TEST_SUITE("FileCamera")
{
    TEST_CASE("load image")
    {
        for (const bool circular : std::vector{false, true})
        {
            CAPTURE(circular);
            AnyCamera cam(FileCamera(
                {.files = {TEST_DATA "test.png"}, .circular = circular}));

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
        for (auto images : std::vector{
                 std::vector<std::string>(), std::vector<std::string>{""},
                 std::vector<std::string>{TEST_DATA "doesnotexists.png"}})
        {
            CAPTURE(images);
            AnyCamera cam(FileCamera({.files = std::move(images)}));

            REQUIRE(cam.open(0, 0, {}));
            CHECK(cam.isOpened());
            cv::Mat m;
            REQUIRE_FALSE(cam.read(m));
            CHECK(m.empty());
        }
    }

    TEST_CASE("load multiple images")
    {
        for (const bool circular : std::vector{false, true})
        {
            CAPTURE(circular);
            AnyCamera cam(FileCamera(
                {.files = {TEST_DATA "test.png", TEST_DATA "test.png",
                           TEST_DATA "", TEST_DATA "test.png"},
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

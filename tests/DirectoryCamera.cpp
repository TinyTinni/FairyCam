#include <doctest.h>
#include <filesystem>

#include <opencv2/core.hpp>

import FairyCam;

using namespace FairyCam;

TEST_SUITE("DirectoryCamera")
{
    TEST_CASE("load image")
    {
        for (const bool circular : std::vector{false, true})
        {
            CAPTURE(circular);
            AnyCamera cam(
                DirectoryCamera({.dir = TEST_DATA, .circular = circular}));

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

    TEST_CASE("load not existing directory")
    {
        REQUIRE(std::filesystem::is_regular_file(TEST_DATA "/test.png"));
        for (const auto &dir : std::vector<std::string>{
                 "", TEST_DATA "/doesnotexists/", TEST_DATA "/test.png"})
        {
            CAPTURE(dir);
            AnyCamera cam(DirectoryCamera({.dir = dir}));

            REQUIRE_FALSE(cam.open(0, 0, {}));
            CHECK_FALSE(cam.isOpened());
            cv::Mat m;
            REQUIRE_FALSE(cam.read(m));
            CHECK(m.empty());
        }
    }
}

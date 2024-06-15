#include <doctest.h>

#include <filesystem>
#include <format>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

import FairyCam;

using namespace FairyCam;

TEST_SUITE("DirectoryTriggerCamera")
{
    TEST_CASE("invalid directory")
    {
        AnyCamera cam(AnyCamera::create<DirectoryTriggerCamera>({.dir = ""}));

        CHECK_FALSE(cam.open(0, 0, {}));
        CHECK_FALSE(cam.isOpened());

        cv::Mat mat;
        CHECK_FALSE(cam.read(mat));
        CHECK(mat.empty());
    }

    TEST_CASE("existing directory without new images")
    {
        AnyCamera cam(AnyCamera::create<DirectoryTriggerCamera>(
            {.dir = TEST_DATA, .grabTimeOutMs = 0}));

        CHECK(cam.open(0, 0, {}));
        CHECK(cam.isOpened());

        cv::Mat mat;
        CHECK_FALSE(cam.read(mat));
        CHECK(mat.empty());
    }

    TEST_CASE("existing directory with new images")
    {
        struct Remover
        {
            std::filesystem::path dir;
            ~Remover()
            {
                std::error_code ec;
                std::filesystem::remove_all(dir, ec);
            }
        };
        for (const auto n_images : std::vector{1, 10})
        {
            CAPTURE(n_images);
            auto new_dir = std::filesystem::temp_directory_path() /
                           "DirectoryTriggerCameraTest";

            std::filesystem::remove_all(new_dir);
            Remover cleanup{.dir = new_dir};
            REQUIRE(std::filesystem::create_directories(new_dir));

            AnyCamera cam(AnyCamera::create<DirectoryTriggerCamera>(
                {.dir = new_dir, .grabTimeOutMs = 50}));

            CHECK(cam.open(0, 0, {}));
            CHECK(cam.isOpened());

            cv::Mat mat;
            CHECK_FALSE(cam.read(mat));
            CHECK(mat.empty());

            for (auto i = 0; i < n_images; ++i)
            {
                cv::imwrite((new_dir / std::format("output{}.png", i)).string(),
                            cv::Mat(16, 16, CV_8UC3));
                CHECK(cam.read(mat));
                CHECK(!mat.empty());
            }

            CHECK_FALSE(cam.read(mat));
            CHECK(mat.empty());
        }
    }
}

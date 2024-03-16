#include <array>
#include <concepts>
#include <vector>

#include <AnyCamera.hpp>
#include <MemoryCamera.hpp>
#include <chaos/ChaosCamera.hpp>
#include <chaos/BernoulliSequence.hpp>
#include <doctest.h>

using namespace FairyCam;

TEST_SUITE("ChaosCamera")
{
    TEST_CASE("enabled exceptions")
    {
        // for (auto [failIsOpen, failGrab, failRetrieve] :
        // std::views::cartesian_product(
        //          std::array{0,1}, std::array{0,1}, std::array{0,1}))
        for (auto [failIsOpen, failGrab, failRetrieve] : std::vector{
                 std::array{0, 0, 0}, std::array{0, 0, 1}, std::array{0, 1, 0},
                 std::array{0, 1, 1}, std::array{1, 0, 0}, std::array{1, 0, 1},
                 std::array{1, 1, 0}, std::array{1, 1, 1}})
        {
            CAPTURE(failIsOpen);
            CAPTURE(failGrab);
            CAPTURE(failRetrieve);

            AnyCamera cam =
                ChaosCamera(MemoryCamera({.images = {cv::Mat(1, 1, CV_8UC1)},
                                          .circular = true}),
                            BernoulliSequence({.isOpen = failIsOpen,
                                               .grab = failGrab,
                                               .retrieve = failRetrieve}));
            for (size_t i = 0; i < 10; ++i)
            {
                if (failIsOpen)
                    REQUIRE_THROWS_AS(cam.open(0, 0, {}), NotOpenException);
                else
                    REQUIRE(cam.open(0, 0, {}));

                if (failIsOpen)
                    REQUIRE_THROWS_AS(cam.grab(), NotOpenException);
                else if (failGrab)
                    REQUIRE_THROWS_AS(cam.grab(), GrabException);
                else
                    REQUIRE(cam.grab());

                cv::Mat mat;
                if (failIsOpen)
                    REQUIRE_THROWS_AS(cam.retrieve(mat), NotOpenException);
                else if (failRetrieve || failGrab)
                    REQUIRE_THROWS_AS(cam.retrieve(mat), RetrieveException);
                else
                    REQUIRE(cam.retrieve(mat));
            }
        }
    }

    TEST_CASE("disabled exceptions")
    {
        // for (auto [failIsOpen, failGrab, failRetrieve] :
        // std::views::cartesian_product(
        //          std::array{0,1}, std::array{0,1}, std::array{0,1}))
        for (auto [failIsOpen, failGrab, failRetrieve] : std::vector{
                 std::array{0, 0, 0}, std::array{0, 0, 1}, std::array{0, 1, 0},
                 std::array{0, 1, 1}, std::array{1, 0, 0}, std::array{1, 0, 1},
                 std::array{1, 1, 0}, std::array{1, 1, 1}})
        {
            CAPTURE(failIsOpen);
            CAPTURE(failGrab);
            CAPTURE(failRetrieve);

            AnyCamera cam =
                ChaosCamera(MemoryCamera({.images = {cv::Mat(1, 1, CV_8UC1)},
                                          .circular = true}),
                            BernoulliSequence({.isOpen = failIsOpen,
                                               .grab = failGrab,
                                               .retrieve = failRetrieve}));

            cam.setExceptionMode(false);
            for (size_t i = 0; i < 10; ++i)
            {
                if (failIsOpen)
                    REQUIRE_FALSE(cam.open(0, 0, {}));
                else
                    REQUIRE(cam.open(0, 0, {}));

                if (failIsOpen)
                    REQUIRE_FALSE(cam.grab());
                else if (failGrab)
                    REQUIRE_FALSE(cam.grab());
                else
                    REQUIRE(cam.grab());

                cv::Mat mat;
                if (failIsOpen)
                    REQUIRE_FALSE(cam.retrieve(mat));
                else if (failRetrieve || failGrab)
                    REQUIRE_FALSE(cam.retrieve(mat));
                else
                    REQUIRE(cam.retrieve(mat));
            }
        }
    }
}

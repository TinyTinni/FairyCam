#include <array>
#include <concepts>
#include <doctest.h>
#include <span>
#include <vector>

#include <chaos/BernoulliSequence.hpp>
#include <chaos/BinaryBlobSequence.hpp>

using namespace FairyCam;

TEST_SUITE("BinaryBlobSequence")
{
    TEST_CASE("always or never")
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

            std::array blob = {static_cast<uint8_t>(failIsOpen * 255),
                               static_cast<uint8_t>(failGrab * 255),
                               static_cast<uint8_t>(failRetrieve * 255)};

            BinaryBlobSequence s(std::span(blob.data(), blob.size()));

            if (failIsOpen)
                REQUIRE_THROWS_AS(s.checkIsOpen(), NotOpenException);
            else
                s.checkIsOpen();

            if (failGrab)
                REQUIRE_THROWS_AS(s.checkGrab(), GrabException);
            else
                s.checkGrab();

            if (failRetrieve)
                REQUIRE_THROWS_AS(s.checkRetrieve(), RetrieveException);
            else
                s.checkRetrieve();

            REQUIRE_THROWS_AS(s.checkRetrieve(), SequenceEndException);
            REQUIRE_THROWS_AS(s.checkGrab(), SequenceEndException);
            REQUIRE_THROWS_AS(s.checkIsOpen(), SequenceEndException);
        }
    }

    TEST_CASE("custom exceptions")
    {
        struct CustomException1
        {
        };

        struct CustomException2
        {
        };

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

            std::array blob = {static_cast<uint8_t>(failIsOpen * 255),
                               static_cast<uint8_t>(failGrab * 255),
                               static_cast<uint8_t>(failRetrieve * 254),
                               static_cast<uint8_t>(failRetrieve * 255)};

            BinaryBlobSequence s(
                std::span(blob.data(), blob.size()),
                {.isOpen = BinaryBlobSequence::Fail().with<CustomException1>(),
                 .grab = BinaryBlobSequence::Fail().with<CustomException2>(),
                 .retrieve = BinaryBlobSequence::Fail()
                                 .with<CustomException1>()
                                 .with<CustomException2>()});

            if (failIsOpen)
                REQUIRE_THROWS_AS(s.checkIsOpen(), CustomException1);
            else
                s.checkIsOpen();

            if (failGrab)
                REQUIRE_THROWS_AS(s.checkGrab(), CustomException2);
            else
                s.checkGrab();

            if (failRetrieve)
            {
                REQUIRE_THROWS_AS(s.checkRetrieve(), CustomException1);
                REQUIRE_THROWS_AS(s.checkRetrieve(), CustomException2);
            }
            else
            {
                s.checkRetrieve();
                s.checkRetrieve();
            }

            REQUIRE_THROWS_AS(s.checkRetrieve(), SequenceEndException);
            REQUIRE_THROWS_AS(s.checkGrab(), SequenceEndException);
            REQUIRE_THROWS_AS(s.checkIsOpen(), SequenceEndException);
        }
    }
}

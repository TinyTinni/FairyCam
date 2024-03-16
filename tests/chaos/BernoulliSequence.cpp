#include <array>
#include <concepts>
#include <doctest.h>
#include <vector>

#include <chaos/BernoulliSequence.hpp>

using namespace FairyCam;

TEST_SUITE("RandomSequence")
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

            BernoulliSequence s({.isOpen = failIsOpen,
                                 .grab = failGrab,
                                 .retrieve = failRetrieve});
            for (size_t i = 0; i < 100; ++i)
            {
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
            }
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

        constexpr size_t runs = 1000;
        for (auto [failIsOpen, failGrab, failRetrieve] : std::vector{
                 std::array{0, 0, 0}, std::array{0, 0, 1}, std::array{0, 1, 0},
                 std::array{0, 1, 1}, std::array{1, 0, 0}, std::array{1, 0, 1},
                 std::array{1, 1, 0}, std::array{1, 1, 1}})
        {
            CAPTURE(failIsOpen);
            CAPTURE(failGrab);
            CAPTURE(failRetrieve);

            BernoulliSequence s(
                {.isOpen = BernoulliSequence::Fail(failIsOpen)
                               .with<CustomException1>()
                               .with<CustomException2>(),
                 .grab = BernoulliSequence::Fail(failGrab)
                             .with<CustomException1>(0.0)
                             .with<CustomException2>(1.0),
                 .retrieve = BernoulliSequence::Fail(failRetrieve)
                                 .with<CustomException1>(1.0)
                                 .with<CustomException2>(0.0)});

            uint32_t threwCustom1 = 0;
            uint32_t threwCustom2 = 0;
            for (size_t i = 0; i < runs; ++i)
            {
                if (failIsOpen)
                {
                    try
                    {
                        s.checkIsOpen();
                        FAIL("checkIsOpen did not throw any exception.");
                    }
                    catch (const CustomException1 &)
                    {
                        threwCustom1 += 1;
                    }
                    catch (const CustomException2 &)
                    {
                        threwCustom2 += 1;
                    }
                    catch (...)
                    {
                        FAIL("checkIsOpen did throw the wrong exception.");
                    }
                }

                if (failGrab)
                    REQUIRE_THROWS_AS(s.checkGrab(), CustomException2);
                else
                    s.checkGrab();

                if (failRetrieve)
                    REQUIRE_THROWS_AS(s.checkRetrieve(), CustomException1);
                else
                    s.checkRetrieve();
            }

            if (failIsOpen)
            {
                REQUIRE_EQ(threwCustom1 + threwCustom2, runs);
                REQUIRE_GE(threwCustom1, runs * 0.35);
                REQUIRE_GE(threwCustom2, runs * 0.35);
            }
        }
    }
}

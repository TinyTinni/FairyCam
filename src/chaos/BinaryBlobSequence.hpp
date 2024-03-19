#pragma once

#include <cassert>
#include <concepts>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

#include "Exceptions.hpp"
#include "Sequence.hpp"

namespace FairyCam
{

class BinaryBlobSequence : public Sequence
{
    std::span<const uint8_t> m_data;
    std::span<const uint8_t>::iterator m_iter;

    struct FailException
    {
        std::vector<std::function<void()>> exceptions;
    };

    FailException m_open;
    FailException m_grab;
    FailException m_retrieve;

    void check(const FailException &exp)
    {
        if (m_iter == m_data.end())
            throw SequenceEndException{};
        uint8_t val = *m_iter;
        ++m_iter;
        if (val == 0)
            return;
        std::invoke(exp.exceptions[val % exp.exceptions.size()]);
    }

  public:
    class Fail
    {
        std::vector<std::function<void()>> exceptions;
        friend BinaryBlobSequence;

      public:
        template <std::default_initializable ExceptionT> Fail &with()
        {
            exceptions.push_back([]() { throw ExceptionT{}; });
            return *this;
        }
    };

    struct Options
    {
        Fail isOpen = Fail().with<NotOpenException>();
        Fail grab = Fail().with<GrabException>();
        Fail retrieve = Fail().with<RetrieveException>();
    };

    BinaryBlobSequence(std::span<const uint8_t> data, Options opts)
        : m_data(data), m_iter(m_data.begin()),
          m_open{std::move(opts.isOpen.exceptions)},
          m_grab{std::move(opts.grab.exceptions)},
          m_retrieve{std::move(opts.retrieve.exceptions)}
    {
    }

    BinaryBlobSequence(std::span<const uint8_t> data)
        : BinaryBlobSequence(data, Options())
    {
    }

    void checkIsOpen() override { check(m_open); }
    void checkRetrieve() override { check(m_retrieve); }
    void checkGrab() override { check(m_grab); }
};

} // namespace FairyCam

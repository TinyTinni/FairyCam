#pragma once

#include <cassert>
#include <concepts>
#include <functional>
#include <random>
#include <thread>
#include <vector>

#include "Exceptions.hpp"
#include "Sequence.hpp"

namespace FairyCam
{

class BernoulliSequence : public Sequence
{
    struct Probas;

  public:
    class Fail
    {
        double probability;
        std::vector<std::function<void()>> exceptions;
        std::vector<double> weights;
        friend Probas;

      public:
        Fail() : probability{0.f} {}
        Fail(int proba) : probability{static_cast<double>(proba)} {}
        Fail(double proba) : probability{proba}
        {
            assert(proba >= 0 && proba <= 0);
        }

        template <std::default_initializable ExceptionT>
        Fail &with(double weight = 1.0)
        {
            return with([]() { throw ExceptionT{}; }, weight);
        }
        template <std::invocable<> Func> Fail &with(Func f, double weight = 1.0)
        {
            if (!f)
                return *this;
            exceptions.push_back(std::move(f));
            weights.push_back(weight);
            return *this;
        }
    };

    struct Options
    {
        Fail isOpen = Fail{};
        Fail grab = Fail{};
        Fail retrieve = Fail{};
    };

    BernoulliSequence(Options opts)
        : m_gen{std::random_device{}()}, m_is_open{std::move(opts.isOpen)},
          m_retrieve{std::move(opts.retrieve)}, m_grab{std::move(opts.grab)}
    {
    }

    BernoulliSequence(int seed, Options opts)
        : m_is_open{std::move(opts.isOpen)},
          m_retrieve{std::move(opts.retrieve)}, m_grab{std::move(opts.grab)}
    {
        m_gen.seed(seed);
    }

    void checkIsOpen() override { checkProba<NotOpenException>(m_is_open); }
    void checkRetrieve() override { checkProba<RetrieveException>(m_retrieve); }
    void checkGrab() override { checkProba<GrabException>(m_grab); }

  private:
    struct Probas
    {
        std::binomial_distribution<> fail;
        std::discrete_distribution<> exception_distribution;
        std::vector<std::function<void()>> exceptions;
        Probas(Fail &&f)
            : fail{std::binomial_distribution<>{1, f.probability}},
              exception_distribution{std::begin(f.weights),
                                     std::end(f.weights)},
              exceptions{std::move(f.exceptions)}
        {
            assert(exceptions.size() == f.weights.size());
        }
    };

    Probas m_is_open;
    Probas m_retrieve;
    Probas m_grab;
    std::mt19937 m_gen;

    template <std::default_initializable ExceptionT> void checkProba(Probas &p)
    {
        if (!p.fail(m_gen))
            return;

        if (p.exceptions.empty())
            throw ExceptionT{};

        auto idx = p.exception_distribution(m_gen);
        std::invoke(p.exceptions[idx]);
    }
};

} // namespace FairyCam

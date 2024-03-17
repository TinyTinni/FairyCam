#pragma once

#include "Sequence.hpp"

namespace FairyCam
{

class NoErrorSequence : public Sequence
{
    virtual void checkIsOpen(){};
    virtual void checkRetrieve(){};
    virtual void checkGrab(){};
};
} // namespace FairyCam

#pragma once

namespace FairyCam
{
class Sequence
{
  public:
    virtual void checkIsOpen() = 0;
    virtual void checkRetrieve() = 0;
    virtual void checkGrab() = 0;
    virtual ~Sequence();
};

// workaround for GCC14, othewise, it segfaults
inline Sequence::~Sequence() = default;
} // namespace FairyCam

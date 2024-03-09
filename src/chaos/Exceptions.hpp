#pragma once

#include <stdexcept>

namespace FairyCam
{

class NotOpenException : public std::runtime_error
{
  public:
    NotOpenException() : std::runtime_error{"FairyCam closed the camera."} {}
};

class RetrieveException : public std::runtime_error
{
  public:
    RetrieveException() : std::runtime_error{"FairyCam error on retrieve."} {}
};

class GrabException : public std::runtime_error
{
  public:
    GrabException() : std::runtime_error{"FairyCam error on grab."} {}
};

} // namespace FairyCam

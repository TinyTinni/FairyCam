module;

#include "AnyCamera.hpp"
#include "DirectoryCamera.hpp"
#include "DirectoryTriggerCamera.hpp"
#include "FileCamera.hpp"
#include "HttpCamera.hpp"
#include "IsAnyCamera.hpp"
#include "MemoryCamera.hpp"
#include "chaos/ChaosCamera.hpp"

#include "chaos/Exceptions.hpp"
#include "chaos/Sequence.hpp"

#include "chaos/BernoulliSequence.hpp"
#include "chaos/BinaryBlobSequence.hpp"
#include "chaos/NoErrorSequence.hpp"

export module FairyCam;

export namespace FairyCam
{
using FairyCam::AnyCamera;
using FairyCam::ChaosCamera;
using FairyCam::DirectoryCamera;
using FairyCam::DirectoryTriggerCamera;
using FairyCam::FileCamera;
using FairyCam::HttpCamera;
using FairyCam::IsAnyCamera;
using FairyCam::MemoryCamera;

using FairyCam::BernoulliSequence;
using FairyCam::BinaryBlobSequence;
using FairyCam::NoErrorSequence;
using FairyCam::Sequence;

using FairyCam::GrabException;
using FairyCam::NotOpenException;
using FairyCam::RetrieveException;
using FairyCam::SequenceEndException;

} // namespace FairyCam

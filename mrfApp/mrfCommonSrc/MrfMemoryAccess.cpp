/*
 * Copyright 2015-2016 aquenos GmbH.
 * Copyright 2015-2016 Karlsruhe Institute of Technology.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * This software has been developed by aquenos GmbH on behalf of the
 * Karlsruhe Institute of Technology's Institute for Beam Physics and
 * Technology.
 *
 * This software contains code originally developed by aquenos GmbH for
 * the s7nodave EPICS device support. aquenos GmbH has relicensed the
 * affected poritions of code from the s7nodave EPICS device support
 * (originally licensed under the terms of the GNU GPL) under the terms
 * of the GNU LGPL version 3 or newer.
 */

#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <stdexcept>

#include "MrfMemoryAccess.h"

namespace anka {
namespace mrf {

MrfMemoryAccess::MrfMemoryAccess() {
}

MrfMemoryAccess::~MrfMemoryAccess() {
}

// We use an anonymous namespace to avoid any name collisions.
namespace {

template<typename T>
class CallbackImpl: public MrfMemoryAccess::Callback<T> {
private:
  std::mutex mutex;
  std::condition_variable cv;
  bool finished = false;
  T value;
  bool successful = false;
  std::uint32_t address;
  MrfMemoryAccess::ErrorCode errorCode;
  std::string details;

public:
  CallbackImpl() :
      address(0), errorCode(MrfMemoryAccess::ErrorCode::unknown) {
  }

  void success(std::uint32_t, T value) {
    std::unique_lock<std::mutex> lock(mutex);
    this->finished = true;
    this->value = value;
    this->successful = true;
    cv.notify_all();
  }

  void failure(std::uint32_t address, MrfMemoryAccess::ErrorCode errorCode,
      const std::string &details) {
    std::unique_lock<std::mutex> lock(mutex);
    this->finished = true;
    this->successful = false;
    this->address = address;
    this->errorCode = errorCode;
    this->details = details;
    cv.notify_all();
  }

  T getResult() {
    std::unique_lock<std::mutex> lock(mutex);
    while (!finished) {
      cv.wait(lock);
    }
    if (!successful) {
      throw std::runtime_error(
          std::string("Memory access operation for address ")
              + mrfMemoryAddressToString(address) + " failed: "
              + (details.empty() ? mrfErrorCodeToString(errorCode) : details));
    }
    return value;
  }

};

}

std::uint16_t MrfMemoryAccess::readUInt16(std::uint32_t address) {
  auto callback = std::make_shared<CallbackImpl<std::uint16_t>>();
  this->readUInt16(address, callback);
  return callback->getResult();
}

std::uint16_t MrfMemoryAccess::writeUInt16(std::uint32_t address,
    std::uint16_t value) {
  auto callback = std::make_shared<CallbackImpl<std::uint16_t>>();
  this->writeUInt16(address, value, callback);
  return callback->getResult();
}

std::uint32_t MrfMemoryAccess::readUInt32(std::uint32_t address) {
  auto callback = std::make_shared<CallbackImpl<std::uint32_t>>();
  this->readUInt32(address, callback);
  return callback->getResult();
}

std::uint32_t MrfMemoryAccess::writeUInt32(std::uint32_t address,
    std::uint32_t value) {
  auto callback = std::make_shared<CallbackImpl<std::uint32_t>>();
  this->writeUInt32(address, value, callback);
  return callback->getResult();
}

bool MrfMemoryAccess::supportsInterrupts() const {
  return false;
}

void MrfMemoryAccess::addInterruptListener(std::shared_ptr<InterruptListener>) {
  throw std::runtime_error("This memory access does not support interrupts.");
}

void MrfMemoryAccess::removeInterruptListener(
    std::shared_ptr<InterruptListener>) {
  throw std::runtime_error("This memory access does not support interrupts.");
}

std::string mrfMemoryAddressToString(std::uint32_t address) {
  char buffer[11];
  if (std::snprintf(buffer, 11, "0x%08x", address) < 0) {
    throw std::runtime_error(
        "Unexpected error while trying to convert an address to a string.");
  }
  return std::string(buffer);
}

std::string mrfErrorCodeToString(MrfMemoryAccess::ErrorCode errorCode) {
  switch (errorCode) {
  case MrfMemoryAccess::ErrorCode::unknown:
    return "Unknown error";
  case MrfMemoryAccess::ErrorCode::invalidAddress:
    return "Invalid address";
  case MrfMemoryAccess::ErrorCode::fpgaTimeout:
    return "FPGA timeout";
  case MrfMemoryAccess::ErrorCode::networkTimeout:
    return "Network timeout";
  case MrfMemoryAccess::ErrorCode::invalidCommand:
    return "Invalid command";
  default:
    return "Unidentified error type";
  }
}

}
}

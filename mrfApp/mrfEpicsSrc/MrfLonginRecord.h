/*
 * Copyright 2015 aquenos GmbH.
 * Copyright 2015 Karlsruhe Institute of Technology.
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

#ifndef ANKA_MRF_EPICS_LONGIN_RECORD_H
#define ANKA_MRF_EPICS_LONGIN_RECORD_H

#include <longinRecord.h>

#include "MrfInputRecord.h"

namespace anka {
namespace mrf {
namespace epics {

/**
 * Device support class for the longin record.
 */
class MrfLonginRecord: public MrfInputRecord<::longinRecord> {

public:

  /**
   * Creates an instance of the device support for the specified record.
   */
  MrfLonginRecord(::longinRecord *record);

protected:

  void writeRecordValue(std::uint32_t value);

private:

  // We do not want to allow copy or move construction or assignment.
  MrfLonginRecord(const MrfLonginRecord &) = delete;
  MrfLonginRecord(MrfLonginRecord &&) = delete;
  MrfLonginRecord &operator=(const MrfLonginRecord &) = delete;
  MrfLonginRecord &operator=(MrfLonginRecord &&) = delete;

};

}
}
}

#endif // ANKA_MRF_EPICS_LONGIN_RECORD_H

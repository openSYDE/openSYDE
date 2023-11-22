//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Grey out information for message
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREELOGGERDATAGREYOUTINFORMATION_HPP
#define C_CAMMETTREELOGGERDATAGREYOUTINFORMATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>
#include <vector>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeLoggerDataGreyOutInformation
{
public:
   C_CamMetTreeLoggerDataGreyOutInformation(void);

   int32_t s32_GrayOutValueMsg;
   uint32_t u32_DataChangedTimeStamp; ///< Time stamp of last
   // changed
   // data bytes of CAN
   // message
   std::vector<int32_t> c_GrayOutValueDataBytes; ///< Gray out values for
   // each
   // data
   // byte of CAN message
   std::map<int32_t, int32_t> c_MapMultiplexerValueToGrayOutValue; ///< Gray out values map for
   // each
   // multiplexer value of a
   // CAN
   // message
   std::vector<uint32_t> c_DataBytesChangedTimeStamps; ///< Time stamp of last
   // change for
   // each data byte of CAN
   // message
   std::map<int32_t, uint32_t> c_MapMultiplexerValueToChangedTimeStamps; ///< Time stamp of last
   // change for each
   // multiplexer value of a
   // CAN message
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

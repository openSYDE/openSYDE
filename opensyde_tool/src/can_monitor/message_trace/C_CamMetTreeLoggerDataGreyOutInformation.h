//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Grey out information for message
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREELOGGERDATAGREYOUTINFORMATION_H
#define C_CAMMETTREELOGGERDATAGREYOUTINFORMATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>
#include <vector>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeLoggerDataGreyOutInformation
{
public:
   C_CamMetTreeLoggerDataGreyOutInformation(void);

   stw_types::sintn sn_GrayOutValueMsg;
   stw_types::uint32 u32_DataChangedTimeStamp;                                              ///< Time stamp of last
                                                                                            // changed
                                                                                            // data bytes of CAN
                                                                                            // message
   std::vector<stw_types::sintn> c_GrayOutValueDataBytes;                                   ///< Gray out values for
                                                                                            // each
                                                                                            // data
                                                                                            // byte of CAN message
   std::map<stw_types::sint32, stw_types::sintn> c_MapMultiplexerValueToGrayOutValue;       ///< Gray out values map for
                                                                                            // each
                                                                                            // multiplexer value of a
                                                                                            // CAN
                                                                                            // message
   std::vector<stw_types::uint32> c_DataBytesChangedTimeStamps;                             ///< Time stamp of last
                                                                                            // change for
                                                                                            // each data byte of CAN
                                                                                            // message
   std::map<stw_types::sint32, stw_types::uint32> c_MapMultiplexerValueToChangedTimeStamps; ///< Time stamp of last
                                                                                            // change for each
                                                                                            // multiplexer value of a
                                                                                            // CAN message
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

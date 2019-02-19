//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Adaption of data class for logging CAN messages for message trace tree view (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMETTREELOGGERDATA_H
#define C_CAMMETTREELOGGERDATA_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>

#include "stwtypes.h"

#include "C_OSCComMessageLoggerData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMetTreeLoggerData :
   public stw_opensyde_core::C_OSCComMessageLoggerData
{
public:
   C_CamMetTreeLoggerData(void);

   // For usage of concrete data model or widget. Will not be set by C_SyvComMessageMonitor
   stw_types::sintn sn_GrayOutValueMsg;
   stw_types::sintn sn_GrayOutValueData;
   stw_types::uint32 u32_DataChangedTimeStamp;                  ///< Time stamp of last changed data bytes of CAN
                                                                // message
   std::vector<stw_types::sintn> c_GrayOutValueDataBytes;       ///< Gray out values for each data byte of CAN message
   std::vector<stw_types::uint32> c_DataBytesChangedTimeStamps; ///< Time stamp of last change for each data byte of CAN
                                                                // message
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

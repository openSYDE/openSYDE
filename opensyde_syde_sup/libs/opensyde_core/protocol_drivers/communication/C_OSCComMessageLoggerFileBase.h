//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with generic interface to handle and write a CAN log file (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGERFILEBASE_H
#define C_OSCCOMMESSAGELOGGERFILEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCComMessageLoggerData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCComMessageLoggerFileBase
{
public:
   C_OSCComMessageLoggerFileBase(const stw_scl::C_SCLString & orc_FilePath,
                                 const stw_scl::C_SCLString & orc_ProtocolName);
   virtual ~C_OSCComMessageLoggerFileBase(void);

   virtual stw_types::sint32 OpenFile(void);
   virtual void AddMessageToFile(const C_OSCComMessageLoggerData & orc_MessageData) = 0;

   void SetProtocolName(const stw_scl::C_SCLString & orc_ProtocolName);

protected:
   stw_scl::C_SCLString mc_FilePath;
   stw_scl::C_SCLString mc_ProtocolName;

private:
   //Avoid call
   C_OSCComMessageLoggerFileBase(const C_OSCComMessageLoggerFileBase &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

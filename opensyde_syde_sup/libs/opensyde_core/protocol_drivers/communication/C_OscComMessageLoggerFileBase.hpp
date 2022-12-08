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
#include "stwtypes.hpp"
#include "C_OscComMessageLoggerData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComMessageLoggerFileBase
{
public:
   C_OscComMessageLoggerFileBase(const stw::scl::C_SclString & orc_FilePath,
                                 const stw::scl::C_SclString & orc_ProtocolName);
   virtual ~C_OscComMessageLoggerFileBase(void);

   virtual int32_t OpenFile(void);
   virtual void AddMessageToFile(const C_OscComMessageLoggerData & orc_MessageData) = 0;

   void SetProtocolName(const stw::scl::C_SclString & orc_ProtocolName);

protected:
   stw::scl::C_SclString mc_FilePath;
   stw::scl::C_SclString mc_ProtocolName;

private:
   //Avoid call
   C_OscComMessageLoggerFileBase(const C_OscComMessageLoggerFileBase &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

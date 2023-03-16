//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with generic interface to handle and write a CAN log file (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGERFILEASC_H
#define C_OSCCOMMESSAGELOGGERFILEASC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <fstream>

#include "C_SclString.hpp"

#include "C_OscComMessageLoggerFileBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComMessageLoggerFileAsc :
   public C_OscComMessageLoggerFileBase
{
public:
   C_OscComMessageLoggerFileAsc(const stw::scl::C_SclString & orc_FilePath,
                                const stw::scl::C_SclString & orc_ProtocolName, const bool oq_HexActive,
                                const bool oq_RelativeTimeStampActive);
   virtual ~C_OscComMessageLoggerFileAsc(void);

   virtual int32_t OpenFile(void);
   virtual void AddMessageToFile(const C_OscComMessageLoggerData & orc_MessageData);

private:
   //Avoid call
   C_OscComMessageLoggerFileAsc(const C_OscComMessageLoggerFileAsc &);
   C_OscComMessageLoggerFileAsc & operator =(const C_OscComMessageLoggerFileAsc &);

   void m_WriteHeader(void);
   static stw::scl::C_SclString mh_GetAscTimeString(void);
   static stw::scl::C_SclString mh_GetDay(const int32_t os32_Day);
   static stw::scl::C_SclString mh_GetMonth(const int32_t os32_Month);
   static stw::scl::C_SclString mh_AdaptTimeStamp(const uint64_t ou64_TimeStamp);

   const bool mq_HexActive;
   const bool mq_RelativeTimeStampActive;
   std::ofstream mc_File;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

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

/* -- Includes ------------------------------------------------------------- */
#include <fstream>

#include "CSCLString.h"

#include "C_OSCComMessageLoggerFileBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComMessageLoggerFileAsc :
   public C_OSCComMessageLoggerFileBase
{
public:
   C_OSCComMessageLoggerFileAsc(const stw_scl::C_SCLString & orc_FilePath,
                                const stw_scl::C_SCLString & orc_ProtocolName, const bool oq_HexActive,
                                const bool oq_RelativeTimeStampActive);
   virtual ~C_OSCComMessageLoggerFileAsc(void);

   virtual stw_types::sint32 OpenFile(void);
   virtual void AddMessageToFile(const C_OSCComMessageLoggerData & orc_MessageData);

private:
   //Avoid call
   C_OSCComMessageLoggerFileAsc(const C_OSCComMessageLoggerFileAsc &);
   C_OSCComMessageLoggerFileAsc & operator =(const C_OSCComMessageLoggerFileAsc &);

   void m_WriteHeader(void);
   static stw_scl::C_SCLString mh_GetAscTimeString(void);
   static stw_scl::C_SCLString mh_GetDay(const stw_types::sintn osn_Day);
   static stw_scl::C_SCLString mh_GetMonth(const stw_types::sintn osn_Month);
   static stw_scl::C_SCLString mh_AdaptTimeStamp(const stw_types::uint64 ou64_TimeStamp);

   const bool mq_HexActive;
   const bool mq_RelativeTimeStampActive;
   std::ofstream mc_File;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus reader/writer

   (See .cpp file for detailed description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMBUSFILERH
#define C_OSCSYSTEMBUSFILERH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OSCXMLParser.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemBusFiler
{
public:
   static stw_types::sint32 h_LoadBus(C_OSCSystemBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBus(const C_OSCSystemBus & orc_Bus, C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif

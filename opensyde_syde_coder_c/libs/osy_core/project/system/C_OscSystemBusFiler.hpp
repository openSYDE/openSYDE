//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus reader/writer

   (See .cpp file for detailed description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMBUSFILER_HPP
#define C_OSCSYSTEMBUSFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemBusFiler
{
public:
   static int32_t h_LoadBus(C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveBus(const C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser);

private:
   static int32_t mh_LoadCanFdProperties(C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveCanFdProperties(const C_OscSystemBus & orc_Bus, C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for standalone HALC config
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGSTANDALONEFILER_H
#define C_OSCHALCCONFIGSTANDALONEFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCXMLParser.h"
#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigStandaloneFiler
{
public:
   C_OSCHalcConfigStandaloneFiler(void);

   static stw_types::sint32 h_LoadFileStandalone(C_OSCHalcConfigStandalone & orc_IOData,
                                                 const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_SaveFileStandalone(const C_OSCHalcConfigStandalone & orc_IOData,
                                                 const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_LoadDataStandalone(C_OSCHalcConfigStandalone & orc_IOData,
                                                 C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_SaveDataStandalone(const C_OSCHalcConfigStandalone & orc_IOData,
                                                 C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

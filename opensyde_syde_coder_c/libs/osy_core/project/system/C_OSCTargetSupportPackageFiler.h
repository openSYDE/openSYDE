//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for target support package file data.

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCTARGETSUPPORTPACKAGEFILER_H
#define C_OSCTARGETSUPPORTPACKAGEFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCTargetSupportPackage.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCTargetSupportPackageFiler
{
public:
   static stw_types::sint32 h_Load(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                   const stw_scl::C_SCLString & orc_Path);

private:
   static stw_types::sint32 mh_Load(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                    C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_ParseApplication(C_OSCTargetSupportPackage & orc_TargetSupportPackage,
                                                C_OSCXMLParserBase & orc_XMLParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

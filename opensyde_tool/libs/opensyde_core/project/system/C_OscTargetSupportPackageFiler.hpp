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
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscTargetSupportPackage.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscTargetSupportPackageFiler
{
public:
   static int32_t h_Load(stw::opensyde_core::C_OscTargetSupportPackage & orc_TargetSupportPackage,
                         const stw::scl::C_SclString & orc_Path);

private:
   static int32_t mh_Load(C_OscTargetSupportPackage & orc_TargetSupportPackage, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_ParseApplication(C_OscTargetSupportPackage & orc_TargetSupportPackage,
                                      C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_ParseHalcSection(C_OscTargetSupportPackage & orc_TargetSupportPackage,
                                      C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

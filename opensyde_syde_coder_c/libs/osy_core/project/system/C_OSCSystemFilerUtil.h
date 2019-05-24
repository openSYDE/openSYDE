//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system filers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMFILERUTIL_H
#define C_OSCSYSTEMFILERUTIL_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OSCXMLParser.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemFilerUtil
{
public:
   C_OSCSystemFilerUtil(void);
   static stw_scl::C_SCLString mh_BusTypeEnumToString(const C_OSCSystemBus::E_Type oe_Type);
   static stw_types::sint32 mh_BusTypeStringToEnum(const stw_scl::C_SCLString & orc_Type,
                                                   C_OSCSystemBus::E_Type & ore_Type);
   static stw_types::sint32 mh_GetParserForExistingFile(C_OSCXMLParser & orc_FileXMLParser,
                                                        const stw_scl::C_SCLString & orc_Path,
                                                        const stw_scl::C_SCLString & orc_RootNode);
   static stw_types::sint32 mh_GetParserForNewFile(C_OSCXMLParser & orc_FileXMLParser,
                                                   const stw_scl::C_SCLString & orc_Path,
                                                   const stw_scl::C_SCLString & orc_RootNode);
   static stw_types::sint32 mh_CreateFolder(const stw_scl::C_SCLString & orc_Path);
   static stw_scl::C_SCLString mh_PrepareItemNameForFileName(const stw_scl::C_SCLString & orc_ItemName);
   static stw_scl::C_SCLString mh_CombinePaths(const stw_scl::C_SCLString & orc_BasePathName,
                                               const stw_scl::C_SCLString & orc_SubFolderFileName);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

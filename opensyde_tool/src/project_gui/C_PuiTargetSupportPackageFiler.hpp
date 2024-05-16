//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Target support package UI side handling
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUITARGETSUPPORTPACKAGEFILER_HPP
#define C_PUITARGETSUPPORTPACKAGEFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OscNode.hpp"
#include "C_PuiSdNode.hpp"
#include "C_OscTargetSupportPackage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiTargetSupportPackageFiler
{
public:
   static int32_t h_LoadTspV3(const QString & orc_TspPath, stw::opensyde_core::C_OscTargetSupportPackage & orc_Tsp,
                              stw::opensyde_core::C_OscNode & orc_OscNode, C_PuiSdNode & orc_UiNode);

private:
   C_PuiTargetSupportPackageFiler();

   static int32_t mh_DoUnzip(const QString & orc_ZipFilePath, QString & orc_FolderPath);
   static QString mh_GetNodeFilePath(const QString & orc_TspPath, const QString & orc_NodeFile);
   static QString mh_GetOscNodeXmlPath(const QString & orc_NodeFolder);
   static QString mh_GetUiNodeXmlPath(const QString & orc_NodeFolder);
   static QString mh_GetZipExtractFolder(void);
   static int32_t mh_DeleteFolder(const QString & orc_Folder);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

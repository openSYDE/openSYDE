//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN monitor utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMUTI_H
#define C_CAMUTI_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamUti
{
public:
   static QString h_GetAbsPathFromProj(const QString & orc_Path);
   static QString h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir);
   static QString h_ResolvePlaceholderVariables(const QString & orc_Path);
   static QString h_GetResolvedAbsolutePathFromProj(const QString & orc_Path);
   static QString h_GetResolvedAbsolutePathFromExe(const QString & orc_Path);

private:
   C_CamUti(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

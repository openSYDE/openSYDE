//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing any file as part of a file based node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMFILEWIDGET_H
#define C_SYVUPPACLISTNODEITEMFILEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacListNodeItemWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemFileWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemFileWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex,
                                    const QString & orc_DeviceName, const bool oq_FileBased,
                                    const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   virtual stw_types::uint32 GetType(void) const override;
   virtual bool IsViewFileInfoPossible(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

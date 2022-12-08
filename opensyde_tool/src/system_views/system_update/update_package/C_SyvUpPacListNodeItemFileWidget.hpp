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
#include "C_SyvUpPacListNodeItemWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemFileWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemFileWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                    const QString & orc_DeviceName, const bool oq_FileBased,
                                    const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   uint32_t GetType(void) const override;
   bool IsViewFileInfoPossible(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

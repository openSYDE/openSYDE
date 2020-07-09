//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing any file as part of a file based node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEITEMFILEWIDGET_H
#define C_SYVUPPACKAGELISTNODEITEMFILEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpUpdatePackageListNodeItemWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeItemFileWidget :
   public C_SyvUpUpdatePackageListNodeItemWidget
{
public:
   C_SyvUpPackageListNodeItemFileWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex,
                                        const QString & orc_DeviceName, const bool oq_FileBased,
                                        const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   virtual bool IsViewFileInfoPossible(void) const override;
   virtual bool IsUserHintPossible(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

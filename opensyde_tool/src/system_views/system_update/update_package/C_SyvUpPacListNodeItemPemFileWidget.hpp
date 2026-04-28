//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific PEM file added to other files of a node
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMPEMFILEWIDGET_HPP
#define C_SYVUPPACLISTNODEITEMPEMFILEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacListNodeItemWidget.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemPemFileWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemPemFileWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                       const QString & orc_DeviceName, const bool oq_FileBased,
                                       const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   uint32_t GetType(void) const override;
   void ViewFileInfo(void) override;
   bool IsViewFileInfoPossible(void) const override;

protected:
   void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove) override;
   QString m_CreateToolTipTitle(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

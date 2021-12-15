//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific PEM file added to other files of a node
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEITEMPEMFILEWIDGET_H
#define C_SYVUPPACKAGELISTNODEITEMPEMFILEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPackageListNodeItemWidget.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeItemPemFileWidget :
   public C_SyvUpPackageListNodeItemWidget
{
public:
   C_SyvUpPackageListNodeItemPemFileWidget(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_NodeIndex, const QString & orc_DeviceName,
                                           const bool oq_FileBased, const bool oq_StwFlashloader,
                                           QWidget * const opc_Parent = NULL);

   virtual stw_types::uint32 GetType(void) const override;
   virtual void ViewFileInfo(void) override;
   virtual bool IsViewFileInfoPossible(void) const override;

   void SetPemStates(const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                     const stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);
   void GetPemStates(stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity & ore_StateSecurity,
                     stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger & ore_StateDebugger) const;

protected:
   virtual void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                      bool & orq_TriggerRemove) override;
   virtual QString m_CreateToolTipTitle(void) const override;

private:
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity me_StateSecurity;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger me_StateDebugger;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific PEM file added to other files of a node
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMPEMFILEWIDGET_H
#define C_SYVUPPACLISTNODEITEMPEMFILEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacListNodeItemWidget.hpp"
#include "C_PuiSvNodeUpdate.hpp"

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

   void SetPemStates(const stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                     const stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);
   void GetPemStates(stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity & ore_StateSecurity,
                     stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger & ore_StateDebugger) const;

protected:
   void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove) override;
   QString m_CreateToolTipTitle(void) const override;

private:
   stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateSecurity me_StateSecurity;
   stw::opensyde_gui_logic::C_PuiSvNodeUpdate::E_StateDebugger me_StateDebugger;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

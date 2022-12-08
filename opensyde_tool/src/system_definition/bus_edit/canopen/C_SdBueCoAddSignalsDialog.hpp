//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog for choosing and adding signals to CANopen manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUECOADDSIGNALSDIALOG_H
#define C_SDBUECOADDSIGNALSDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_OscCanOpenManagerMappableSignal.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueCoAddSignalsDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueCoAddSignalsDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueCoAddSignalsDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   ~C_SdBueCoAddSignalsDialog(void) override;

   void InitStaticNames(void) const;
   std::vector<stw::opensyde_core::C_OscCanOpenManagerMappableSignal> GetSelectedSignals(void) const;
   void PrepareCleanUp(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdBueCoAddSignalsDialog * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_HandleHiding(void) const;
   void m_OnSearch(const QString & orc_Text) const;
   void m_UpdateSelection(const int32_t os32_SelectionCount) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);

   //Avoid call
   C_SdBueCoAddSignalsDialog(const C_SdBueCoAddSignalsDialog &);
   C_SdBueCoAddSignalsDialog & operator =(const C_SdBueCoAddSignalsDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

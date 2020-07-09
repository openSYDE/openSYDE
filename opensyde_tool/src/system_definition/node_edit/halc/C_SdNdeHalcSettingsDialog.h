//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for HALC settings.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCSETTINGSDIALOG_H
#define C_SDNDEHALCSETTINGSDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcSettingsDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcSettingsDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcSettingsDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                      const stw_types::uint32 ou32_NodeIndex);
   ~C_SdNdeHalcSettingsDialog(void);

   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeHalcSettingsDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;

   void m_InitComboboxes(void) const;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SdNdeHalcSettingsDialog(const C_SdNdeHalcSettingsDialog &);
   C_SdNdeHalcSettingsDialog & operator =(const C_SdNdeHalcSettingsDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

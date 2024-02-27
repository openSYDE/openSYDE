//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Hex file view
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPHEXFILEVIEW_H
#define C_FLAUPHEXFILEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_FlaUpHexFileInfo.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaUpHexFileView;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUpHexFileView :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaUpHexFileView(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                               const C_FlaUpHexFileInfo & orc_HexFileInfo);
   ~C_FlaUpHexFileView(void) override;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_FlaUpHexFileView * mpc_Ui;
   void m_OkClicked(void);
   void m_LoadInfo(const C_FlaUpHexFileInfo * const opc_HexFileInfo) const;
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const QString mhc_START_TD;
   static const QString mhc_CONTINUE_TD;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

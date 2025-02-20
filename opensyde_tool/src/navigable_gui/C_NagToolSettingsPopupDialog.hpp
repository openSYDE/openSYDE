//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for global tool settings
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOOLSETTINGSPOPUPDIALOG_HPP
#define C_NAGTOOLSETTINGSPOPUPDIALOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscSystemNameMaxCharLimitChangeReportItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagToolSettingsPopupDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagToolSettingsPopupDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagToolSettingsPopupDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_NagToolSettingsPopupDialog(void) override;

   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_NagToolSettingsPopupDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitEnvironmentSection(void);

   //Avoid call
   C_NagToolSettingsPopupDialog(const C_NagToolSettingsPopupDialog &);
   C_NagToolSettingsPopupDialog & operator =(const C_NagToolSettingsPopupDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

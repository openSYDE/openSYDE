//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for pem file information
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPEMFILEINFOPOPUP_H
#define C_SYVUPPEMFILEINFOPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPemFileInfoPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPemFileInfoPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPemFileInfoPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_Path);
   ~C_SyvUpPemFileInfoPopUp(void);

   void InitStaticNames(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPemFileInfoPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   //Avoid call
   C_SyvUpPemFileInfoPopUp(const C_SyvUpPemFileInfoPopUp &);
   C_SyvUpPemFileInfoPopUp & operator =(const C_SyvUpPemFileInfoPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

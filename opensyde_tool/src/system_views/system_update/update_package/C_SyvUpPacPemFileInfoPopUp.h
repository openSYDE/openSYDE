//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for pem file information
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPEMFILEINFOPOPUP_H
#define C_SYVUPPACPEMFILEINFOPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacPemFileInfoPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFileInfoPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFileInfoPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const QString & orc_Path);
   ~C_SyvUpPacPemFileInfoPopUp(void);

   void InitStaticNames(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacPemFileInfoPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   //Avoid call
   C_SyvUpPacPemFileInfoPopUp(const C_SyvUpPacPemFileInfoPopUp &);
   C_SyvUpPacPemFileInfoPopUp & operator =(const C_SyvUpPacPemFileInfoPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

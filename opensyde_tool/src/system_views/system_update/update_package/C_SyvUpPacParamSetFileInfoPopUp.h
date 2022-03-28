//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for parameter set file information (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPARAMSETFILEINFOPOPUP_H
#define C_SYVUPPACPARAMSETFILEINFOPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvUpPacParamSetFileInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacParamSetFileInfoPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileInfoPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacParamSetFileInfoPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                            const QString & orc_Path, const stw_types::uint32 ou32_NodeIndex);
   ~C_SyvUpPacParamSetFileInfoPopUp(void);

   void InitStaticNames(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacParamSetFileInfoPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw_opensyde_gui_logic::C_SyvUpPacParamSetFileInfo mc_FileInfo;

   stw_types::sint32 m_ReadFile(void);
   void m_OkClicked(void);

   //Avoid call
   C_SyvUpPacParamSetFileInfoPopUp(const C_SyvUpPacParamSetFileInfoPopUp &);
   C_SyvUpPacParamSetFileInfoPopUp & operator =(const C_SyvUpPacParamSetFileInfoPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

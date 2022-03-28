//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for parameter set file information (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPARAMSETFILEADDPOPUP_H
#define C_SYVUPPACPARAMSETFILEADDPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvUpPacParamSetFileInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacParamSetFileAddPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileAddPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacParamSetFileAddPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const QString & orc_Path, const QString & orc_StoragePath,
                                           const stw_types::uint32 ou32_NodeIndex);
   ~C_SyvUpPacParamSetFileAddPopUp(void);

   stw_types::sint32 ReadFile(void);
   const stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & GetParamInfo(void) const;
   void InitStaticNames(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacParamSetFileAddPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw_opensyde_gui_logic::C_SyvUpPacParamSetFileInfo mc_FileInfo;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SyvUpPacParamSetFileAddPopUp(const C_SyvUpPacParamSetFileAddPopUp &);
   C_SyvUpPacParamSetFileAddPopUp & operator =(const C_SyvUpPacParamSetFileAddPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

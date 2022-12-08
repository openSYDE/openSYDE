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
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvUpPacParamSetFileInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacParamSetFileAddPopUp;
}
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileAddPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacParamSetFileAddPopUp(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const QString & orc_Path, const QString & orc_StoragePath,
                                           const uint32_t ou32_NodeIndex);
   ~C_SyvUpPacParamSetFileAddPopUp(void) override;

   int32_t ReadFile(void);
   const stw::opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & GetParamInfo(void) const;
   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpPacParamSetFileAddPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw::opensyde_gui_logic::C_SyvUpPacParamSetFileInfo mc_FileInfo;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SyvUpPacParamSetFileAddPopUp(const C_SyvUpPacParamSetFileAddPopUp &);
   C_SyvUpPacParamSetFileAddPopUp & operator =(const C_SyvUpPacParamSetFileAddPopUp &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

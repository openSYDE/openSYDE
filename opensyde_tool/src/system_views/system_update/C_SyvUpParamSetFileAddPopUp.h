//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for parameter set file information (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPARAMSETFILEADDPOPUP_H
#define C_SYVUPPARAMSETFILEADDPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvUpParamSetFileInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpParamSetFileAddPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpParamSetFileAddPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpParamSetFileAddPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                        const QString & orc_Path, const QString & orc_StoragePath,
                                        const stw_types::uint32 ou32_NodeIndex);
   ~C_SyvUpParamSetFileAddPopUp(void);

   stw_types::sint32 ReadFile(void);
   const stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & GetParamInfo(void) const;
   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvUpParamSetFileAddPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw_opensyde_gui_logic::C_SyvUpParamSetFileInfo mc_FileInfo;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SyvUpParamSetFileAddPopUp(const C_SyvUpParamSetFileAddPopUp &);
   C_SyvUpParamSetFileAddPopUp & operator =(const C_SyvUpParamSetFileAddPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

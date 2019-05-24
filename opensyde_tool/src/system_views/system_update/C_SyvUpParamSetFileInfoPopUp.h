//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Screen for parameter set file information (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPARAMSETFILEINFOPOPUP_H
#define C_SYVUPPARAMSETFILEINFOPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvUpParamSetFileInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpParamSetFileInfoPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpParamSetFileInfoPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpParamSetFileInfoPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const QString & orc_Path, const stw_types::uint32 ou32_NodeIndex);
   ~C_SyvUpParamSetFileInfoPopUp(void);

   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvUpParamSetFileInfoPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw_opensyde_gui_logic::C_SyvUpParamSetFileInfo mc_FileInfo;

   stw_types::sint32 m_ReadFile(void);
   void m_OkClicked(void);

   //Avoid call
   C_SyvUpParamSetFileInfoPopUp(const C_SyvUpParamSetFileInfoPopUp &);
   C_SyvUpParamSetFileInfoPopUp & operator =(const C_SyvUpParamSetFileInfoPopUp &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

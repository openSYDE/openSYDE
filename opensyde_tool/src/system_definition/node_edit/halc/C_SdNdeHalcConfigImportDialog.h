//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for import of a HALC configuration file
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGIMPORTDIALOG_H
#define C_SDNDEHALCCONFIGIMPORTDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcConfigImportDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcConfigImportDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const stw_types::uint32 ou32_NodeIndex, const QString & orc_ImportFileName);
   ~C_SdNdeHalcConfigImportDialog(void);

   void InitStaticNames(void) const;

   stw_types::sint32 PrepareDialog(QString & orc_ErrorDetails);
   stw_types::sint32 GetResult(QString & orc_ErrorDetails) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeHalcConfigImportDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const stw_types::uint32 mu32_NodeIndex;
   const QString mc_ImportFileName;
   stw_types::sint32 ms32_Result;

   stw_opensyde_core::C_OSCHalcConfigStandalone mc_ImportConfig;

   void m_OkClicked(void);
   void m_OnCancel(void);

   //Avoid call
   C_SdNdeHalcConfigImportDialog(const C_SdNdeHalcConfigImportDialog &);
   C_SdNdeHalcConfigImportDialog & operator =(const C_SdNdeHalcConfigImportDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace
#endif

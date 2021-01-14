//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for generate code
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCODEGENERATIONDIALOG_H
#define C_SDCODEGENERATIONDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdCodeGenerationDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdCodeGenerationDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdCodeGenerationDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdCodeGenerationDialog(void);

   void InitStaticNames(void) const;
   void PrepareDialog(const std::vector<stw_types::uint32> & orc_NodesIndices) const;
   void GetCheckedItems(std::vector<stw_types::uint32> & orc_NodeIndices,
                        std::vector<std::vector<stw_types::uint32> > & orc_AppIndicesPerNode) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdCodeGenerationDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_OnCancel(void);
   void m_UpdateSelection(const stw_types::sintn osn_SelectionCount) const;

   //Avoid call
   C_SdCodeGenerationDialog(const C_SdCodeGenerationDialog &);
   C_SdCodeGenerationDialog & operator =(const C_SdCodeGenerationDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace
#endif

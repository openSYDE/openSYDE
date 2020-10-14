//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for editing Datapool list comment.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTCOMMENTDIALOG_H
#define C_SDNDEDPLISTCOMMENTDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpListCommentDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListCommentDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListCommentDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const stw_types::uint32 ou32_NodeIndex,
                                       const stw_types::uint32 ou32_DataPoolIndex,
                                       const stw_types::uint32 ou32_ListIndex);
   ~C_SdNdeDpListCommentDialog(void);

   void InitStaticNames(void) const;
   QString GetComment(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDpListCommentDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SdNdeDpListCommentDialog(const C_SdNdeDpListCommentDialog &);
   C_SdNdeDpListCommentDialog & operator =(const C_SdNdeDpListCommentDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

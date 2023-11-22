//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for editing Datapool list comment.
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTCOMMENTDIALOG_HPP
#define C_SDNDEDPLISTCOMMENTDIALOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpListCommentDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListCommentDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListCommentDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                       const uint32_t ou32_ListIndex);
   ~C_SdNdeDpListCommentDialog(void) override;

   void InitStaticNames(void) const;
   QString GetComment(void) const;
   void GetCommentToEditor(const QString oc_Comment) const;
   void SetTitle(const QString oc_Title);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDpListCommentDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_DataPoolIndex;
   const uint32_t mu32_ListIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SdNdeDpListCommentDialog(const C_SdNdeDpListCommentDialog &);
   C_SdNdeDpListCommentDialog & operator =(const C_SdNdeDpListCommentDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

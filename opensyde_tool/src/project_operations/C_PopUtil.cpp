//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility functions for project operations (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>

#include "C_PopErrorHandling.h"
#include "C_PopUtil.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgePopUpDialog.h"
#include "C_PopSaveAsDialogWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Asks the user if the changes shall be saved, rejected or canceled

   \param[in] opc_Parent                    Parent widget
   \param[in] oq_AllowContinueWithoutSaving Optional flag to block continuing without saving

   \return
   true     Action can be continued
   false    Cancel the action

   \created     13.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_PopUtil::h_AskUserToContinue(QWidget * const opc_Parent, const bool oq_AllowContinueWithoutSaving)
{
   bool q_Return = false;

   if ((C_PuiSdHandler::h_GetInstance()->HasHashChanged() == true) ||
       ((C_PuiProject::h_GetInstance()->HasHashChanged() == true) ||
        (C_PuiSvHandler::h_GetInstance()->HasHashChanged() == true)))
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Unsaved changes"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you want to save the changes of the current project?"));
      if (oq_AllowContinueWithoutSaving == true)
      {
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Save"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Don't Save"));
         c_MessageBox.ShowCancelButton();
      }
      else
      {
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Save and Continue"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
      }
      e_ReturnMessageBox = c_MessageBox.Execute();

      switch (e_ReturnMessageBox)
      {
      case C_OgeWiCustomMessage::eYES:
         // check if empty path (new unsaved project)
         if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
         {
            // open save as dialog
            QPointer<stw_opensyde_gui_elements::C_OgePopUpDialog> const c_New =
               new stw_opensyde_gui_elements::C_OgePopUpDialog(opc_Parent, opc_Parent);
            C_PopSaveAsDialogWidget * const pc_Dialog = new C_PopSaveAsDialogWidget(*c_New);
            if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               q_Return = true;
               // TODO trigger update of window title...
            }
            if (c_New != NULL)
            {
               pc_Dialog->SaveUserSettings();
               c_New->HideOverlay();
            }
            //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
         }
         else
         {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            //Save project
            C_PopErrorHandling::mh_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), opc_Parent);
            QApplication::restoreOverrideCursor();
            q_Return = true;
         }
         break;
      case C_OgeWiCustomMessage::eNO:
         if (oq_AllowContinueWithoutSaving == true)
         {
            // continue and do nothing
            q_Return = true;
         }
         else
         {
            // do not continue and do nothing
         }
         break;
      case C_OgeWiCustomMessage::eCANCEL:
         // do not continue and do nothing
         break;
      default:
         break;
      }
   }
   else
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Private constructor

   \created     13.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PopUtil::C_PopUtil(void)
{
}

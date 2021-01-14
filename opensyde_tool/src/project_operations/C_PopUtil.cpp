//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for project operations (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include <QFileInfo>

#include "C_PopErrorHandling.h"
#include "C_PopUtil.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OgePopUpDialog.h"
#include "C_PopSaveAsDialogWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Asks the user if the changes shall be saved, rejected or canceled

   \param[in] opc_Parent                    Parent widget
   \param[in] oq_AllowContinueWithoutSaving Optional flag to block continuing without saving

   \return
   true     Action can be continued
   false    Cancel the action
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PopUtil::h_AskUserToContinue(QWidget * const opc_Parent, const bool oq_AllowContinueWithoutSaving)
{
   bool q_Return = false;

   if ((C_PuiSdHandler::h_GetInstance()->HasHashChanged() == true) ||
       ((C_PuiProject::h_GetInstance()->HasHashChanged() == true) ||
        (C_PuiSvHandler::h_GetInstance()->HasHashChanged() == true)))
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      QString c_Description = "";
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Unsaved changes"));
      if (oq_AllowContinueWithoutSaving == true)
      {
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Save"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Don't Save"));
         c_MessageBox.ShowCancelButton();
      }
      else
      {
         c_Description = C_GtGetText::h_GetText("To continue the current project changes have to be saved. ");
         c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Save and Continue"));
         c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
      }
      c_Description += static_cast<QString>(C_GtGetText::h_GetText("Do you want to save the changes of the current project"));

      //Don't set details. User expects in details something like a list of what was changed in project
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == false)
      {
         c_Description += " \"" + C_PuiProject::h_GetInstance()->GetName() + "\"";
      }
      c_Description += "?";
      c_MessageBox.SetDescription(c_Description);
      c_MessageBox.SetCustomMinHeight(180, 180);
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
         }  //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
         else
         {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            //Save project
            C_PopErrorHandling::h_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), opc_Parent);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for critical node names and notify user if necessary

   \param[in,out] opc_Parent                 Parent widget
   \param[in]     oq_DisableActiveWaitCursor Optional flag to call restore of wait cursor before showing a pop up and
                                             restoring the wait cursor after that so the caller does not need a separate
                                             handling

   \return
   True  Naming conflict detected
   False No naming conflict detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PopUtil::h_CheckCriticalNamingConflict(QWidget * const opc_Parent, const bool oq_DisableActiveWaitCursor)
{
   std::vector<QString> c_CriticalNodeNames;
   std::vector<QString> c_CriticalBusNames;
   std::vector<QString> c_CriticalDatapoolNamespaceNames;
   const bool q_Retval = C_PuiSdHandler::h_GetInstance()->CheckCriticalNameConflict(&c_CriticalNodeNames,
                                                                                    &c_CriticalBusNames,
                                                                                    &c_CriticalDatapoolNamespaceNames);
   if (q_Retval)
   {
      QString c_Details;
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project save"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "The project cannot be saved with conflicting names for either nodes, buses or Datapools"));
      if (c_CriticalNodeNames.size() > 0UL)
      {
         c_Details.append(C_GtGetText::h_GetText("Conflicting node names:\n"));
         for (uint32 u32_ItName = 0UL; u32_ItName < c_CriticalNodeNames.size(); ++u32_ItName)
         {
            c_Details.append(static_cast<QString>("\"%1\"\n").arg(c_CriticalNodeNames[u32_ItName]));
         }
      }
      if (c_CriticalBusNames.size() > 0UL)
      {
         c_Details.append(C_GtGetText::h_GetText("Conflicting bus names:\n"));
         for (uint32 u32_ItName = 0UL; u32_ItName < c_CriticalBusNames.size(); ++u32_ItName)
         {
            c_Details.append(static_cast<QString>("\"%1\"\n").arg(c_CriticalBusNames[u32_ItName]));
         }
      }
      if (c_CriticalDatapoolNamespaceNames.size() > 0UL)
      {
         c_Details.append(C_GtGetText::h_GetText("Conflicting datapool names:\n"));
         for (uint32 u32_ItName = 0UL; u32_ItName < c_CriticalDatapoolNamespaceNames.size(); ++u32_ItName)
         {
            c_Details.append(static_cast<QString>("%1\n").arg(c_CriticalDatapoolNamespaceNames[u32_ItName]));
         }
      }
      c_Message.SetDetails(c_Details);
      if (oq_DisableActiveWaitCursor)
      {
         QApplication::restoreOverrideCursor();
      }
      c_Message.SetCustomMinHeight(230, 300);
      c_Message.Execute();
      if (oq_DisableActiveWaitCursor)
      {
         QApplication::setOverrideCursor(Qt::WaitCursor);
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Private constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopUtil::C_PopUtil(void)
{
}

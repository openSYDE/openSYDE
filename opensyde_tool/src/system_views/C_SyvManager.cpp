//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       System views manager (add, delete, ...) (implementation)

   System views manager (add, delete, ...)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_SyvUtil.h"
#include "C_GtGetText.h"
#include "C_SyvManager.h"
#include "C_PuiSvHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
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
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvManager::C_SyvManager(QObject * const opc_Parent) :
   QObject(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle add new view action

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::AddView(void)
{
   const uint32 u32_NewViewIndex = C_PuiSvHandler::h_GetInstance()->GetViewCount();
   QString c_SubMode;
   QString c_SubSubMode;

   C_PuiSvHandler::h_GetInstance()->AddView(C_PuiSvData(), true, true);
   //Add one dashboard per view
   C_PuiSvHandler::h_GetInstance()->AddDashboard(u32_NewViewIndex, C_PuiSvDashboard(), true);

   C_SyvUtil::h_GetViewDisplayName(u32_NewViewIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);

   //Trigger reload
   Q_EMIT this->SigReloadNaviBarSystemViewContent();
   //Select new view
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, ms32_SUBMODE_SYSVIEW_SETUP, u32_NewViewIndex, c_SubMode, c_SubSubMode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update all view names

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::UpdateAllViewNames(void) const
{
   Q_EMIT this->SigReloadNaviBarSystemViewNames();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle initial view setup

   \created     21.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::HandleInitialSystemView(void)
{
   //Check if new project
   if (C_PuiSvHandler::h_GetInstance()->GetViewCount() == 0UL)
   {
      this->AddView();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle rename view action

   Warning: Name assumed to be correct

   \param[in] ou32_Index Index identifier
   \param[in] orc_Name   New name

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::RenameView(const uint32 ou32_Index, const QString & orc_Name) const
{
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewName(ou32_Index, orc_Name) == C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle move view action

   \param[in] ou32_StartIndex  Start index
   \param[in] ou32_TargetIndex Target index

   \created     11.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::MoveView(const uint32 ou32_StartIndex, const uint32 ou32_TargetIndex)
{
   if ((ou32_StartIndex < C_PuiSvHandler::h_GetInstance()->GetViewCount()) &&
       (ou32_TargetIndex < C_PuiSvHandler::h_GetInstance()->GetViewCount()))
   {
      QString c_SubMode;
      QString c_SubSubMode;
      tgl_assert(C_PuiSvHandler::h_GetInstance()->MoveView(ou32_StartIndex, ou32_TargetIndex) == C_NO_ERR);

      C_SyvUtil::h_GetViewDisplayName(ou32_TargetIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
      //Update all names
      this->UpdateAllViewNames();
      //Select moved view
      //Flag = 1 -> do not trigger widget prepare to close (has to have happened before this call)
      Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, ms32_SUBMODE_SYSVIEW_SETUP, ou32_TargetIndex, c_SubMode,
                                 c_SubSubMode, 1UL);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Duplicate system view

   \param[in] ou32_Index System view index

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::DuplicateSysView(const uint32 ou32_Index)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_Index);

   if (pc_View != NULL)
   {
      const uint32 u32_NewViewIndex = ou32_Index + 1UL;
      QString c_SubMode;
      QString c_SubSubMode;

      C_PuiSvHandler::h_GetInstance()->InsertView(u32_NewViewIndex, *pc_View, true, false);

      C_SyvUtil::h_GetViewDisplayName(u32_NewViewIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
      //Trigger reload
      Q_EMIT this->SigReloadNaviBarSystemViewContent();
      //Select new view
      Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, ms32_SUBMODE_SYSVIEW_SETUP, u32_NewViewIndex, c_SubMode,
                                 c_SubSubMode);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle delete view action

   \param[in] ou32_Index           View index to delete
   \param[in] os32_SelectedSubMode Currently selected submode
   \param[in] ou32_SelectedIndex   Currently selected view index

   \created     13.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvManager::DeleteSysView(const uint32 ou32_Index, const sint32 os32_SelectedSubMode,
                                 const uint32 ou32_SelectedIndex, QWidget * const opc_Parent)
{
   if (C_PuiSvHandler::h_GetInstance()->GetViewCount() > 1UL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_Index);
      if (pc_View != NULL)
      {
         C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eQUESTION);
         c_Message.SetHeading(C_GtGetText::h_GetText("View delete"));
         c_Message.SetDescription(QString(C_GtGetText::h_GetText(
                                             "Do you really want to delete this view (%1)?")).arg(
                                     pc_View->GetName()));
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
         c_Message.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
         if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
         {
            QString c_SubMode;
            QString c_SubSubMode;

            tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteView(ou32_Index) == C_NO_ERR);
            //Important reload view
            Q_EMIT this->SigReloadNaviBarSystemViewContent();
            //Select?
            if (ou32_SelectedIndex < C_PuiSvHandler::h_GetInstance()->GetViewCount())
            {
               C_SyvUtil::h_GetViewDisplayName(ou32_SelectedIndex, os32_SelectedSubMode, c_SubMode, c_SubSubMode);

               //Flag = 1 -> do not trigger widget prepare to close (has to have happened before this call)
               Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, os32_SelectedSubMode, ou32_SelectedIndex, c_SubMode,
                                          c_SubSubMode, 1UL);
            }
            else
            {
               C_SyvUtil::h_GetViewDisplayName(ou32_SelectedIndex - 1UL, os32_SelectedSubMode, c_SubMode,
                                               c_SubSubMode);

               //Flag = 1 -> do not trigger widget prepare to close (has to have happened before this call)
               Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, os32_SelectedSubMode, ou32_SelectedIndex - 1UL,
                                          c_SubMode,
                                          c_SubSubMode, 1UL);
            }
         }
      }
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eINFORMATION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("View delete"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("You cannot delete the last view."));
      c_MessageBox.Execute();
   }
}

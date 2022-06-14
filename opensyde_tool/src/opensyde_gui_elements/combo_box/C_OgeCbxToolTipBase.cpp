//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with tool tip (implementation)

   Combo box with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QListView>
#include "C_OgeWiUtil.h"
#include "C_OgeCbxToolTipBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxToolTipBase::C_OgeCbxToolTipBase(QWidget * const opc_Parent) :
   QComboBox(opc_Parent)
{
   // Deactivate custom context menu of scroll bar
   this->view()->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->view()->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeCbxToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (this->mq_ToolTipActive == true)
   {
      if (opc_Event->type() == QEvent::ToolTip)
      {
         //show tooltip
         if (this->m_GetToolTip()->isVisible() == false)
         {
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

            if (pc_HelpEvent != NULL)
            {
               this->setMouseTracking(true);

               this->m_GetToolTip()->SetHeading(this->mc_ToolTipHeading);
               this->m_GetToolTip()->SetContent(this->mc_ToolTipContent);
               this->m_GetToolTip()->SetType(this->me_ToolTipType);
               this->m_GetToolTip()->show();
               this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
            }
         }

         //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this widget
         // already
         opc_Event->accept();
         q_Return = true;
      }
      else if (opc_Event->type() == QEvent::Leave)
      {
         //hide on leave
         this->m_HideToolTip();

         this->setMouseTracking(false);

         opc_Event->accept();
         q_Return = true;
      }
      else
      {
         q_Return = QComboBox::event(opc_Event);
      }
   }
   else
   {
      q_Return = QComboBox::event(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set item at index as disabled

   \param[in] os32_Index Index to disable
   \param[in] oq_Status  Item status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxToolTipBase::SetItemState(const stw_types::sint32 os32_Index, const bool oq_Status) const
{
   if (os32_Index < this->count())
   {
      QStandardItemModel * const pc_Model = dynamic_cast<QStandardItemModel * const>(this->model());
      if (pc_Model != NULL)
      {
         QStandardItem * const pc_Item = pc_Model->item(os32_Index);
         if (pc_Item != NULL)
         {
            Qt::ItemFlags c_Flags;
            c_Flags.setFlag(Qt::ItemIsEnabled, oq_Status);
            c_Flags.setFlag(Qt::ItemIsSelectable, oq_Status);
            //Disable
            pc_Item->setFlags(c_Flags);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set item at index as visible or invisible

   \param[in] os32_Index Index to disable
   \param[in] oq_Visible Item visibility flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxToolTipBase::SetItemVisible(const stw_types::sint32 os32_Index, const bool oq_Visible) const
{
   if (os32_Index < this->count())
   {
      QListView * const pc_ListView = dynamic_cast<QListView *>(this->view());

      // Hide the Combobox item
      if (pc_ListView != NULL)
      {
         pc_ListView->setRowHidden(os32_Index, !oq_Visible);
      }

      this->SetItemState(os32_Index, oq_Visible);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Explicitly ignore events which are globally handled (Won't be forwarded otherwise!)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxToolTipBase::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (C_OgeWiUtil::h_CheckGlobalKey(opc_Event) == true)
   {
      opc_Event->ignore();
   }
   else
   {
      QComboBox::keyPressEvent(opc_Event);
   }
}

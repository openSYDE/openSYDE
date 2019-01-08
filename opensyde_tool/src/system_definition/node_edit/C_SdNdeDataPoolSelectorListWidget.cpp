//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       List with all datapools of a specific type of one node(implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include <QApplication>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"

#include "C_SdNdeDataPoolSelectorListWidget.h"
#include "C_SdNdeDataPoolSelectorItemWidget.h"
#include "C_GtGetText.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDatapoolProperties.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdNodeDataPool.h"
#include "C_SdClipBoardHelper.h"
#include "C_Uti.h"
#include "C_PuiSdUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolSelectorListWidget::C_SdNdeDataPoolSelectorListWidget(QWidget * const opc_Parent) :
   C_OgeHorizontalListWidget(opc_Parent),
   mpc_ContextMenu(NULL),
   mpc_AddAction(NULL),
   mpc_EditAction(NULL),
   mpc_DeleteAction(NULL),
   mpc_CopyAction(NULL),
   mpc_CutAction(NULL),
   mpc_PasteAction(NULL),
   mpc_MoveLeftAction(NULL),
   mpc_MoveRightAction(NULL),
   mu32_NodeIndex(0),
   ms32_LastKnownDpCount(0),
   me_DataPoolType(stw_opensyde_core::C_OSCNodeDataPool::eDIAG),
   mq_ItemActive(false),
   mq_Maximized(true),
   mq_UsageViewActive(false),
   mq_InteractionAvailable(false)
{
   this->msn_ItemsPerLine = 3;
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   this->setItemDelegate(&this->mc_Delegate);

   //Custom context menu
   m_SetupContextMenu();

   connect(this, &C_SdNdeDataPoolSelectorListWidget::itemSelectionChanged,
           this, &C_SdNdeDataPoolSelectorListWidget::m_SelectionChanged);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolSelectorListWidget::~C_SdNdeDataPoolSelectorListWidget()
{
   //lint -e{429,1540}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Specify associated node

   \param[in] oe_Type              Datapool type
   \param[in] ou32_NodeIndex       Node index
   \param[in] oq_UsageViewActive   Flag for showing usage bars for datapool

   \return
   true   datapool added
   false  no datapool added

   \created     17.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorListWidget::SetTypeAndNode(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_Type,
                                                       const uint32 ou32_NodeIndex, const bool oq_UsageViewActive)
{
   bool q_Return = false;

   this->mq_UsageViewActive = oq_UsageViewActive;
   this->me_DataPoolType = oe_Type;
   this->mu32_NodeIndex = ou32_NodeIndex;

   this->m_InitFromData();
   Q_EMIT this->SigDataPoolChanged(true);

   if (this->count() > 0)
   {
      q_Return = true;
   }

   //Trigger interaction availability check
   CheckDataPoolInteraction();

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the conflict state of the active datapool

   \param[in] oq_Active     Flag if conflict is active or not

   \return
   true     minimum one conflict
   false    no conflicts in all datapools

   \created     22.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorListWidget::SetActualDataPoolConflict(const bool oq_Active) const
{
   bool q_Return = false;
   sintn sn_Counter;
   QListWidgetItem * pc_Item;
   C_SdNdeDataPoolSelectorItemWidget * pc_WidgetItem;

   if (oq_Active == true)
   {
      // activate the conflict state for the actual datapool
      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         pc_Item = this->item(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            if (pc_WidgetItem->GetActive() == true)
            {
               pc_WidgetItem->SetStateConflict(true);
            }
            break;
         }
      }

      q_Return = true;
   }
   else
   {
      // check all datapools for a conflict and deactivate the conflict state of the actual datapool
      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         pc_Item = this->item(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            //Avoid current row because on error change this widget does not necessarily have the focus
            if (pc_WidgetItem->GetActive() == true)
            {
               // deactivate the conflict of this datapool
               pc_WidgetItem->SetStateConflict(false);
            }

            // ask before to improve performance
            if (pc_WidgetItem->GetStateConflict() == true)
            {
               // has any datapool an conflict?
               q_Return = true;
            }
         }
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger check of data pool interaction availability

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::CheckDataPoolInteraction(void)
{
   if (this->me_DataPoolType == stw_opensyde_core::C_OSCNodeDataPool::eCOM)
   {
      this->mq_InteractionAvailable = true;
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if (((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL)) &&
          (pc_Node->pc_DeviceDefinition->q_ProgrammingSupport == true))
      {
         this->mq_InteractionAvailable = true;
      }
      else
      {
         this->mq_InteractionAvailable = false;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the widget for the actual datapool

   \created     24.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::UpdateActualDataPool(void)
{
   const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                this->me_DataPoolType,
                                                                                this->currentRow());

   if (s32_DpIndex >= 0)
   {
      // reload data of widget
      this->m_UpdateDataPoolWidget(static_cast<uint32>(s32_DpIndex), this->currentRow());
      Q_EMIT this->SigDataPoolChanged();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new datapool widget to the list

   \param[in] oru32_DataPoolIndex   Data pool index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_AddDataPoolWidget(const uint32 ou32_DataPoolIndex)
{
   C_OSCNodeDataPool c_OSCDataPool;
   C_PuiSdNodeDataPool c_UIDataPool;
   sint32 s32_Return;

   s32_Return = C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex, c_OSCDataPool,
                                                             c_UIDataPool);

   if (s32_Return == C_NO_ERR)
   {
      // TODO how to detect conflict of datapool?
      QListWidgetItem * pc_Item = new QListWidgetItem(NULL, static_cast<sintn>(QListWidgetItem::ItemType::UserType));
      //lint -e{429}  no memory leak because of the parent of pc_ItemWidget and the Qt memory management
      C_SdNdeDataPoolSelectorItemWidget * pc_ItemWidget = new C_SdNdeDataPoolSelectorItemWidget(
         this->mq_UsageViewActive, this);
      const sintn sn_Number = this->count() + 1;

      pc_ItemWidget->SetNumber(sn_Number);
      pc_ItemWidget->SetData(c_OSCDataPool);
      pc_ItemWidget->SetMaximized(this->mq_Maximized);
      connect(pc_ItemWidget, &C_SdNdeDataPoolSelectorItemWidget::SigUpdateErrorToolTip, this,
              &C_SdNdeDataPoolSelectorListWidget::m_UpdateItemErrorToolTip);
      connect(pc_ItemWidget, &C_SdNdeDataPoolSelectorItemWidget::SigHideOtherToolTips, this,
              &C_SdNdeDataPoolSelectorListWidget::SigHideOtherToolTips);

      pc_Item->setSizeHint(pc_ItemWidget->size());

      if (this->mq_Maximized == true)
      {
         this->m_AdaptSize(C_SdNdeDataPoolSelectorItemWidget::hc_MaximumSize);
      }
      else
      {
         this->m_AdaptSize(C_SdNdeDataPoolSelectorItemWidget::hc_MinimumSize);
      }

      this->addItem(pc_Item);
      this->setItemWidget(pc_Item, pc_ItemWidget);

      this->m_UpdateCounters();

      Q_EMIT this->SigListChanged();
      //lint -e{429}  no memory leak because of the parent of pc_Item and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates a datapool widget in the list

   \param[in] oru32_DataPoolIndex        Data pool index
   \param[in] ou32_DataPoolWidgetIndex   Data pool widget index

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range

   \created     16.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_UpdateDataPoolWidget(const uint32 ou32_DataPoolIndex,
                                                               const sintn osn_DataPoolWidgetIndex) const
{
   C_OSCNodeDataPool c_OSCDataPool;
   C_PuiSdNodeDataPool c_UIDataPool;
   sint32 s32_Return;

   s32_Return = C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex, c_OSCDataPool,
                                                             c_UIDataPool);

   if (s32_Return == C_NO_ERR)
   {
      C_SdNdeDataPoolSelectorItemWidget * pc_WidgetItem;
      QListWidgetItem * pc_Item;

      // update the widget
      pc_Item = this->item(osn_DataPoolWidgetIndex);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->SetData(c_OSCDataPool);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the sub widgets inactive

   Adapts the stylesheet of the items

   \param[in]  oq_Active   Flag if widget is active or not

   \created     06.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::SetActive(const bool oq_Active)
{
   QListWidgetItem * pc_Item;
   C_SdNdeDataPoolSelectorItemWidget * pc_WidgetItem;
   sintn sn_Counter;

   if (oq_Active == false)
   {
      // deactivate all items
      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         pc_Item = this->item(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            // ask before to improve performance
            if (pc_WidgetItem->GetActive() == true)
            {
               pc_WidgetItem->SetActive(false);
            }
         }
      }
      this->mq_ItemActive = false;
   }
   else if (this->mq_ItemActive == false) // only if no item is already active
   {
      sintn sn_CurrentRow = currentRow();
      if (sn_CurrentRow < 0)
      {
         sn_CurrentRow = 0;
      }

      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         // activate the first item and deactivate all other items
         pc_Item = this->item(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            if (sn_Counter == sn_CurrentRow)
            {
               pc_WidgetItem->SetActive(true);
               this->mq_ItemActive = true;
            }
            else
            {
               // ask before to improve performance
               if (pc_WidgetItem->GetActive() == true)
               {
                  pc_WidgetItem->SetActive(false);
               }
            }
         }
      }
   }
   else
   {
      // nothing to do
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the widget in maximized or minimized mode

   \param[in]     oq_Maximized   Flag for maximized

   \created     07.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::SetMaximized(const bool oq_Maximized)
{
   sintn sn_Counter;

   this->mq_Maximized = oq_Maximized;

   // adapt all widgets in the items
   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDataPoolSelectorItemWidget * const pc_WidgetItem =
         dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->SetMaximized(this->mq_Maximized);
      }
   }

   // adapt the list itself
   if (this->mq_Maximized == true)
   {
      this->m_AdaptSize(C_SdNdeDataPoolSelectorItemWidget::hc_MaximumSize);
   }
   else
   {
      this->m_AdaptSize(C_SdNdeDataPoolSelectorItemWidget::hc_MinimumSize);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new datapool by opening the datapool properties datapool

   \created     16.02.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::AddNewDatapool(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      // is enough space available
      if (C_SdUtil::h_CheckDatapoolNumber(this->mu32_NodeIndex, this->me_DataPoolType, false, this) == true)
      {
         C_OSCNodeDataPool c_NewDatapool;
         C_PuiSdNodeDataPool c_UIDataPool;

         // the type must be initialized
         c_NewDatapool.c_Name = C_PuiSdHandler::h_GetInstance()->GetUniqueDataPoolName(this->mu32_NodeIndex,
                                                                                       c_NewDatapool.c_Name);
         c_NewDatapool.e_Type = this->me_DataPoolType;
         if (this->m_OpenDataPoolDialog(c_NewDatapool, c_UIDataPool, pc_Node->pc_DeviceDefinition->q_ProgrammingSupport,
                                        -1) == true)
         {
            //Initialize (after type is certain)
            for (uint32 u32_ItList = 0; u32_ItList < c_NewDatapool.c_Lists.size(); ++u32_ItList)
            {
               C_OSCNodeDataPoolList & rc_List = c_NewDatapool.c_Lists[u32_ItList];
               for (uint32 u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size(); ++u32_ItElement)
               {
                  C_PuiSdHandler::h_InitDataElement(c_NewDatapool.e_Type, c_NewDatapool.q_IsSafety,
                                                    rc_List.c_Elements[u32_ItElement]);
               }
               if ((c_NewDatapool.e_Type == C_OSCNodeDataPool::eNVM) && (c_NewDatapool.q_IsSafety == true))
               {
                  rc_List.q_NvMCRCActive = true;
               }
            }
            //Add step
            this->m_AddNewDataPool(c_NewDatapool, c_UIDataPool, -1, false, true);
            Q_EMIT this->SigDataPoolChanged();
            //Check
            Q_EMIT this->SigErrorCheck();
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste list item

   \created     20.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::Paste(void)
{
   sint32 s32_LastIndex = 0;
   C_OSCNodeDataPool c_OSCContent;
   C_PuiSdNodeDataPool c_UIContent;
   sint32 s32_Return;

   if (C_SdUtil::h_CheckDatapoolNumber(this->mu32_NodeIndex, this->me_DataPoolType, false, this) == true)
   {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      // load from "clipboard"
      s32_Return = C_SdClipBoardHelper::h_LoadToDataPool(c_OSCContent, c_UIContent);

      //Check and replace application if necessary
      if (c_OSCContent.s32_RelatedDataBlockIndex >= 0)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_Node != NULL)
         {
            bool q_ReplaceNecessary = false;
            //Check if current application index is valid
            if (static_cast<uint32>(c_OSCContent.s32_RelatedDataBlockIndex) < pc_Node->c_Applications.size())
            {
               const C_OSCNodeApplication & rc_Application =
                  pc_Node->c_Applications[static_cast<uint32>(c_OSCContent.s32_RelatedDataBlockIndex)];
               if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  q_ReplaceNecessary = true;
               }
            }
            else
            {
               q_ReplaceNecessary = true;
            }
            //Replace
            if (q_ReplaceNecessary == true)
            {
               bool q_Found = false;
               for (uint32 u32_ItApp = 0; u32_ItApp < pc_Node->c_Applications.size(); ++u32_ItApp)
               {
                  const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApp];
                  if (rc_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
                  {
                     c_OSCContent.s32_RelatedDataBlockIndex = static_cast<sint32>(u32_ItApp);
                     q_Found = true;
                     break;
                  }
               }
               if (q_Found == false)
               {
                  //Should not really happen
                  c_OSCContent.s32_RelatedDataBlockIndex = -1;
               }
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         if (c_OSCContent.e_Type == this->me_DataPoolType)
         {
            if (this->currentIndex().isValid() == true)
            {
               //Add after current index
               s32_LastIndex = static_cast<sint32>(this->currentRow()) + 1;
            }
            else
            {
               //Add at end
               s32_LastIndex = this->count();
            }

            this->m_AddNewDataPool(c_OSCContent, c_UIContent, s32_LastIndex, true, false);
            Q_EMIT this->SigDataPoolChanged();
            //Check error
            Q_EMIT this->SigErrorCheck();
            QApplication::restoreOverrideCursor();
         }
         else
         {
            C_OgeWiCustomMessage c_MessageBox(this);
            QApplication::restoreOverrideCursor();
            c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                           "Cannot paste Datapool. Datapool type is not compatible."));
            c_MessageBox.Execute();
         }
      }
      else
      {
         QApplication::restoreOverrideCursor();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks all datapools for conflicts and sets the icon

   \return
   true   Conflict found
   false  No conflict found

   \created     27.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorListWidget::CheckDataPoolsForConflict(std::vector<uint32> * const opc_InvalidDatapoolIndices)
const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   bool q_Return = false;

   if (pc_Node != NULL)
   {
      uint32 u32_DpCounter;
      sintn sn_TypeCounter = 0;

      // check all datapools of the lists
      for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
      {
         if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
         {
            // conflict detected, update the widget
            C_SdNdeDataPoolSelectorItemWidget * pc_WidgetItem;
            QListWidgetItem * pc_Item;

            // update the widget
            pc_Item = this->item(sn_TypeCounter);
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            pc_WidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
            if (pc_WidgetItem != NULL)
            {
               bool q_NameConflict;
               bool q_NameInvalid;
               bool q_IsErrorInListOrMessage;
               pc_Node->CheckErrorDataPool(u32_DpCounter, &q_NameConflict, &q_NameInvalid, &q_IsErrorInListOrMessage,
                                           NULL);
               if (((q_NameConflict == false) && (q_NameInvalid == false)) && (q_IsErrorInListOrMessage == false))
               {
                  pc_WidgetItem->SetStateConflict(false);
               }
               else
               {
                  pc_WidgetItem->SetStateConflict(true);
                  q_Return = true;
                  if (opc_InvalidDatapoolIndices != NULL)
                  {
                     opc_InvalidDatapoolIndices->push_back(u32_DpCounter);
                  }
               }
            }

            ++sn_TypeCounter;
         }
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the datapool with a specific index active

   \param[in]  ou32_DataPoolIndex   Real datapool index

   \return
   true     datapool found with this index
   false    no datapool found with this index

   \created     dd.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorListWidget::SetDataPoolActive(const uint32 ou32_DataPoolIndex)
{
   sintn sn_Counter;
   sint32 s32_DpIndex;
   bool q_Return = false;

   // search the index
   for (sn_Counter = 0U; sn_Counter < this->count(); ++sn_Counter)
   {
      s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                      this->me_DataPoolType,
                                                                      static_cast<uint32>(sn_Counter));
      if ((s32_DpIndex >= 0) &&
          (static_cast<uint32>(s32_DpIndex) == ou32_DataPoolIndex))
      {
         // datapool found
         if (this->item(sn_Counter)->isSelected() == false)
         {
            this->setCurrentRow(sn_Counter);
            this->ScrollToItem(sn_Counter);
            Q_EMIT this->SigListChanged();
         }
         q_Return = true;
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overridden paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QListWidget::paintEvent(opc_Event);

   if (this->count() == 0)
   {
      QPainter c_Painter(this->viewport());
      // show a tip for adding a datapool
      QPen c_Pen = c_Painter.pen();
      QFont c_Font;
      QString c_Text;
      if (this->mq_InteractionAvailable == true)
      {
         c_Text = QString(C_GtGetText::h_GetText("No")) + QString(" ") +
                  C_PuiSdUtil::h_ConvertDataPoolTypeToString(this->me_DataPoolType) +
                  QString(C_GtGetText::h_GetText(
                             " Datapool is declared.\nYou may add any via the '+' button."));
      }
      else
      {
         c_Text =
            QString(C_GtGetText::h_GetText("No programming support on this type of node.\n"
                                           "Datapools cannot be added."));
      }

      // configure color
      c_Pen.setColor(mc_STYLE_GUIDE_COLOR_8);
      c_Painter.setPen(c_Pen);

      // configure font
      c_Font = mc_STYLE_GUIDE_FONT_REGULAR_13;
      c_Font.setPixelSize(c_Font.pointSize());
      c_Painter.setFont(c_Font);

      c_Painter.drawText(this->rect(), static_cast<sintn>(Qt::AlignCenter), c_Text);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event

   Updates the information about the count of columns and items per row

   \param[in,out] opc_Event Event identification and information

   \created     01.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   this->m_UpdateCounters();

   Q_EMIT this->SigListChanged();

   QListWidget::resizeEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: handle list actions

   \param[in,out] opc_Event Event identification and information

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   const sintn sn_Row = this->currentRow();
   bool q_CallOrig = true;

   if (this->mq_InteractionAvailable == true)
   {
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         switch (opc_Event->key())
         {
         case Qt::Key_C:
            m_Copy();
            break;
         case Qt::Key_X:
            m_Cut();
            break;
         case Qt::Key_Left:
            q_CallOrig = false;
            m_MoveLeft();
            // scrolling
            if (((sn_Row % this->msn_ItemsPerLine) == 0) &&
                (this->msn_ActualLine >= 0))
            {
               // change of line not necessary, but scrolling to the line is necessary
               this->SetActualLine(this->GetActualLine(), false);
            }
            break;
         case Qt::Key_Right:
            q_CallOrig = false;
            m_MoveRight();
            // scrolling
            if (((sn_Row % this->msn_ItemsPerLine) == (this->msn_ItemsPerLine - 1)) &&
                (this->msn_ActualLine < this->msn_CountLines))
            {
               // change of line not necessary, but scrolling to the line is necessary
               this->SetActualLine(this->GetActualLine(), false);
            }
            break;
         default:
            //Nothing to do
            break;
         }
      }
      else
      {
         switch (opc_Event->key())
         {
         case Qt::Key_Delete:
            m_Delete(true);
            break;
         case Qt::Key_Left:
            if (((sn_Row % this->msn_ItemsPerLine) == 0) &&
                (this->msn_ActualLine > 0))
            {
               this->SetActualLine(this->GetActualLine() - 1, false);
               this->setCurrentRow(sn_Row - 1);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_Right:
            if (((sn_Row % this->msn_ItemsPerLine) == (this->msn_ItemsPerLine - 1)) &&
                (this->msn_ActualLine < (this->msn_CountLines - 1)))
            {
               this->SetActualLine(this->GetActualLine() + 1);
               q_CallOrig = false;
            }
            break;
         case Qt::Key_F2:
            this->m_Edit(true);
            q_CallOrig = false;
            break;
         default:
            //Nothing to do
            break;
         }
      }
   }
   else
   {
      if (opc_Event->key() == static_cast<sintn>(Qt::Key_F2))
      {
         this->m_Edit(true);
         q_CallOrig = false;
      }
   }

   if (q_CallOrig == true)
   {
      QListWidget::keyPressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overridden focus in event

   \param[in,out] opc_Event  Pointer to paint event

   \created     06.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::focusInEvent(QFocusEvent * const opc_Event)
{
   QListWidget::focusInEvent(opc_Event);

   this->SetActive(true);

   if ((this->currentRow() < 0) &&
       (this->count() > 0))
   {
      this->setCurrentRow(0);
   }

   Q_EMIT this->SigWidgetFocused(this->currentRow());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse double click event slot

   Here: Manage double click for current item

   \param[in,out] opc_Event Event identification and information

   \created     28.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QListWidgetItem * const pc_Item = this->itemAt(opc_Event->pos());

   QListWidget::mouseDoubleClickEvent(opc_Event);
   if (pc_Item != NULL)
   {
      m_ItemDoubleClicked(pc_Item);
   }
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_MoveItem(const sintn osn_SourceIndex, const sintn osn_TargetIndex)
{
   this->m_MoveDatapool(osn_SourceIndex, osn_TargetIndex, false);
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_UpdateNumbers(void) const
{
   sintn sn_Counter;

   // Update all visible numbers of the concrete widgets
   for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
   {
      QListWidgetItem * const pc_Item = this->item(sn_Counter);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_SdNdeDataPoolSelectorItemWidget * const pc_WidgetItem =
         dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
      if (pc_WidgetItem != NULL)
      {
         pc_WidgetItem->SetNumber(static_cast<uint32>(sn_Counter + 1));
      }
   }
}
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_DelegateStartPaint(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDataPoolSelectorItemWidget * pc_ItemWidget =
      dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(this->item(this->currentIndex().row())));

   this->mc_Delegate.StartPaint(this->currentIndex().row(), pc_ItemWidget);
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_DelegateStopPaint(void)
{
   this->mc_Delegate.StopPaint();
}

//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolSelectorListWidget::m_OpenDataPoolDialog(C_OSCNodeDataPool & orc_OSCDataPool,
                                                             C_PuiSdNodeDataPool & orc_UiDataPool,
                                                             const bool oq_ShowApplicationSection,
                                                             const sint32 os32_DataPoolIndex, const bool oq_SelectName)
{
   bool q_Return = false;

   QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SdNdeDatapoolProperties * const pc_Dialog = new C_SdNdeDatapoolProperties(*c_New, "DATAPOOL", &orc_OSCDataPool,
                                                                               &orc_UiDataPool,
                                                                               &this->me_ProtocolType,
                                                                               this->me_DataPoolType,
                                                                               os32_DataPoolIndex,
                                                                               this->mu32_NodeIndex,
                                                                               oq_SelectName,
                                                                               oq_ShowApplicationSection);

   Q_UNUSED(pc_Dialog)

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      q_Return = true;
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }

   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   return q_Return;
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_AddNewDataPool(const C_OSCNodeDataPool & orc_OSCDataPool,
                                                         const C_PuiSdNodeDataPool & orc_UIDataPool,
                                                         const sint32 os32_DataPoolIndex,
                                                         const bool oq_AllowNameAdaptation,
                                                         const bool oq_AllowDataAdaptation)
{
   sint32 s32_Return;
   sintn sn_Row = os32_DataPoolIndex;
   sintn sn_RealIndex;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if (os32_DataPoolIndex == -1)
   {
      s32_Return = C_PuiSdHandler::h_GetInstance()->AddDataPool(this->mu32_NodeIndex, orc_OSCDataPool, orc_UIDataPool,
                                                                this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                oq_AllowDataAdaptation);
      // the new datapool will be at the end
      sn_Row = this->count();
   }
   else if (os32_DataPoolIndex >= 0)
   {
      sn_RealIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex, this->me_DataPoolType,
                                                                       os32_DataPoolIndex);
      if (sn_RealIndex >= 0)
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->InsertDataPool(this->mu32_NodeIndex,
                                                                      static_cast<uint32>(sn_RealIndex),
                                                                      orc_OSCDataPool, orc_UIDataPool,
                                                                      this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                      oq_AllowDataAdaptation);
      }
      else
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->AddDataPool(this->mu32_NodeIndex, orc_OSCDataPool,
                                                                   orc_UIDataPool,
                                                                   this->me_ProtocolType, oq_AllowNameAdaptation,
                                                                   oq_AllowDataAdaptation);
         // the new datapool will be at the end
         sn_Row = this->count();
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }

   // update the UI
   if (s32_Return == C_NO_ERR)
   {
      this->m_InitFromData();
      this->setCurrentRow(sn_Row);
      this->m_UpdateCounters();

      // scroll to the position and set the item active
      this->ScrollToItem(sn_Row);

      Q_EMIT this->SigListChanged();
   }
   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     03.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   if (this->mq_InteractionAvailable == true)
   {
      const QModelIndex c_Current = this->currentIndex();
      QListWidgetItem * const pc_Item = this->itemAt(orc_Pos);

      // add action shall be shown only if no item concrete was clicked
      if ((c_Current.isValid() == true) && (c_Current.parent().isValid() == false) && (pc_Item != NULL))
      {
         this->mpc_AddAction->setVisible(false);

         this->mpc_EditAction->setVisible(true);
         this->mpc_EditActionSeparator->setVisible(true);
         this->mpc_DeleteAction->setVisible(true);
         this->mpc_DeleteActionSeparator->setVisible(true);
         this->mpc_CopyAction->setVisible(true);
         this->mpc_CutAction->setVisible(true);
         this->mpc_PasteAction->setVisible(true);
         this->mpc_MoveLeftAction->setVisible(true);
         this->mpc_MoveRightAction->setVisible(true);
         this->mpc_MoveActionSeparator->setVisible(true);

         // actions depends on count of datapools
         if (this->count() > 1)
         {
            this->mpc_MoveLeftAction->setEnabled(this->currentRow() > 0);
            this->mpc_MoveRightAction->setEnabled(this->currentRow() < (this->count() - 1));
            this->mpc_MoveActionSeparator->setEnabled(true);
         }
         else
         {
            this->mpc_MoveLeftAction->setEnabled(false);
            this->mpc_MoveRightAction->setEnabled(false);
            this->mpc_MoveActionSeparator->setEnabled(false);
         }

         // actions depend on datapool type
         if (this->me_DataPoolType == C_OSCNodeDataPool::eCOM)
         {
            this->mpc_CopyAction->setEnabled(false);
            this->mpc_CutAction->setEnabled(false);
            this->mpc_PasteAction->setEnabled(false);
         }
         else
         {
            this->mpc_CopyAction->setEnabled(true);
            this->mpc_CutAction->setEnabled(true);
            this->mpc_PasteAction->setEnabled(true);
         }
      }
      else
      {
         this->mpc_AddAction->setVisible(true);
         this->mpc_EditActionSeparator->setVisible(true);
         this->mpc_PasteAction->setVisible(true);

         this->mpc_EditAction->setVisible(false);
         this->mpc_DeleteAction->setVisible(false);
         this->mpc_DeleteActionSeparator->setVisible(false);
         this->mpc_CopyAction->setVisible(false);
         this->mpc_CutAction->setVisible(false);
         this->mpc_MoveLeftAction->setVisible(false);
         this->mpc_MoveRightAction->setVisible(false);
         this->mpc_MoveActionSeparator->setVisible(false);
      }

      this->mpc_ContextMenu->popup(this->mapToGlobal(orc_Pos));
   }
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_AddAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Add new Datapool"), this,
                                                          &C_SdNdeDataPoolSelectorListWidget::AddNewDatapool,
                                                          static_cast<sintn>(Qt::CTRL) +
                                                          static_cast<sintn>(Qt::Key_Plus));

   this->mpc_EditAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Edit Properties"), this,
                                                           &C_SdNdeDataPoolSelectorListWidget::m_Edit);

   this->mpc_EditActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_CutAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Cut"), this, &C_SdNdeDataPoolSelectorListWidget::m_Cut,
                                                          static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_CopyAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Copy"), this, &C_SdNdeDataPoolSelectorListWidget::m_Copy,
                                                           static_cast<sintn>(Qt::CTRL) +
                                                           static_cast<sintn>(Qt::Key_C));
   this->mpc_PasteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                               "Paste"), this, &C_SdNdeDataPoolSelectorListWidget::Paste,
                                                            static_cast<sintn>(Qt::CTRL) +
                                                            static_cast<sintn>(Qt::Key_V));

   this->mpc_MoveActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_MoveLeftAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Left"), this,
                                                               &C_SdNdeDataPoolSelectorListWidget::m_MoveLeft,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Left));
   this->mpc_MoveRightAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                   "Move Right"), this,
                                                                &C_SdNdeDataPoolSelectorListWidget::m_MoveRight,
                                                                static_cast<sintn>(Qt::CTRL) +
                                                                static_cast<sintn>(Qt::Key_Right));

   this->mpc_DeleteActionSeparator = this->mpc_ContextMenu->addSeparator();

   this->mpc_DeleteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Delete"), this, &C_SdNdeDataPoolSelectorListWidget::m_DeleteSlot,
                                                             static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, &C_SdNdeDataPoolSelectorListWidget::customContextMenuRequested, this,
           &C_SdNdeDataPoolSelectorListWidget::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_Edit(const bool oq_SelectName)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      C_OSCNodeDataPool c_OSCDatapool;
      C_PuiSdNodeDataPool c_UIDataPool;

      // get the real datapool index
      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->currentRow());

      if (s32_DpIndex >= 0)
      {
         // fill the structure
         if (C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, s32_DpIndex,
                                                          c_OSCDatapool, c_UIDataPool) == C_NO_ERR)
         {
            // open the dialog
            if (this->m_OpenDataPoolDialog(c_OSCDatapool, c_UIDataPool,
                                           pc_Node->pc_DeviceDefinition->q_ProgrammingSupport, s32_DpIndex,
                                           oq_SelectName) == true)
            {
               //Update crc active if safety
               if ((c_OSCDatapool.e_Type == C_OSCNodeDataPool::eNVM) && (c_OSCDatapool.q_IsSafety == true))
               {
                  for (uint32 u32_ItList = 0; u32_ItList < c_OSCDatapool.c_Lists.size(); ++u32_ItList)
                  {
                     C_OSCNodeDataPoolList & rc_List = c_OSCDatapool.c_Lists[u32_ItList];
                     rc_List.q_NvMCRCActive = true;
                  }
               }
               // save the changes
               if (C_PuiSdHandler::h_GetInstance()->SetDataPool(this->mu32_NodeIndex, s32_DpIndex,
                                                                c_OSCDatapool, c_UIDataPool, true,
                                                                this->me_ProtocolType) == C_NO_ERR)
               {
                  this->m_UpdateDataPoolWidget(s32_DpIndex, this->currentRow());
                  Q_EMIT this->SigDataPoolChanged();
                  Q_EMIT this->SigWidgetFocused(this->currentRow());
                  //Update may be necessary for CRC option
                  if (c_OSCDatapool.e_Type == C_OSCNodeDataPool::eNVM)
                  {
                     Q_EMIT this->SigUpdateLists(this->mu32_NodeIndex, s32_DpIndex);
                  }

                  //Check
                  Q_EMIT this->SigErrorCheck();
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy list item to clipboard

   \created     20.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_Copy(void) const
{
   if (this->me_DataPoolType != C_OSCNodeDataPool::eCOM)
   {
      C_OSCNodeDataPool c_OSCContent;
      C_PuiSdNodeDataPool c_UIContent;

      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->currentRow());

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if (s32_DpIndex >= 0)
      {
         if (C_PuiSdHandler::h_GetInstance()->GetDataPool(this->mu32_NodeIndex, s32_DpIndex,
                                                          c_OSCContent, c_UIContent) == C_NO_ERR)
         {
            C_SdClipBoardHelper::h_StoreDataPool(c_OSCContent, c_UIContent);
         }
      }

      QApplication::restoreOverrideCursor();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy tree item to clipboard and delete it afterwards

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_Cut(void)
{
   m_Copy();
   m_Delete(false);
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_DeleteSlot(void)
{
   this->m_Delete(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete datapool list item

   \param   oq_AskUser     Flag for asking user for continuing the deletion

   \created     17.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_Delete(const bool oq_AskUser)
{
   bool q_Continue = true;

   if (oq_AskUser == true)
   {
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool delete"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the selected Datapool(s)?"));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox != C_OgeWiCustomMessage::eYES)
      {
         q_Continue = false;
      }
   }

   if (q_Continue == true)
   {
      sint32 s32_Return;
      const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                   this->me_DataPoolType,
                                                                                   this->currentRow());

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if (s32_DpIndex >= 0)
      {
         s32_Return = C_PuiSdHandler::h_GetInstance()->RemoveDataPool(this->mu32_NodeIndex, s32_DpIndex);
         if (s32_Return == C_NO_ERR)
         {
            this->m_InitFromData();
            if (this->count() > 0)
            {
               this->setCurrentRow(0);
            }
            else
            {
               Q_EMIT this->SigNoDataPoolSelected();
            }
            Q_EMIT this->SigListChanged();
            Q_EMIT this->SigDataPoolChanged();
            //Check
            Q_EMIT this->SigErrorCheck();
         }
      }

      QApplication::restoreOverrideCursor();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move selected item left by one slot

   \created     17.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_MoveLeft(void)
{
   sintn sn_Index = this->currentRow();

   if (sn_Index > 0)
   {
      this->m_MoveDatapool(sn_Index, (sn_Index - 1));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move selected item right by one slot

   \created     17.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_MoveRight(void)
{
   sintn sn_Index = this->currentRow();

   if (sn_Index < (this->count() - 1))
   {
      this->m_MoveDatapool(sn_Index, (sn_Index + 1));
   }
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_ItemDoubleClicked(QListWidgetItem * const opc_Item)
{
   this->setCurrentItem(opc_Item);
   this->m_Edit();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_MoveDatapool(const sintn osn_SourceIndex, const sintn osn_TargetIndex,
                                                       const bool oq_Reinit)
{
   const sint32 s32_SourceDpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                      this->me_DataPoolType,
                                                                                      osn_SourceIndex);
   const sint32 s32_TargetDpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                      this->me_DataPoolType,
                                                                                      osn_TargetIndex);

   QApplication::setOverrideCursor(Qt::WaitCursor);

   // check for index errors
   if ((s32_SourceDpIndex >= 0) &&
       (s32_TargetDpIndex >= 0))
   {
      C_PuiSdHandler::h_GetInstance()->MoveDataPool(this->mu32_NodeIndex, s32_SourceDpIndex, s32_TargetDpIndex);
      if (oq_Reinit == true)
      {
         // reload data. swapping in the list is not possible without loosing the widget
         this->m_InitFromData(true);
      }
      this->setCurrentRow(osn_TargetIndex);
      Q_EMIT this->SigListChanged();
      Q_EMIT this->SigDataPoolChanged();
   }

   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_InitFromData(const bool oq_Update)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if (pc_Node != NULL)
   {
      uint32 u32_DpCounter;
      sintn sn_TypeCounter = 0;

      if (oq_Update == false)
      {
         this->clear();
      }

      // add all datapools to the lists
      for (u32_DpCounter = 0; u32_DpCounter < pc_Node->c_DataPools.size(); ++u32_DpCounter)
      {
         if (pc_Node->c_DataPools[u32_DpCounter].e_Type == this->me_DataPoolType)
         {
            if (oq_Update == false)
            {
               // add new
               this->m_AddDataPoolWidget(u32_DpCounter);
            }
            else
            {
               // update all widgets
               this->m_UpdateDataPoolWidget(u32_DpCounter, sn_TypeCounter);
            }
            ++sn_TypeCounter;
         }
      }
   }

   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_SelectionChanged(void)
{
   const QList<QListWidgetItem *> & rc_Items = this->selectedItems();

   if (rc_Items.count() > 0)
   {
      C_SdNdeDataPoolSelectorItemWidget * pc_SelectedWidgetItem;
      sintn sn_Counter;
      //Handle force change
      bool q_ForceChange;
      if (this->ms32_LastKnownDpCount == this->count())
      {
         q_ForceChange = false;
      }
      else
      {
         q_ForceChange = true;
      }
      this->ms32_LastKnownDpCount = this->count();

      QApplication::setOverrideCursor(Qt::WaitCursor);

      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_SelectedWidgetItem = dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(rc_Items[0]));

      Q_EMIT this->SigWidgetFocused(this->currentRow(), q_ForceChange);

      for (sn_Counter = 0; sn_Counter < this->count(); ++sn_Counter)
      {
         // search the item which was clicked and adapt the stylesheet
         QListWidgetItem * const pc_Item = this->item(sn_Counter);
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_SdNdeDataPoolSelectorItemWidget * const pc_WidgetItem =
            dynamic_cast<C_SdNdeDataPoolSelectorItemWidget *>(this->itemWidget(pc_Item));
         if (pc_WidgetItem != NULL)
         {
            if (pc_WidgetItem == pc_SelectedWidgetItem)
            {
               pc_WidgetItem->SetActive(true);
            }
            else
            {
               // deactivate all other items if they are active
               // ask before to improve performance
               if (pc_WidgetItem->GetActive() == true)
               {
                  pc_WidgetItem->SetActive(false);
               }
            }
         }
      }

      // update the visible "line"
      this->msn_ActualLine = this->currentRow() / this->msn_ItemsPerLine;

      QApplication::restoreOverrideCursor();
   }
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_AdaptSize(const QSize & orc_WidgetSize)
{
   this->setMinimumHeight(orc_WidgetSize.height());
   this->setMaximumHeight(orc_WidgetSize.height());
   this->setMinimumWidth(orc_WidgetSize.width() + 10);
   this->doItemsLayout();
}

//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_UpdateCounters()
{
   // Items on each row
   this->msn_ItemsPerLine = this->width() / C_SdNdeDataPoolSelectorItemWidget::hc_MaximumSize.width();

   if (this->msn_ItemsPerLine < 1)
   {
      this->msn_ItemsPerLine = 1;
   }

   // calculate the necessary rows
   this->msn_CountLines = this->count() / this->msn_ItemsPerLine;

   // correct rounding error
   if ((this->count() % this->msn_ItemsPerLine) > 0)
   {
      ++this->msn_CountLines;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger live update of error tooltip

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolSelectorListWidget::m_UpdateItemErrorToolTip(void) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDataPoolSelectorItemWidget * const pc_Sender =
      dynamic_cast<C_SdNdeDataPoolSelectorItemWidget * const>(this->sender());

   if (pc_Sender != NULL)
   {
      //Search
      uint32 u32_FoundIndex = 0UL;
      bool q_Found = false;
      for (sintn sn_ItDp = 0; sn_ItDp < this->count(); ++sn_ItDp)
      {
         if (this->sender() == this->itemWidget(this->item(sn_ItDp)))
         {
            q_Found = true;
            u32_FoundIndex = static_cast<uint32>(sn_ItDp);
         }
      }
      if (q_Found == true)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         const sint32 s32_DpIndex = C_PuiSdHandler::h_GetInstance()->GetDataPoolIndex(this->mu32_NodeIndex,
                                                                                      this->me_DataPoolType,
                                                                                      u32_FoundIndex);
         if ((s32_DpIndex >= 0) && (pc_Node != NULL))
         {
            const uint32 u32_DataPoolIndex = static_cast<uint32>(s32_DpIndex);
            bool q_NameConflict;
            bool q_NameInvalid;
            bool q_IsErrorInListOrMessage;
            std::vector<uint32> c_InvalidListIndices;
            pc_Node->CheckErrorDataPool(u32_DataPoolIndex, &q_NameConflict, &q_NameInvalid, &q_IsErrorInListOrMessage,
                                        &c_InvalidListIndices);
            if (((q_NameConflict == false) && (q_NameInvalid == false)) && (q_IsErrorInListOrMessage == false))
            {
               pc_Sender->SetErrorToolTip(C_GtGetText::h_GetText("No error found!"), "");
            }
            else
            {
               const QString c_Heading = C_GtGetText::h_GetText("Datapool has invalid content");
               QString c_Content;
               if ((q_NameConflict == true) || (q_NameInvalid == true))
               {
                  c_Content += C_GtGetText::h_GetText("Invalid properties:\n");
                  if (q_NameConflict == true)
                  {
                     c_Content += C_GtGetText::h_GetText("Duplicate datapool name detected.\n");
                  }
                  if (q_NameInvalid == true)
                  {
                     c_Content += C_GtGetText::h_GetText("Datapool name is empty or contains invalid characters.\n");
                  }
                  c_Content += "\n";
               }
               if (q_IsErrorInListOrMessage == true)
               {
                  c_Content += C_GtGetText::h_GetText("Lists:\n");
                  for (uint32 u32_ItList = 0;
                       (u32_ItList < c_InvalidListIndices.size()) &&
                       (u32_ItList < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                       ++u32_ItList)
                  {
                     const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                        this->mu32_NodeIndex, u32_DataPoolIndex, c_InvalidListIndices[u32_ItList]);
                     if (pc_List != NULL)
                     {
                        c_Content += QString("%1\n").arg(pc_List->c_Name.c_str());
                     }
                  }
                  if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidListIndices.size())
                  {
                     c_Content += QString("+%1\n").arg(
                        c_InvalidListIndices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
                  }
                  c_Content += "\n";
               }
               pc_Sender->SetErrorToolTip(c_Heading, c_Content);
            }
         }
      }
   }
}

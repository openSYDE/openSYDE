//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list elements table item (implementation)

   Data pool list elements table item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QHeaderView>
#include <QMimeData>
#include <QDrag>
#include <QScrollBar>
#include <QPointer>

#include "C_SdNdeDataPoolListTableView.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_SdClipBoardHelper.h"
#include "C_Uti.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeSingleHeaderView.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDataPoolListArrayEditWidget.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_SdUtil.h"
#include "C_OgeWiCustomMessage.h"
#include "C_UsHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_elements;
using namespace stw_errors;
using namespace stw_tgl;

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
C_SdNdeDataPoolListTableView::C_SdNdeDataPoolListTableView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0),
   ms32_LastIndicatorSize(-1),
   mc_UndoManager(),
   mc_Delegate(),
   mpc_ContextMenu(NULL),
   mpc_ModelViewManager(NULL),
   mpc_ActionMoveUp(NULL),
   mpc_ActionMoveDown(NULL),
   mpc_ActionAdd(NULL),
   mpc_ActionCut(NULL),
   mpc_ActionCopy(NULL),
   mpc_ActionPaste(NULL),
   mpc_ActionDelete(NULL),
   mq_AllowMoveUp(true),
   mq_AllowMoveDown(true),
   mq_AllowAdd(true)
{
   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
   this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(
      QAbstractItemView::DoubleClicked /* | QAbstractItemView::CurrentChanged*/ | QAbstractItemView::AnyKeyPressed |
      QAbstractItemView::EditKeyPressed);
   //Consider all elements for resize
   this->setHorizontalHeader(new C_SdNdeSingleHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

   //Row Height
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(40);
   this->verticalHeader()->setMaximumSectionSize(40);
   this->verticalHeader()->setDefaultSectionSize(40);

   // Icon
   this->setIconSize(QSize(20, 20));

   //Hide vertical header
   this->verticalHeader()->hide();

   this->setItemDelegate(&mc_Delegate);

   //Custom context menu
   m_SetupContextMenu();

   //Hover event
   this->setMouseTracking(true);

   //Avoid styling table inside
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->verticalScrollBar(), "C_SdNdeDataPoolListTableView", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->horizontalScrollBar(), "C_SdNdeDataPoolListTableView", true);

   //Connects
   connect(&mc_Delegate, &C_SdNdeDataPoolListTableDelegate::SigLinkClicked, this,
           &C_SdNdeDataPoolListTableView::m_HandleLinkClicked);
   connect(this->horizontalHeader(), &QHeaderView::sectionResized, this,
           &C_SdNdeDataPoolListTableView::m_OnColumnResize);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolListTableView::~C_SdNdeDataPoolListTableView(void)
{
   m_HandleColumnStateSave();
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->UnRegisterElementView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                        this->mu32_ListIndex, this);
   }
   //lint -e{1540}  no memory leak because of the parent and the Qt memory management or never took ownership
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SetList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                           const uint32 & oru32_ListIndex)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->UnRegisterElementView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                        this->mu32_ListIndex, this);
   }
   mu32_NodeIndex = oru32_NodeIndex;
   mu32_DataPoolIndex = oru32_DataPoolIndex;
   mu32_ListIndex = oru32_ListIndex;
   Reset();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if equal

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDataPoolListTableView::Equals(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                          const uint32 & oru32_ListIndex) const
{
   bool q_Retval;

   if (((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex)) &&
       (this->mu32_ListIndex == oru32_ListIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy tree item to clipboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Copy(void) const
{
   std::vector<C_OSCNodeDataPoolListElement> c_OSCContentVec;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UIContentVec;
   C_OSCNodeDataPoolListElement c_OSCContent;
   C_PuiSdNodeDataPoolListElement c_UIContent;

   std::vector<uint32> c_SelectedIndices = m_GetSelectedIndices();
   c_OSCContentVec.reserve(c_SelectedIndices.size());
   c_UIContentVec.reserve(c_SelectedIndices.size());

   //Sort to have "correct" copy order
   C_SdUtil::h_SortIndicesAscending(c_SelectedIndices);
   for (uint32 u32_ItIndex = 0; u32_ItIndex < c_SelectedIndices.size(); ++u32_ItIndex)
   {
      if (C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mu32_ListIndex,
                                                                  c_SelectedIndices[u32_ItIndex],
                                                                  c_OSCContent,
                                                                  c_UIContent) == C_NO_ERR)
      {
         c_OSCContentVec.push_back(c_OSCContent);
         c_UIContentVec.push_back(c_UIContent);
      }
   }
   C_SdClipBoardHelper::h_StoreDataPoolListElementsToClipBoard(c_OSCContentVec, c_UIContentVec);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy tree item to clipboard and delete it afterwards
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Cut(void)
{
   Copy();
   Delete();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste tree item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Paste(void)
{
   const uint32 u32_LastIndex = m_GetOneAfterHighestSelected();

   this->mc_UndoManager.DoPaste(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                this->mpc_ModelViewManager,
                                u32_LastIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete tree item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Delete(void)
{
   std::vector<uint32> c_Indices = m_GetSelectedIndices();
   this->mc_UndoManager.DoDeleteElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                         this->mpc_ModelViewManager,
                                         c_Indices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert triggered via action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::InsertAction(void)
{
   Insert(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert tree item

   \param[in] orq_SetFocus Optional flag if inital focus should be set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Insert(const bool & orq_SetFocus)
{
   if (mq_AllowAdd == true)
   {
      std::vector<uint32> c_Indices;
      c_Indices.push_back(m_GetOneAfterHighestSelected());

      this->mc_UndoManager.DoAddElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                         this->mpc_ModelViewManager, c_Indices);
      if (orq_SetFocus == true)
      {
         this->EditInitial();
      }
   }
   else
   {
      QString c_Text;
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex);
      if (pc_DataPool != NULL)
      {
         if (pc_DataPool->e_Type == C_OSCNodeDataPool::eDIAG)
         {
            c_Text = C_GtGetText::h_GetText("Variables");
         }
         else
         {
            c_Text = C_GtGetText::h_GetText("Parameters");
         }
      }
      else
      {
         c_Text = C_GtGetText::h_GetText("Data elements");
      }
      c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("Only %1 %2 allowed per list.")).arg(
                                     mu32_NODE_DATA_POOL_LIST_ELEMENT_MAX).arg(c_Text));
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item up by one slot
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::DoMoveUp(void)
{
   if (this->mq_AllowMoveUp == true)
   {
      bool q_AllowMove = true;

      std::vector<uint32> c_TargetIndices;
      std::vector<uint32> c_SourceIndices = m_GetSelectedIndices();
      c_TargetIndices.resize(c_SourceIndices.size());

      //Sort to have "correct" move order
      C_SdUtil::h_SortIndicesAscending(c_SourceIndices);
      for (uint32 u32_ItItem = 0; u32_ItItem < c_TargetIndices.size(); ++u32_ItItem)
      {
         //Check if not first
         if (c_SourceIndices[u32_ItItem] > 0)
         {
            c_TargetIndices[u32_ItItem] = c_SourceIndices[u32_ItItem] - 1;
         }
         else
         {
            //No move
            q_AllowMove = false;
         }
      }
      if (q_AllowMove == true)
      {
         m_Move(c_SourceIndices, c_TargetIndices);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move selected item down by one slot
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::DoMoveDown(void)
{
   if (this->mq_AllowMoveDown == true)
   {
      if (this->mpc_ModelViewManager != NULL)
      {
         C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_Model != NULL)
         {
            bool q_AllowMove = true;

            std::vector<uint32> c_TargetIndices;
            std::vector<uint32> c_SourceIndices = m_GetSelectedIndices();
            const uint32 u32_LastIndex = static_cast<uint32>(pc_Model->rowCount() - 1);
            c_TargetIndices.resize(c_SourceIndices.size());

            //Sort to have "correct" move order
            C_SdUtil::h_SortIndicesAscending(c_SourceIndices);
            for (uint32 u32_ItItem = 0; u32_ItItem < c_TargetIndices.size(); ++u32_ItItem)
            {
               //Check if not last
               if (c_SourceIndices[u32_ItItem] < u32_LastIndex)
               {
                  c_TargetIndices[u32_ItItem] = c_SourceIndices[u32_ItItem] + 1;
               }
               else
               {
                  //No move
                  q_AllowMove = false;
               }
            }
            if (q_AllowMove == true)
            {
               m_Move(c_SourceIndices, c_TargetIndices);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enter name edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::EditInitial(void)
{
   const std::vector<uint32> c_Selection = this->m_GetSelectedIndices();

   if (c_Selection.size() == 1)
   {
      if (this->mpc_ModelViewManager != NULL)
      {
         C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_Model != NULL)
         {
            this->edit(this->model()->index(c_Selection[0], pc_Model->EnumToColumn(
                                               C_SdNdeDataPoolListTableModel::eNAME)));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set undo stack

   \param[in,out] opc_Value Undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SetUndoStack(C_SdNdeUnoDataPoolManager * const opc_Value)
{
   this->mc_UndoManager.SetUndoStack(opc_Value);
   //Undo
   this->mc_Delegate.SetUndoStack(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::OnErrorChangePossible(void)
{
   Q_EMIT this->SigErrorChangePossible();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::OnSizeChangePossible(void)
{
   m_HandleColumnStateSave();
   Q_EMIT this->SigSizeChangePossible();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]     oru32_NodeIndex                Node index
   \param[in]     oru32_DataPoolIndex            Node data pool index
   \param[in]     oru32_DataPoolListIndex        Node data pool list index
   \param[in,out] opc_DataPoolListTableModel     Data pool lists table model to perform actions on
   \param[in]     oru32_DataPoolListElementIndex Node data pool list element index
   \param[in]     orc_NewData                    New data
   \param[in]     ore_DataChangeType             Data change type
   \param[in]     oru32_ArrayIndex               Optional array index
   \param[in]     ors32_DataSetIndex             Optional data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::OnDataChangeElements(const uint32 & oru32_DataPoolListElementIndex,
                                                        const QVariant & orc_NewData,
                                                        const C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32 & oru32_ArrayIndex,
                                                        const sint32 & ors32_DataSetIndex)
{
   this->mc_UndoManager.DoDataChangeElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                             this->mpc_ModelViewManager, oru32_DataPoolListElementIndex, orc_NewData,
                                             ore_DataChangeType, oru32_ArrayIndex, ors32_DataSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle list paste

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   switch (opc_Event->key())
   {
   case Qt::Key_C:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         Copy();
         opc_Event->accept();
      }
      break;
   case Qt::Key_X:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         Cut();
         opc_Event->accept();
      }
      break;
   case Qt::Key_V:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         Paste();
         opc_Event->accept();
      }
      break;
   case Qt::Key_Delete:
      q_CallOrig = false;
      Delete();
      opc_Event->accept();
      break;
   case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
   case Qt::Key_Plus:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         Insert(false);
         opc_Event->accept();
      }
      break;
   case Qt::Key_Up:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         DoMoveUp();
         opc_Event->accept();
      }
      break;
   case Qt::Key_Down:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         DoMoveDown();
         opc_Event->accept();
      }
      break;
   default:
      //Nothing to do
      break;
   }

   if (q_CallOrig == true)
   {
      QTableView::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Move elements

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::dropEvent(QDropEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SdNdeDataPoolListTableView * const pc_SourceTable =
      dynamic_cast<C_SdNdeDataPoolListTableView * const>(opc_Event->source());

   if (pc_SourceTable != NULL)
   {
      const QMimeData * const pc_MimeData = opc_Event->mimeData();
      if (pc_MimeData != NULL)
      {
         if (this->mpc_ModelViewManager != NULL)
         {
            C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
               this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
            if (pc_Model != NULL)
            {
               if (pc_Model->mimeTypes().size() > 2)
               {
                  if ((pc_MimeData->hasFormat(pc_Model->mimeTypes().at(1)) == true) &&
                      (pc_MimeData->hasFormat(pc_Model->mimeTypes().at(2)) == true))
                  {
                     std::vector<C_OSCNodeDataPoolListElement> c_OSCContent;
                     std::vector<C_PuiSdNodeDataPoolListElement> c_UIContent;
                     const QString c_Content = pc_MimeData->data(pc_Model->mimeTypes().at(1));
                     //Insert indices
                     if (C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromString(c_OSCContent, c_UIContent,
                                                                                     c_Content) == C_NO_ERR)
                     {
                        std::vector<uint32> c_SourceIndices;
                        uint32 u32_TargetRow;
                        const QString c_IndicesString = pc_MimeData->data(pc_Model->mimeTypes().at(2));
                        std::vector<uint32> c_NewIndices;
                        QModelIndex c_Index = this->indexAt(opc_Event->pos());
                        //Target row
                        if (c_Index.isValid())
                        {
                           u32_TargetRow = static_cast<uint32>(c_Index.row()) + 1UL;
                        }
                        else
                        {
                           u32_TargetRow = pc_Model->rowCount();
                        }
                        //Calculate target indices
                        c_NewIndices.resize(c_OSCContent.size());
                        for (uint32 u32_ItNewIndex = 0; u32_ItNewIndex < c_NewIndices.size(); ++u32_ItNewIndex)
                        {
                           c_NewIndices[u32_ItNewIndex] = u32_TargetRow + u32_ItNewIndex;
                        }
                        //Get indices
                        if (C_SdClipBoardHelper::h_LoadIndicesFromString(c_SourceIndices,
                                                                         c_IndicesString) == C_NO_ERR)
                        {
                           if (pc_SourceTable == this)
                           {
                              //Effective move
                              this->mc_UndoManager.DoMoveElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mu32_ListIndex,
                                                                  this->mpc_ModelViewManager,
                                                                  c_SourceIndices,
                                                                  c_NewIndices, true);
                           }
                           else
                           {
                              //Add here
                              this->mc_UndoManager.DoAddSpecificElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex,
                                                                         this->mpc_ModelViewManager,
                                                                         c_NewIndices,
                                                                         c_OSCContent, c_UIContent);
                              //Delete indices in source
                              pc_SourceTable->m_DeleteIndices(c_SourceIndices);
                           }
                        }
                        //Accept event
                        opc_Event->accept();
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten selection changed event slot

   Here: Register selection change

   \param[in] orc_Selected   Selected items
   \param[in] orc_Deselected Deselected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::selectionChanged(const QItemSelection & orc_Selected,
                                                    const QItemSelection & orc_Deselected)
{
   std::vector<uint32> c_SelectedIndices;
   QTableView::selectionChanged(orc_Selected, orc_Deselected);

   c_SelectedIndices = C_SdNdeDataPoolUtil::h_ConvertVector(this->selectedIndexes());
   C_Uti::h_Uniqueify(c_SelectedIndices);
   Q_EMIT this->SigSelectionChanged(c_SelectedIndices.size());
   m_CheckActions(c_SelectedIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten start drag event

   Here: start drag manually (for custom preview)

   \param[in] oc_SupportedActions Supported actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::startDrag(const Qt::DropActions oc_SupportedActions)
{
   const QModelIndexList c_SelectedItems = this->selectedIndexes();
   const std::vector<uint32> c_ReallySelectedItems = this->m_GetSelectedIndices();

   if (c_ReallySelectedItems.size() > 0)
   {
      //Manual drag
      QDrag * const pc_Drag = new QDrag(this);

      pc_Drag->setMimeData(this->model()->mimeData(c_SelectedItems));
      pc_Drag->exec(oc_SupportedActions, this->defaultDropAction());
      //lint -e{429}  no memory leak because of the parent of pc_Drag and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model view manager

   \param[in,out] opc_Value Model view manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SetModelViewManager(C_SdNdeDataPoolListModelViewManager * const opc_Value)
{
   this->mpc_ModelViewManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to select a concrete dataelement

   \param[in] os32_DataElementIndex   Data element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SelectDataElement(const sint32 os32_DataElementIndex)
{
   this->clearSelection();
   this->selectRow(os32_DataElementIndex);
   this->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select data elements in a specific range

   \param[in] ou32_FirstRow First row to select
   \param[in] ou32_LastRow  Last row to select
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SelectRange(const uint32 ou32_FirstRow, const uint32 ou32_LastRow) const
{
   const QModelIndex c_TopLeft = this->model()->index(ou32_FirstRow, 0);
   const QModelIndex c_BottomRight = this->model()->index(ou32_LastRow, this->model()->columnCount() - 1);
   const QItemSelection c_Selection(c_TopLeft, c_BottomRight);

   this->selectionModel()->select(c_Selection, QItemSelectionModel::Select);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if table is selected

   \return
   True  Selected
   False Not selected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDataPoolListTableView::IsSelected(void) const
{
   return (this->selectedIndexes().size() > 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update selection for new data set column

   \param[in] os32_DataSetColumn Index of new data set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::UpdateSelectionForNewDataSetColumn(const sint32 os32_DataSetColumn)
{
   const QModelIndexList c_SelectedIndices = this->selectedIndexes();

   if ((c_SelectedIndices.size() > 0) && (this->mpc_ModelViewManager != NULL))
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         const sint32 s32_DataSetOffset = pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eDATA_SET);
         std::vector<uint32> c_Rows;
         //Get all rows
         for (uint32 u32_ItIndex = 0; static_cast<sintn>(u32_ItIndex) < c_SelectedIndices.size(); ++u32_ItIndex)
         {
            const QModelIndex & rc_CurIndex = c_SelectedIndices[u32_ItIndex];
            c_Rows.push_back(rc_CurIndex.row());
         }
         //Make rows unique
         C_Uti::h_Uniqueify(c_Rows);
         //Select all rows (again)
         for (uint32 u32_ItRow = 0; u32_ItRow < c_Rows.size(); ++u32_ItRow)
         {
            //this->selectRow(c_Rows[u32_ItRow]);
            this->selectionModel()->select(pc_Model->index(c_Rows[u32_ItRow],
                                                           static_cast<sintn>(s32_DataSetOffset + os32_DataSetColumn)),
                                           QItemSelectionModel::Select);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::Reset(void)
{
   m_UpdateModelView();
   m_HandleColumnChange();
   m_CheckContextMenuText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selected variable names

   \param[in] orc_VariableNames New selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::SetSelectedVariableNames(const std::vector<QString> & orc_VariableNames)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

         if (pc_List != NULL)
         {
            for (uint32 u32_ItElement = 0; u32_ItElement < pc_List->c_Elements.size(); ++u32_ItElement)
            {
               const C_OSCNodeDataPoolListElement & rc_Element = pc_List->c_Elements[u32_ItElement];
               for (uint32 u32_ItVarName = 0; u32_ItVarName < orc_VariableNames.size(); ++u32_ItVarName)
               {
                  const QString & rc_VariableName = orc_VariableNames[u32_ItVarName];
                  if (rc_VariableName.compare(rc_Element.c_Name.c_str()) == 0)
                  {
                     //Match
                     this->selectRow(u32_ItElement);
                     //If selected, scroll there (first column)
                     this->scrollTo(pc_Model->index(u32_ItElement, 0));
                     break;
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected variable names

   \return
   Current selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QString> C_SdNdeDataPoolListTableView::GetSelectedVariableNames(void) const
{
   std::vector<QString> c_Retval;
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
   if (pc_List != NULL)
   {
      const std::vector<uint32> c_Indices = this->m_GetSelectedIndices();
      c_Retval.reserve(c_Indices.size());
      for (uint32 u32_ItVar = 0; u32_ItVar < c_Indices.size(); ++u32_ItVar)
      {
         const uint32 u32_VarIndex = c_Indices[u32_ItVar];
         if (u32_VarIndex < pc_List->c_Elements.size())
         {
            const C_OSCNodeDataPoolListElement & rc_Element = pc_List->c_Elements[u32_VarIndex];
            c_Retval.push_back(rc_Element.c_Name.c_str());
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specified indices

   \param[in] orc_Indices Indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_DeleteIndices(const std::vector<uint32> & orc_Indices)
{
   std::vector<uint32> c_Indices = orc_Indices;
   C_Uti::h_Uniqueify(c_Indices);
   this->mc_UndoManager.DoDeleteElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                         this->mpc_ModelViewManager,
                                         c_Indices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move item in tree

   \param[in] oru32_SourceIndices Source index
   \param[in] oru32_TargetIndices Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_Move(const std::vector<uint32> & oru32_SourceIndices,
                                          const std::vector<uint32> & oru32_TargetIndices)
{
   this->mc_UndoManager.DoMoveElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex,
                                       this->mpc_ModelViewManager,
                                       oru32_SourceIndices, oru32_TargetIndices, false);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected indices

   \return
   Selected indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_SdNdeDataPoolListTableView::m_GetSelectedIndices(void) const
{
   std::vector<uint32> c_Retval;
   QModelIndexList c_SelectedItems = this->selectedIndexes();

   c_Retval.reserve(c_SelectedItems.size());
   for (QModelIndexList::const_iterator c_ItSelectedItem = c_SelectedItems.begin();
        c_ItSelectedItem != c_SelectedItems.end(); ++c_ItSelectedItem)
   {
      const QModelIndex & rc_Item = *c_ItSelectedItem;
      c_Retval.push_back(rc_Item.row());
   }
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);

   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction("", this, &C_SdNdeDataPoolListTableView::InsertAction,
                                                          static_cast<sintn>(Qt::CTRL) +
                                                          static_cast<sintn>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   m_CheckContextMenuText();

   this->mpc_ActionCut = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                             "Cut"), this, &C_SdNdeDataPoolListTableView::Cut,
                                                          static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_ActionCopy = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                              "Copy"), this, &C_SdNdeDataPoolListTableView::Copy,
                                                           static_cast<sintn>(Qt::CTRL) +
                                                           static_cast<sintn>(Qt::Key_C));
   this->mpc_ActionPaste = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                               "Paste"), this, &C_SdNdeDataPoolListTableView::Paste,
                                                            static_cast<sintn>(Qt::CTRL) +
                                                            static_cast<sintn>(Qt::Key_V));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionMoveUp = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Move Up"), this,
                                                             &C_SdNdeDataPoolListTableView::DoMoveUp,
                                                             static_cast<sintn>(Qt::CTRL) +
                                                             static_cast<sintn>(Qt::Key_Up));
   this->mpc_ActionMoveDown = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Down"), this,
                                                               &C_SdNdeDataPoolListTableView::DoMoveDown,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Down));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionDelete = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                "Delete"), this, &C_SdNdeDataPoolListTableView::Delete,
                                                             static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeDataPoolListTableView::customContextMenuRequested, this,
           &C_SdNdeDataPoolListTableView::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   this->mpc_ContextMenu->popup(this->viewport()->mapToGlobal(orc_Pos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle column size change so hide necessary columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_HandleColumnChange(void)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         C_OSCNodeDataPool::E_Type e_DataPoolType;
         //Show all columns
         for (uint32 u32_Col = 0; u32_Col < static_cast<uint32>(pc_Model->columnCount()); ++u32_Col)
         {
            this->showColumn(u32_Col);
         }
         //Handle data pool type
         C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                          e_DataPoolType);
         if (e_DataPoolType == C_OSCNodeDataPool::E_Type::eDIAG)
         {
            this->hideColumn(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eADDRESS));
            this->hideColumn(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eDATA_SIZE));
         }
         else
         {
            //NVM
            this->hideColumn(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eEVENT_CALL));
         }
         m_HandleColumnStateRestore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Recheck if actions statuses are still up to date

   \param[in] orc_SelectedIndices Selected indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_CheckActions(const std::vector<uint32> & orc_SelectedIndices)
{
   if (orc_SelectedIndices.size() > 0)
   {
      bool q_AllowAdd;
      bool q_AllowMoveUp;
      bool q_AllowMoveDown;

      //Add
      if (static_cast<uint32>(this->model()->rowCount()) < mu32_NODE_DATA_POOL_LIST_ELEMENT_MAX)
      {
         q_AllowAdd = true;
      }
      else
      {
         q_AllowAdd = false;
      }
      //Move down & up
      q_AllowMoveUp = true;
      q_AllowMoveDown = true;
      if (this->mpc_ModelViewManager != NULL)
      {
         C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_Model != NULL)
         {
            for (uint32 u32_ItSelectedIndex = 0; u32_ItSelectedIndex < orc_SelectedIndices.size();
                 ++u32_ItSelectedIndex)
            {
               if (orc_SelectedIndices[u32_ItSelectedIndex] == 0)
               {
                  q_AllowMoveUp = false;
               }
               if (orc_SelectedIndices[u32_ItSelectedIndex] == static_cast<uint32>(pc_Model->rowCount() - 1))
               {
                  q_AllowMoveDown = false;
               }
            }
         }
      }
      //Actions
      this->mq_AllowMoveDown = q_AllowMoveDown;
      if (this->mpc_ActionMoveDown != NULL)
      {
         this->mpc_ActionMoveDown->setEnabled(q_AllowMoveDown);
      }
      this->mq_AllowAdd = q_AllowAdd;
      this->mq_AllowMoveUp = q_AllowMoveUp;
      if (this->mpc_ActionMoveUp != NULL)
      {
         this->mpc_ActionMoveUp->setEnabled(q_AllowMoveUp);
      }
      //Simple Activation
      if (this->mpc_ActionCut != NULL)
      {
         this->mpc_ActionCut->setEnabled(true);
      }
      if (this->mpc_ActionCopy != NULL)
      {
         this->mpc_ActionCopy->setEnabled(true);
      }
      if (this->mpc_ActionPaste != NULL)
      {
         this->mpc_ActionPaste->setEnabled(true);
      }
      if (this->mpc_ActionDelete != NULL)
      {
         this->mpc_ActionDelete->setEnabled(true);
      }
      //Buttons
      Q_EMIT this->SigButtonChange(true, true, true, true, true, q_AllowMoveDown, q_AllowMoveUp);
   }
   else
   {
      //Simple Deactivation
      if (this->mpc_ActionCut != NULL)
      {
         this->mpc_ActionCut->setEnabled(false);
      }
      if (this->mpc_ActionCopy != NULL)
      {
         this->mpc_ActionCopy->setEnabled(false);
      }
      if (this->mpc_ActionPaste != NULL)
      {
         this->mpc_ActionPaste->setEnabled(false);
      }
      if (this->mpc_ActionDelete != NULL)
      {
         this->mpc_ActionDelete->setEnabled(false);
      }
      this->mq_AllowMoveDown = false;
      if (this->mpc_ActionMoveDown != NULL)
      {
         this->mpc_ActionMoveDown->setEnabled(this->mq_AllowMoveDown);
      }
      this->mq_AllowMoveUp = false;
      if (this->mpc_ActionMoveUp != NULL)
      {
         this->mpc_ActionMoveUp->setEnabled(this->mq_AllowMoveUp);
      }
      //Buttons
      Q_EMIT this->SigButtonChange(true, false, false, false, false, false, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update model
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_UpdateModelView(void)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex,
         this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         this->setModel(pc_Model);
         this->mc_Delegate.SetModel(pc_Model);
      }
   }
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->RegisterElementView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                      this->mu32_ListIndex, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle link clicked

   \param[in] oc_Index Index of clicked link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_HandleLinkClicked(const QModelIndex & orc_Index)
{
   if (orc_Index.isValid() == true)
   {
      tgl_assert(this->mpc_ModelViewManager != NULL);
      if (this->mpc_ModelViewManager != NULL)
      {
         //Model
         C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
            this->mu32_NodeIndex,
            this->mu32_DataPoolIndex,
            this->mu32_ListIndex);
         if (pc_Model != NULL)
         {
            sint32 s32_DataSetIndex = -1;
            C_SdNdeDataPoolUtil::E_ArrayEditType e_ArrayEditType;
            const C_SdNdeDataPoolListTableModel::E_Columns e_Column = pc_Model->ColumnToEnum(
               orc_Index.column(), &s32_DataSetIndex);
            if (e_Column == C_SdNdeDataPoolListTableModel::eMIN)
            {
               e_ArrayEditType = C_SdNdeDataPoolUtil::eARRAY_EDIT_MIN;
            }
            else if (e_Column == C_SdNdeDataPoolListTableModel::eMAX)
            {
               e_ArrayEditType = C_SdNdeDataPoolUtil::eARRAY_EDIT_MAX;
            }
            else
            {
               e_ArrayEditType = C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET;
            }
            if (orc_Index.row() >= 0)
            {
               const uint32 u32_ElementIndex = static_cast<uint32>(orc_Index.row());
               const uint32 u32_DataSetIndex = static_cast<uint32>(s32_DataSetIndex);
               C_OSCNodeDataPoolListElement c_OSCElement;
               C_PuiSdNodeDataPoolListElement c_UIElement;
               QPointer<C_OgePopUpDialog> c_Dialog = new C_OgePopUpDialog(this, this);

               if (C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(this->mu32_NodeIndex,
                                                                           this->mu32_DataPoolIndex,
                                                                           this->mu32_ListIndex, u32_ElementIndex,
                                                                           c_OSCElement,
                                                                           c_UIElement) == C_NO_ERR)
               {
                  C_SdNdeDataPoolListArrayEditWidget * const pc_ArrayEditWidget =
                     new C_SdNdeDataPoolListArrayEditWidget(
                        *c_Dialog,
                        this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                        this->mu32_ListIndex, u32_ElementIndex, e_ArrayEditType, u32_DataSetIndex);

                  pc_ArrayEditWidget->SetModelViewManager(this->mpc_ModelViewManager);
                  //Resize
                  c_Dialog->SetSize(QSize(937, 330));
                  if (c_Dialog->exec() == static_cast<sintn>(QDialog::Accepted))
                  {
                     //Register undo
                     QUndoCommand * const pc_UndoCommand = pc_ArrayEditWidget->TakeUndoCommand();
                     if (pc_UndoCommand != NULL)
                     {
                        //Undo all because push automatically redos
                        pc_UndoCommand->undo();
                        this->mc_UndoManager.DoPush(pc_UndoCommand);
                     }
                  }
                  else
                  {
                     if (c_Dialog != NULL)
                     {
                        //Revert changes
                        C_PuiSdHandler::h_GetInstance()->SetDataPoolListElement(this->mu32_NodeIndex,
                                                                                this->mu32_DataPoolIndex,
                                                                                this->mu32_ListIndex, u32_ElementIndex,
                                                                                c_OSCElement, c_UIElement);
                     }
                  }
                  //lint -e{429}  no memory leak because of the parent of pc_ArrayEditWidget and the Qt memory
                  // management
               }
               if (c_Dialog != NULL)
               {
                  c_Dialog->HideOverlay();
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reinitialize context menu text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_CheckContextMenuText(void)
{
   if (this->mpc_ActionAdd != NULL)
   {
      QString c_Text;
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex,
         this->mu32_DataPoolIndex);

      if (pc_DataPool != NULL)
      {
         if (pc_DataPool->e_Type == C_OSCNodeDataPool::eDIAG)
         {
            c_Text = C_GtGetText::h_GetText("Add new Variable");
         }
         else
         {
            c_Text = C_GtGetText::h_GetText("Add new Parameter");
         }
      }
      else
      {
         c_Text = C_GtGetText::h_GetText("Add new Data Element");
      }
      this->mpc_ActionAdd->setText(c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   React to column resize

   Currently deactivated but kept if necessary in future
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_OnColumnResize(void)
{
   tgl_assert(this->mpc_ModelViewManager != NULL);
   if (this->mpc_ModelViewManager != NULL)
   {
      //Model
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex,
         this->mu32_DataPoolIndex,
         this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         const sint32 s32_ColAuto =
            this->columnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX));
         const sint32 s32_ColEvent = this->columnWidth(pc_Model->EnumToColumn(
                                                          C_SdNdeDataPoolListTableModel::eEVENT_CALL));
         if (this->ms32_LastIndicatorSize < std::max(s32_ColAuto, s32_ColEvent))
         {
            QString c_Style;
            this->ms32_LastIndicatorSize = std::max(s32_ColAuto, s32_ColEvent);

            //Stylesheet to make complete checkbox area interact-able
            c_Style = QString("stw_opensyde_gui--C_SdNdeDataPoolListTableView::indicator {"
                              "width: %1px;"
                              "height: 40px;"
                              "}").arg(this->ms32_LastIndicatorSize);
            this->setStyleSheet(c_Style);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of element after highest selected element

   \return
   Index of element after highest selected element
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeDataPoolListTableView::m_GetOneAfterHighestSelected(void)
{
   uint32 u32_Retval = 0;
   const std::vector<uint32> c_SelectedItems = this->m_GetSelectedIndices();

   if (c_SelectedItems.size() > 0)
   {
      for (uint32 u32_ItSelected = 0; u32_ItSelected < c_SelectedItems.size(); ++u32_ItSelected)
      {
         const uint32 u32_Cur = c_SelectedItems[u32_ItSelected];
         u32_Retval = std::max(u32_Retval, u32_Cur);
      }
      ++u32_Retval;
   }
   else
   {
      if (this->mpc_ModelViewManager != NULL)
      {
         //Add at end
         const C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if (pc_Model != NULL)
         {
            const sint32 s32_RowCount = pc_Model->rowCount();
            if (s32_RowCount >= 0)
            {
               u32_Retval = static_cast<uint32>(s32_RowCount);
            }
         }
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle column state store
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_HandleColumnStateSave(void) const
{
   if (this->model() != NULL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      const C_OSCNodeDataPool * const pc_NodeDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex);
      const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

      if (((pc_Node != NULL) && (pc_NodeDataPool != NULL)) && (pc_NodeDataPoolList != NULL))
      {
         std::vector<sint32> c_ColumnWidths;
         c_ColumnWidths.reserve(this->model()->columnCount());
         for (sint32 s32_ItColumn = 0; s32_ItColumn < this->model()->columnCount(); ++s32_ItColumn)
         {
            c_ColumnWidths.push_back(this->columnWidth(s32_ItColumn));
         }
         C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolListColumnSizes(
            pc_Node->c_Properties.c_Name.c_str(), pc_NodeDataPool->c_Name.c_str(),
            pc_NodeDataPoolList->c_Name.c_str(), c_ColumnWidths);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle column state restore
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListTableView::m_HandleColumnStateRestore(void)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_ModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_Model != NULL)
      {
         bool q_RestoreDefault = true;
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         const C_OSCNodeDataPool * const pc_NodeDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex);
         const C_OSCNodeDataPoolList * const pc_NodeDataPoolList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

         if (((pc_Node != NULL) && (pc_NodeDataPool != NULL)) && (pc_NodeDataPoolList != NULL))
         {
            const C_UsNode c_UserNode =
               C_UsHandler::h_GetInstance()->GetProjSdNode(pc_Node->c_Properties.c_Name.c_str());
            const C_UsNodeDatapool c_UserDataPool = c_UserNode.GetDatapool(pc_NodeDataPool->c_Name.c_str());
            const C_UsNodeDatapoolList c_UserList = c_UserDataPool.GetOtherList(pc_NodeDataPoolList->c_Name.c_str());
            const std::vector<sint32> & rc_ColumnWidths = c_UserList.GetColumnWidths();
            //Only apply user settings if number of expected columns, otherwise this could lead to unexpected behavior
            if ((rc_ColumnWidths.size() > 0UL) &&
                (static_cast<uint32>(pc_Model->columnCount()) == rc_ColumnWidths.size()))
            {
               q_RestoreDefault = false;
               for (uint32 u32_ItCol = 0; u32_ItCol < rc_ColumnWidths.size(); ++u32_ItCol)
               {
                  this->setColumnWidth(static_cast<sint32>(u32_ItCol), rc_ColumnWidths[u32_ItCol]);
               }
            }
         }
         if (q_RestoreDefault == true)
         {
            //Table size
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eINVALID), 32);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eICON), 26);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eINDEX), 50);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eNAME), 225);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eCOMMENT), 248);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eVALUE_TYPE), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eARRAY_SIZE), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eMIN), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eMAX), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eFACTOR), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eOFFSET), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eUNIT), 40);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eACCESS), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eDATA_SIZE), 85);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eADDRESS), 85);
            //this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eDATA_SET), 150);
            this->setColumnWidth(pc_Model->EnumToColumn(C_SdNdeDataPoolListTableModel::eEVENT_CALL), 85);
         }
      }
   }
}

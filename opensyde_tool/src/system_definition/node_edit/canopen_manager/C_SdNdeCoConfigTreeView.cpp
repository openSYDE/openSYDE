//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for tree for navigation and configuration of CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QKeyEvent>
#include <QScrollBar>

#include "C_Uti.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_PuiUtil.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCImportEdsDcf.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeCoConfigTreeView.h"
#include "C_CieImportReportWidget.h"
#include "C_CieDataPoolListAdapter.h"
#include "C_SdNdeCoAddDeviceDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeDelegate::C_SdNdeCoConfigTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: Draw special background of channel ID and paint parent selected state.

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                        const QModelIndex & orc_Index) const
{
   // paint parent of current item in selected style
   QStyleOptionViewItem c_Option = orc_Option;

   if (orc_Index == mc_Index)
   {
      c_Option.state.setFlag(QStyle::State_Selected, true);
      c_Option.state.setFlag(QStyle::State_Enabled, true);
   }

   // draw original
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remember current index.

   \param[in]  orc_Index   Relevant model index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeDelegate::SetCurrentIndexParentRow(const QModelIndex & orc_Index)
{
   this->mc_Index = orc_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoConfigTreeView::C_SdNdeCoConfigTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mpc_ContextMenu(new C_OgeContextMenu),
   mpc_AddAction(NULL),
   mpc_RemoveAction(NULL)
{
   this->C_SdNdeCoConfigTreeView::setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   this->setDragEnabled(false);
   this->setHeaderHidden(true);
   this->setSelectionMode(QAbstractItemView::SingleSelection);
   this->m_SetupContextMenu();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigCommDatapoolsChanged, this,
           &C_SdNdeCoConfigTreeView::SigCommDatapoolsChanged);
   connect(&this->mc_Model, &C_SdNdeCoConfigTreeModel::SigErrorChange, this,
           &C_SdNdeCoConfigTreeView::SigErrorChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of all elements and the Qt memory management
C_SdNdeCoConfigTreeView::~C_SdNdeCoConfigTreeView(void)
{
   delete this->mpc_ContextMenu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetNodeId(const uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mc_Model.SetNodeId(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select specific interface in CANopen tree view.

   \param[in]  ou8_InterfaceNumber  interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetInterfaceSelected(const uint8 ou8_InterfaceNumber)
{
   const QModelIndex c_Index = this->mc_Model.GetInterfaceModelIndex(ou8_InterfaceNumber);

   this->setCurrentIndex(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select specific device in CANopen tree view.

   \param[in]  ou8_InterfaceNumber  interface number
   \param[in]  orc_NodeId           node index
   \param[in]  ou32_UseCaseIndex    index of use case (EDS_FILE | Configuration | PDOs)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::SetDeviceSelected(const uint8 ou8_InterfaceNumber, const C_OSCCanInterfaceId & orc_NodeId,
                                                const uint32 ou32_UseCaseIndex)
{
   const QModelIndex c_Index = this->mc_Model.GetDeviceUseCaseModelIndex(ou8_InterfaceNumber, orc_NodeId,
                                                                         ou32_UseCaseIndex);

   this->setCurrentIndex(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Selection changed

   \param[in]  orc_SelectedItems    Selected items
   \param[in]  orc_DeselectedItems  Deselected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::selectionChanged(const QItemSelection & orc_SelectedItems,
                                               const QItemSelection & orc_DeselectedItems)
{
   C_OgeTreeViewToolTipBase::selectionChanged(orc_SelectedItems, orc_DeselectedItems);
   this->m_OnItemSelected();
   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: Draw the separator lines and background for selected items

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->viewport() != NULL)
   {
      QRect c_Rect;
      QPainter c_Painter(this->viewport());
      const QModelIndexList & rc_SelectedIndexes = this->selectionModel()->selectedIndexes();

      c_Painter.setPen(Qt::transparent);

      if (rc_SelectedIndexes.size() == 1)
      {
         const QModelIndex & rc_CurrentIndex = rc_SelectedIndexes[0];
         const QModelIndex c_Parent = rc_CurrentIndex.parent();

         // draw background of selected items on complete row width
         c_Rect = this->visualRect(rc_CurrentIndex);
         c_Rect.setX(0);
         c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_10);
         c_Painter.drawRect(c_Rect);

         // draw the special lines
         if ((this->mc_Model.hasChildren(rc_CurrentIndex) == false) &&
             (c_Parent.isValid() == true) &&
             (this->isExpanded(c_Parent) == true))
         {
            QModelIndex c_LastIndex;

            c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_7);

            c_LastIndex = this->mc_Model.index(this->mc_Model.rowCount(c_Parent) - 1, 0, c_Parent);

            // draw the top line
            c_Rect = this->visualRect(c_Parent);
            c_Painter.drawRect(0, c_Rect.y(), this->width(), 2);

            // draw the bottom line
            c_Rect = this->visualRect(c_LastIndex);
            c_Painter.drawRect(0, c_Rect.y() + c_Rect.height(), this->width(), 2);
         }
      }
   }

   C_OgeTreeViewToolTipBase::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_SetupContextMenu()
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeCoConfigTreeView::customContextMenuRequested,
           this, &C_SdNdeCoConfigTreeView::m_OnCustomContextMenuRequested);

   this->mpc_AddAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add CANopen Device"),
                                                          this, &C_SdNdeCoConfigTreeView::m_OnAddDevice);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"), this, &C_SdNdeCoConfigTreeView::expandAll);
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"), this,
                                    &C_SdNdeCoConfigTreeView::collapseAll);
   this->mpc_ContextMenu->addSeparator();
   this->mpc_RemoveAction = this->mpc_ContextMenu->addAction("Unknown",
                                                             this, &C_SdNdeCoConfigTreeView::m_OnRemoveDevice);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QModelIndex c_Index = this->indexAt(orc_Pos);

   if (c_Index.isValid() == true)
   {
      uint8 u8_InterfaceNumber;
      C_OSCCanInterfaceId c_NodeId;
      const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

      this->mc_LastKnownContextMenuPos = orc_Pos;
      if (this->mc_Model.GetInterfaceIdForModelIndex(c_Index, u8_InterfaceNumber) == C_NO_ERR)
      {
         if (C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(this->mu32_NodeIndex, u8_InterfaceNumber) != NULL)
         {
            this->mpc_AddAction->setEnabled(true);
         }
         else
         {
            this->mpc_AddAction->setEnabled(false);
         }
      }

      if (this->mc_Model.GetDeviceIndexForModelIndex(c_Index, u8_InterfaceNumber, c_NodeId) == C_NO_ERR)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeId.u32_NodeIndex);
         if (pc_Node != NULL)
         {
            this->mpc_RemoveAction->setVisible(true);
            this->mpc_RemoveAction->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                    "Remove CANopen Device \"%1\"")).arg(pc_Node->
                                                                                                         c_Properties.
                                                                                                         c_Name.c_str()));
         }
         else
         {
            this->mpc_RemoveAction->setVisible(false);
         }
      }
      else
      {
         this->mpc_RemoveAction->setVisible(false);
      }

      // popup
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDevice(void)
{
   const QModelIndex c_Index = this->indexAt(this->mc_LastKnownContextMenuPos);
   uint8 u8_InterfaceNumber;

   if (this->mc_Model.GetInterfaceIdForModelIndex(c_Index, u8_InterfaceNumber) == C_NO_ERR)
   {
      QPointer<C_OgePopUpDialog> const c_PopUp = new C_OgePopUpDialog(this, this);
      C_SdNdeCoAddDeviceDialog * const pc_AddDialog = new C_SdNdeCoAddDeviceDialog(*c_PopUp,
                                                                                   this->mu32_NodeIndex,
                                                                                   u8_InterfaceNumber);

      //Resize
      c_PopUp->SetSize(QSize(1200, 745));

      Q_UNUSED(pc_AddDialog)

      if (c_PopUp->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         // Let user select node and interface
         uint32 u32_NodeIndex = 0;
         uint32 u32_InterfaceIndex = 0;
         QString c_EdsPath;
         if (pc_AddDialog->GetNodeSelection(u32_NodeIndex, u32_InterfaceIndex, c_EdsPath) == C_RANGE)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Add CANopen Device"));
            c_Message.SetDescription(C_GtGetText::h_GetText(
                                        "Cannot add CANopen Device. There is no free Node available."));
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();
         }
         else if (!TGL_FileExists(pc_AddDialog->GetEDSFile()))
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("EDS File"));
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                             "Could not load specified EDS File.")));
            c_Message.SetDetails(C_GtGetText::h_GetText("Specified EDS File does not exist."));
            c_Message.SetCustomMinHeight(180, 250);
            c_Message.Execute();
         }
         else
         {
            this->m_OnAddDeviceReport(u32_NodeIndex, u32_InterfaceIndex, u8_InterfaceNumber, c_EdsPath);
         }
      }

      if (c_PopUp != NULL)
      {
         c_PopUp->HideOverlay();
         c_PopUp->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device report

   \param[in]  ou32_SelectedNodeIndex           Selected node index
   \param[in]  ou32_SelectedNodeInterfaceIndex  Selected node interface index
   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_EdsPath                      Eds path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDeviceReport(const uint32 ou32_SelectedNodeIndex,
                                                  const uint32 ou32_SelectedNodeInterfaceIndex,
                                                  const uint8 ou8_OriginalNodeInterfaceNumber,
                                                  const QString & orc_EdsPath)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_SelectedNodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      tgl_assert(ou32_SelectedNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      if (ou32_SelectedNodeInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_CurInterface =
            pc_Node->c_Properties.c_ComInterfaces[ou32_SelectedNodeInterfaceIndex];
         stw_scl::C_SCLString c_ParsingError;
         std::vector<C_OSCCanMessage> c_OSCRxMessageData;
         std::vector<C_OSCNodeDataPoolListElement> c_OSCRxSignalData;
         std::vector<uint8> c_RxSignalDefaultMinMaxValuesUsed;
         std::vector<C_OSCCanMessage> c_OSCTxMessageData;
         std::vector<C_OSCNodeDataPoolListElement> c_OSCTxSignalData;
         std::vector<uint8> c_TxSignalDefaultMinMaxValuesUsed;
         std::vector<std::vector<stw_scl::C_SCLString> > c_ImportMessagesPerMessage;
         std::vector<C_OSCCanMessage> c_InvalidOSCRxMessageData;
         std::vector<C_OSCNodeDataPoolListElement> c_InvalidOSCRxSignalData;
         std::vector<uint8> c_InvalidRxSignalDefaultMinMaxValuesUsed;
         std::vector<C_OSCCanMessage> c_InvalidOSCTxMessageData;
         std::vector<C_OSCNodeDataPoolListElement> c_InvalidOSCTxSignalData;
         std::vector<uint8> c_InvalidTxSignalDefaultMinMaxValuesUsed;
         std::vector<std::vector<stw_scl::C_SCLString> > c_InvalidImportMessagesPerMessage;
         const sint32 s32_ImportResult = C_OSCImportEdsDcf::h_Import(
            C_PuiUtil::h_GetAbsolutePathFromProject(
               orc_EdsPath).toStdString().c_str(),
            rc_CurInterface.u8_NodeID,
            c_OSCRxMessageData, c_OSCRxSignalData, c_RxSignalDefaultMinMaxValuesUsed,
            c_OSCTxMessageData, c_OSCTxSignalData, c_TxSignalDefaultMinMaxValuesUsed,
            c_ImportMessagesPerMessage, c_ParsingError, true,
            c_InvalidOSCRxMessageData, c_InvalidOSCRxSignalData, c_InvalidRxSignalDefaultMinMaxValuesUsed,
            c_InvalidOSCTxMessageData, c_InvalidOSCTxSignalData, c_InvalidTxSignalDefaultMinMaxValuesUsed,
            c_InvalidImportMessagesPerMessage);
         if (s32_ImportResult == C_NO_ERR)
         {
            uint8 u8_InterfaceId;
            uint32 u32_InterfaceIndex;
            C_CieImportDataAssignment c_NodeAssignment;
            C_CieImportDataAssignment c_InvalidNodeAssignment;
            c_NodeAssignment.u32_OsyNodeIndex = this->mu32_NodeIndex;
            c_InvalidNodeAssignment.u32_OsyNodeIndex = this->mu32_NodeIndex;
            if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceNumberToIndex(this->mu32_NodeIndex,
                                                                                    ou8_OriginalNodeInterfaceNumber,
                                                                                    u32_InterfaceIndex) == C_NO_ERR)
            {
               c_NodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
               c_InvalidNodeAssignment.u32_OsyInterfaceIndex = u32_InterfaceIndex;
            }

            c_NodeAssignment.c_ImportData =
               C_CieDataPoolListAdapter::h_GetStructureFromDCFAndEDSFileImport(c_OSCRxMessageData,
                                                                               c_OSCRxSignalData,
                                                                               c_RxSignalDefaultMinMaxValuesUsed,
                                                                               c_OSCTxMessageData,
                                                                               c_OSCTxSignalData,
                                                                               c_TxSignalDefaultMinMaxValuesUsed,
                                                                               c_ImportMessagesPerMessage);
            c_InvalidNodeAssignment.c_ImportData =
               C_CieDataPoolListAdapter::h_GetStructureFromDCFAndEDSFileImport(c_InvalidOSCRxMessageData,
                                                                               c_InvalidOSCRxSignalData,
                                                                               c_InvalidRxSignalDefaultMinMaxValuesUsed,
                                                                               c_InvalidOSCTxMessageData,
                                                                               c_InvalidOSCTxSignalData,
                                                                               c_InvalidTxSignalDefaultMinMaxValuesUsed,
                                                                               c_InvalidImportMessagesPerMessage);
            if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(ou32_SelectedNodeIndex,
                                                                                ou32_SelectedNodeInterfaceIndex,
                                                                                u8_InterfaceId) == C_NO_ERR)
            {
               const C_OSCCanInterfaceId c_Id(ou32_SelectedNodeIndex, u8_InterfaceId);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id, c_NodeAssignment.c_ImportData.c_Core.c_OSCRxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id, c_NodeAssignment.c_ImportData.c_Core.c_OSCTxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id,
                                                      c_InvalidNodeAssignment.c_ImportData.c_Core.c_OSCRxMessageData);
               C_CieDataPoolListAdapter::h_AssignNode(c_Id,
                                                      c_InvalidNodeAssignment.c_ImportData.c_Core.c_OSCTxMessageData);
            }
            if (((c_NodeAssignment.c_ImportData.c_Core.c_OSCRxMessageData.size() > 0UL) ||
                 (c_NodeAssignment.c_ImportData.c_Core.c_OSCTxMessageData.size() > 0UL)) ||
                ((c_InvalidNodeAssignment.c_ImportData.c_Core.c_OSCRxMessageData.size() > 0UL) ||
                 (c_InvalidNodeAssignment.c_ImportData.c_Core.c_OSCTxMessageData.size() > 0UL))
                )
            {
               std::vector<C_CieImportDataAssignment> c_NodeAssignmentVector;
               c_NodeAssignmentVector.push_back(c_NodeAssignment);

               std::vector<C_CieImportDataAssignment> c_InvalidNodeAssignmentVector;
               c_InvalidNodeAssignmentVector.push_back(c_InvalidNodeAssignment);

               QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
               C_CieImportReportWidget * const pc_Dialog =
                  new C_CieImportReportWidget(*c_New, orc_EdsPath, rc_CurInterface.u32_BusIndex,
                                              C_OSCCanProtocol::eCAN_OPEN,
                                              c_NodeAssignmentVector, c_InvalidNodeAssignmentVector,
                                              &pc_Node->c_Properties.c_Name);

               Q_UNUSED(pc_Dialog)

               //Resize
               c_New->SetSize(QSize(1210, 790));

               if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
               {
                  this->m_OnAddDeviceReportAccepted(ou32_SelectedNodeIndex, u8_InterfaceId,
                                                    ou8_OriginalNodeInterfaceNumber, orc_EdsPath);
               }

               if (c_New != NULL)
               {
                  c_New->HideOverlay();
                  c_New->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
            else
            {
               C_OgeWiCustomMessage c_Message(this);
               c_Message.SetHeading(C_GtGetText::h_GetText("Nothing found"));
               c_Message.SetDescription(C_GtGetText::h_GetText("No messages found in file."));
               c_Message.SetDetails(static_cast<QString>(
                                       "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                    arg(C_GtGetText::h_GetText(
                                           "For possible parsing errors, warnings and detailed information see ")).
                                    arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                                    arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                    arg(C_GtGetText::h_GetText("log file")));
               c_Message.SetCustomMinHeight(180, 270);
               c_Message.Execute();
            }
         }
         else
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("EDS file"));
            switch (s32_ImportResult)
            {
            case C_RANGE:
               c_Message.SetDescription(C_GtGetText::h_GetText("Invalid parameter."));
               c_Message.SetDetails(static_cast<QString>(c_ParsingError.c_str()));
               c_Message.SetCustomMinHeight(180, 300);
               break;
            case C_NOACT:
               c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                "EDS file import failed.\nNode ID %1 is invalid.")).arg(
                                           rc_CurInterface.u8_NodeID));
               c_Message.SetDetails(C_GtGetText::h_GetText("CANopen standard only supports node IDs in the range "
                                                           "of 1 to 127.\nThe node ID can be changed in node "
                                                           "properties."));
               c_Message.SetCustomMinHeight(180, 270);
               break;
            case C_CONFIG:
               c_Message.SetDescription(C_GtGetText::h_GetText("An error occured while parsing."));
               //Update log file
               C_OSCLoggingHandler::h_Flush();
               c_Message.SetDetails(static_cast<QString>(
                                       "%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                    arg(C_GtGetText::h_GetText("For more information see ")).
                                    arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                                    arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                    arg(C_GtGetText::h_GetText("log file")));
               c_Message.SetCustomMinHeight(180, 250);
               break;
            default:
               c_Message.SetDescription(C_GtGetText::h_GetText("Unknown reason."));
               c_Message.SetCustomMinHeight(180, 180);
               break;
            }
            c_Message.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add device report accepted

   \param[in]  ou32_SelectedNodeIndex           Selected node index
   \param[in]  ou8_SelectedNodeInterfaceNumber  Selected node interface number
   \param[in]  ou8_OriginalNodeInterfaceNumber  Original node interface number
   \param[in]  orc_EdsPath                      Eds path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnAddDeviceReportAccepted(const uint32 ou32_SelectedNodeIndex,
                                                          const uint8 ou8_SelectedNodeInterfaceNumber,
                                                          const uint8 ou8_OriginalNodeInterfaceNumber,
                                                          const QString & orc_EdsPath)
{
   C_OSCCanOpenManagerDeviceInfo c_Config;

   c_Config.c_EDSFileName = TGL_ExtractFileName(orc_EdsPath.toStdString());
   tgl_assert(c_Config.c_EDSFileContent.LoadFromFile(orc_EdsPath.toStdString()) == C_NO_ERR);

   this->mc_Model.AddDevice(ou32_SelectedNodeIndex, ou8_SelectedNodeInterfaceNumber, ou8_OriginalNodeInterfaceNumber,
                            c_Config);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle remove device action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnRemoveDevice()
{
   const QModelIndex c_Index = this->indexAt(this->mc_LastKnownContextMenuPos);
   uint8 u8_InterfaceNumber;
   C_OSCCanInterfaceId c_NodeId;

   if (this->mc_Model.GetDeviceIndexForModelIndex(c_Index, u8_InterfaceNumber, c_NodeId) == C_NO_ERR)
   {
      this->mc_Model.RemoveDevice(u8_InterfaceNumber, c_NodeId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle item selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoConfigTreeView::m_OnItemSelected(void)
{
   const QModelIndexList c_SelectedItems = this->selectedIndexes();

   if (c_SelectedItems.size() >= 1)
   {
      const QModelIndex c_Current =  c_SelectedItems.at(0);
      tgl_assert(c_SelectedItems.size() == 1);
      if (c_Current.isValid() == true)
      {
         uint8 u8_InterfaceNumber;

         if ((this->mc_Model.hasChildren(c_Current) == false) &&
             (c_Current.parent().isValid() == true) &&
             (this->isExpanded(c_Current.parent()) == true))
         {
            // delegate needs to know parent row of current index
            this->mc_Delegate.SetCurrentIndexParentRow(c_Current.parent());
         }
         else
         {
            this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
         }

         if (this->mc_Model.GetInterfaceIdForModelIndex(c_Current, u8_InterfaceNumber) == C_NO_ERR)
         {
            C_OSCCanInterfaceId c_NodeId;
            uint32 u32_UseCaseIndex;

            if (this->mc_Model.GetDeviceAndUseCaseIndexForModelIndex(c_Current, u8_InterfaceNumber, c_NodeId,
                                                                     u32_UseCaseIndex) == C_NO_ERR)
            {
               Q_EMIT (this->SigDeviceSelected(u8_InterfaceNumber, c_NodeId, u32_UseCaseIndex));
            }
            else
            {
               Q_EMIT (this->SigInterfaceSelected(u8_InterfaceNumber));
            }
         }
      }
      else
      {
         this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
      }
   }
   else
   {
      this->mc_Delegate.SetCurrentIndexParentRow(QModelIndex());
   }
}

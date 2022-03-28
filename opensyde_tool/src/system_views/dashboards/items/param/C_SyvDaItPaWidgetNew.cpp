//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for parameterization widget.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SyvDaItPaWidgetNew.h"
#include "ui_C_SyvDaItPaWidgetNew.h"

#include "C_Uti.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdUtil.h"
#include "C_GiSvDaParam.h"
#include "C_PuiSvHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaItPaWriteWidget.h"
#include "C_SyvDaItPaImportReport.h"
#include "C_SyvDaPeDataElementBrowse.h"
#include "C_SyvDaItPaImageRecordWidget.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_UsHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCParamSetHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaItPaWidgetNew::mhc_FILE_EXTENSION_PARAMSET = ".syde_ps";
const QString C_SyvDaItPaWidgetNew::mhc_FILE_EXTENSION_PARAMSETIMAGE = ".syde_psi";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oru32_ViewIndex  View index
   \param[in]      opc_DataWidget   Data widget
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaWidgetNew::C_SyvDaItPaWidgetNew(const uint32 & oru32_ViewIndex,
                                           C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItPaWidgetNew),
   mu32_ViewIndex(oru32_ViewIndex),
   mpc_DataWidget(opc_DataWidget),
   mpc_ComDriver(NULL),
   mq_IsConnected(false),
   mq_ReadActive(false),
   mq_WriteActive(false),
   mq_Editable(false),
   mq_ReadAllowed(false),
   mu32_ListCounter(0U)
{
   mpc_Ui->setupUi(this);

   //Init lists
   m_HandleTreeInitAction();

   // Init GUI
   InitStaticNames();

   this->mpc_Ui->pc_PushButtonAdd->setIconSize(QSize(20, 20));
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(false);

   //Remove debug texts
   this->mpc_Ui->pc_GroupBoxTree->setTitle("");
   this->mpc_Ui->pc_GroupBoxTreeEmpty->setTitle("");

   m_UpdateButtonToolTips();

   //Connections
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SyvDaItPaWidgetNew::ButtonAddClicked);

   //tree actions
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionRead, this,
           &C_SyvDaItPaWidgetNew::m_HandleTreeReadAction);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionWrite, this,
           &C_SyvDaItPaWidgetNew::m_HandleWriteProcessTrigger);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionLoad, this,
           &C_SyvDaItPaWidgetNew::m_LoadElements);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionSave, this,
           &C_SyvDaItPaWidgetNew::m_SaveElements);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionRecord, this,
           &C_SyvDaItPaWidgetNew::m_RecordElements);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigActionRemove, this,
           &C_SyvDaItPaWidgetNew::m_HandleRemoveTrigger);
   connect(this->mpc_Ui->pc_TreeView, &C_SyvDaItPaTreeView::SigInformUserFloatRangeCheck, this,
           &C_SyvDaItPaWidgetNew::m_InformUserFloatRangeCheck);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaWidgetNew::~C_SyvDaItPaWidgetNew()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PushButtonAdd->setText(C_GtGetText::h_GetText("Add List"));
   this->mpc_Ui->pc_LabelTreeEmpty->setText(C_GtGetText::h_GetText("Add any NVM Lists via the '+' button"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::ConnectionActiveChanged(const bool oq_Active)
{
   this->mq_IsConnected = oq_Active;

   if (oq_Active == true)
   {
      this->mpc_Ui->pc_TreeView->ClearECUValues();
      this->mpc_Ui->pc_PushButtonAdd->setEnabled(false);
   }
   else if (this->mq_Editable == true)
   {
      this->mpc_Ui->pc_PushButtonAdd->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonAdd->setEnabled(false);
   }

   this->mpc_Ui->pc_TreeView->SetConnected(oq_Active);

   //Update buttons
   this->m_UpdateButtons();
   m_UpdateButtonToolTips();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the com driver for parametrization functions

   \param[in,out]  orc_ComDriver    Reference to the com driver
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetSyvComDriver(C_SyvComDriverDiag & orc_ComDriver)
{
   this->mpc_ComDriver = &orc_ComDriver;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update shown ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::UpdateECUValues(void) const
{
   this->mpc_Ui->pc_TreeView->UpdateECUValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_NRC        NRC

   \return
   true  Operation was relevant and was handled
   false Operation was not relevant and was not handled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaWidgetNew::HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC)
{
   const bool q_Return = this->mq_ReadActive;
   sint32 s32_Result = os32_Result;

   if (q_Return == true)
   {
      const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
      if (pc_ParamWidget != NULL)
      {
         if (this->mpc_ComDriver != NULL)
         {
            if (s32_Result == C_CHECKSUM)
            {
               //Should be the last read list
               tgl_assert((this->mu32_ListCounter - 1UL) < this->mc_ListIds.size());
               if ((this->mu32_ListCounter - 1UL) < this->mc_ListIds.size())
               {
                  this->mc_ListsWithCRCError.push_back(
                     this->mc_ListIds[static_cast<uintn>(this->mu32_ListCounter) - 1U]);
                  //Signal CRC error
                  this->mpc_Ui->pc_TreeView->SetCRCStatus(
                     this->mc_ListIds[static_cast<uintn>(this->mu32_ListCounter) - 1U], false);
               }
            }
            else if (s32_Result == C_NO_ERR)
            {
               //Should be the last read list
               tgl_assert((this->mu32_ListCounter - 1UL) < this->mc_ListIds.size());
               if ((this->mu32_ListCounter - 1UL) < this->mc_ListIds.size())
               {
                  //Signal CRC OK
                  this->mpc_Ui->pc_TreeView->SetCRCStatus(
                     this->mc_ListIds[static_cast<uintn>(this->mu32_ListCounter) - 1U], true);
               }
            }
            else
            {
               // Nothing to do here. C_CHECKSUM is no functional error. The functional errors are handled later
            }

            //Ignore warn because CRC errors on read should be fixed by this widget
            if ((s32_Result == C_NO_ERR) || (s32_Result == C_CHECKSUM))
            {
               if (this->mu32_ListCounter < this->mc_ListIds.size())
               {
                  const C_OSCNodeDataPoolListElementId & rc_ID = this->mc_ListIds[this->mu32_ListCounter];

                  // Continue the poll for all other lists
                  s32_Result = this->mpc_ComDriver->PollNvmReadList(rc_ID.u32_NodeIndex,
                                                                    static_cast<uint8>(rc_ID.u32_DataPoolIndex),
                                                                    static_cast<uint16>(rc_ID.u32_ListIndex));

                  if (s32_Result == C_NO_ERR)
                  {
                     ++this->mu32_ListCounter;
                  }
               }
               else
               {
                  if (this->mc_ListsWithCRCError.size() > 0UL)
                  {
                     C_OgeWiCustomMessage c_MessageResult(
                        pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eWARNING);
                     QString c_ListsString;
                     for (uint32 u32_ItList = 0; u32_ItList < this->mc_ListsWithCRCError.size(); ++u32_ItList)
                     {
                        const C_OSCNodeDataPoolListElementId & rc_CurId = this->mc_ListsWithCRCError[u32_ItList];
                        const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                           rc_CurId.u32_NodeIndex);
                        const C_OSCNodeDataPool * const pc_DataPool =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
                              rc_CurId.u32_NodeIndex, rc_CurId.u32_DataPoolIndex);
                        const C_OSCNodeDataPoolList * const pc_List =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                              rc_CurId.u32_NodeIndex, rc_CurId.u32_DataPoolIndex, rc_CurId.u32_ListIndex);
                        if (((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL))
                        {
                           c_ListsString += static_cast<QString>("- %1::%2::%3\n").
                                            arg(pc_Node->c_Properties.c_Name.c_str()).
                                            arg(pc_DataPool->c_Name.c_str()).arg(pc_List->c_Name.c_str());
                        }
                     }
                     c_MessageResult.SetHeading(C_GtGetText::h_GetText("Invalid List CRC"));
                     c_MessageResult.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                            "Detected CRCs are invalid for at least one list.")));
                     c_MessageResult.SetDetails(c_ListsString);
                     c_MessageResult.SetCustomMinHeight(180, 300);
                     c_MessageResult.Execute();
                  }
                  // Thread finished with last list
                  m_UpdateReadAllowedFlag(true);
                  m_UpdateButtonToolTips();
                  this->mq_ReadActive = false;
                  //Trigger ECU value reload
                  this->UpdateECUValues();
                  //Special write handling (Trigger write after read finished)
                  if (this->mq_WriteActive == true)
                  {
                     this->mq_WriteActive = false;
                     m_WriteElements(this->mc_ElementsToWriteAfterRead);
                     this->mc_ElementsToWriteAfterRead.clear();
                  }
               }
            }
            else
            {
               //Always clear trigger write
               this->mq_WriteActive = false;
            }
         }
         else
         {
            s32_Result = C_CONFIG;
         }

         // Handling of all functional errors
         if ((s32_Result != C_NO_ERR) && (s32_Result != C_CHECKSUM))
         {
            C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent()->parentWidget(), // parent because
                                                                                                   // of
                                                                                                   // style issues
                                                 C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("System Parametrization"));
            if (s32_Result == C_TIMEOUT)
            {
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("No response received."));
               c_MessageResult.SetDetails(C_GtGetText::h_GetText("This may be caused by:\n"
                                                                 "- Node shutdown/restart while connected\n"
                                                                 "- Connection interrupted while connected\n"
                                                                 "- Node response time outside expected timing constraints\n"
                                                                 "  (node or bus may be overloaded)"));
               c_MessageResult.SetCustomMinHeight(180, 300);
            }
            else if (s32_Result == C_WARN)
            {
               QString c_Details;
               QString c_Node = C_GtGetText::h_GetText("Node");
               if ((this->mu32_ListCounter - 1UL) < this->mc_ListIds.size())
               {
                  const C_OSCNodeDataPoolListElementId & rc_CurEntryId =
                     this->mc_ListIds[static_cast<uintn>(this->mu32_ListCounter) - 1U];
                  const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                     rc_CurEntryId.u32_NodeIndex);
                  if (pc_Node != NULL)
                  {
                     c_Node = static_cast<QString>(C_GtGetText::h_GetText("Node %1")).arg(
                        pc_Node->c_Properties.c_Name.c_str());
                  }
               }
               switch (ou8_NRC)
               {
               case 0x13:
                  c_Details = C_GtGetText::h_GetText("Incorrect length of request");
                  break;
               case 0x31:
                  c_Details = C_GtGetText::h_GetText("Address or length format invalid (> 4 bytes)\n"
                                                     "Requested memory range specified by address and size invalid");
                  break;
               case 0x22:
                  c_Details = C_GtGetText::h_GetText("Reading failed.");
                  break;
               case 0x33:
                  c_Details = C_GtGetText::h_GetText("Required security level was not unlocked.");
                  break;
               case 0x14:
                  c_Details = C_GtGetText::h_GetText(
                     "The total length of the response message exceeds the available buffer size.");
                  break;
               case 0x7F:
                  c_Details = C_GtGetText::h_GetText(
                     "The requested service is not available in the currently active session.");
                  break;
               default:
                  c_Details =
                     static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_NRC,
                                                                                                           16));
                  break;
               }
               c_MessageResult.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                      "%1 responded with error response.")).arg(
                                                 c_Node));
               c_MessageResult.SetDetails(c_Details);
               c_MessageResult.SetCustomMinHeight(180, 250);
            }
            else
            {
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("Failure on manual read."));
               c_MessageResult.SetDetails(C_Uti::h_StwError(s32_Result));
               c_MessageResult.SetCustomMinHeight(180, 250);
            }
            c_MessageResult.Execute();
            m_UpdateReadAllowedFlag(true);
            m_UpdateButtonToolTips();
            this->mq_ReadActive = false;
         }
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark flag value

   \param[in]  oq_Value    New dark flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetDark(const bool oq_Value) const
{
   this->mpc_Ui->pc_TreeView->SetDark(oq_Value);
   this->mpc_Ui->pc_PushButtonAdd->SetDarkMode(oq_Value);

   if (oq_Value == true)
   {
      this->mpc_Ui->pc_PushButtonAdd->SetSvg("://images/IconAddEnabledDarkMode.svg",
                                             "://images/IconAddDisabled.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonAdd->SetSvg("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::ButtonAddClicked(void)
{
   C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(
         pc_ParamWidget->GetPopUpParent(), pc_ParamWidget->GetPopUpParent());
      C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, true,
                                                                                    false, true, false, true, true);

      //Resize
      c_New->SetSize(QSize(800, 800));

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();

         //Implicitly update the data this one is important for the following reload step!
         pc_ParamWidget->UpdateData();

         for (uint32 u32_ItNewElement = 0; u32_ItNewElement < c_DataElements.size(); ++u32_ItNewElement)
         {
            //Get CURRENT param item
            const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
            if (pc_Param != NULL)
            {
               bool q_Found = false;
               const C_PuiSvDbNodeDataPoolListElementId & rc_NewId = c_DataElements[u32_ItNewElement];
               for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_DataPoolElementsConfig.size();
                    ++u32_ItElement)
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config =
                     pc_Param->c_DataPoolElementsConfig[u32_ItElement];
                  if (rc_Config.c_ElementId == rc_NewId)
                  {
                     q_Found = true;
                  }
               }
               if (q_Found == false)
               {
                  tgl_assert(pc_ParamWidget->AddParamItemDataElement(rc_NewId, NULL) == C_NO_ERR);
               }
            }
         }
         //Update
         m_HandleTreeInitAction();
         m_UpdateButtons();
      }

      if (c_New != NULL)
      {
         pc_Dialog->SaveUserSettings();
         pc_Dialog->PrepareCleanUp();
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get template with the type description for the current selection

   \return
   Template with the type description for the current selection
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWidgetNew::GetSelectedItemTypeTemplate(void) const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedItemTypeTemplate();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected item count

   \return
   Current selected item count
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SyvDaItPaWidgetNew::GetSelectedItemCount(void) const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedItemCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current column widths

   \return
   Current column widths
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SyvDaItPaWidgetNew::GetCurrentColumnWidths(void) const
{
   return this->mpc_Ui->pc_TreeView->GetCurrentColumnWidths();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current column position indices

   \return
   Current column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SyvDaItPaWidgetNew::GetCurrentColumnPositionIndices(void) const
{
   return this->mpc_Ui->pc_TreeView->GetCurrentColumnPositionIndices();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new column widths

   \param[in]  orc_NewColWidths  New column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetColumnWidth(const std::vector<sint32> & orc_NewColWidths) const
{
   this->mpc_Ui->pc_TreeView->SetColumnWidth(orc_NewColWidths);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set column position indices

   \param[in]  orc_NewColPositionIndices  New column position indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetColumnPositionIndices(const std::vector<sint32> & orc_NewColPositionIndices) const
{
   this->mpc_Ui->pc_TreeView->SetColumnPositionIndices(orc_NewColPositionIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all expanded tree items

   \return
   All expanded tree items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbExpandedTreeIndex> C_SyvDaItPaWidgetNew::GetAllExpandedTreeItems(void) const
{
   return this->mpc_Ui->pc_TreeView->GetAllExpandedTreeItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set all items as expanded

   \param[in]  orc_Items   Items to expand
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetAllExpandedTreeItems(const std::vector<C_PuiSvDbExpandedTreeIndex> & orc_Items) const
{
   this->mpc_Ui->pc_TreeView->SetAllExpandedTreeItems(orc_Items);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of edit mode

   \param[in]  oq_EditMode          Edit mode
   \param[in]  oq_EditContentMode   Edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::SetEditMode(const bool oq_EditMode, const bool oq_EditContentMode)
{
   this->mq_Editable = oq_EditContentMode;
   this->mpc_Ui->pc_TreeView->SetEditMode(oq_EditContentMode);
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(oq_EditContentMode);

   if (oq_EditContentMode)
   {
      this->mpc_Ui->pc_TreeView->SetLoadSaveActive(true);
   }
   else
   {
      this->mpc_Ui->pc_TreeView->SetLoadSaveActive(!oq_EditMode);
   }
   //Update selection
   this->m_UpdateButtons();
   m_UpdateButtonToolTips();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::HideToolTip(void)
{
   // In case of deactivating the content edit mode when a tooltip is still visible,
   // no further events can be received here. In this case all tooltips must be hided manually
   this->mpc_Ui->pc_TreeView->HideToolTip();
   this->mpc_Ui->pc_PushButtonAdd->HideToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle dashboard param widget specific key functionality

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::keyPressEvent(QKeyEvent * const opc_Event)
{
   // Events can get here only in content edit mode
   bool q_CallOrig = true;

   switch (opc_Event->key())
   {
   case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
   case Qt::Key_Plus:
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         q_CallOrig = false;
         this->ButtonAddClicked();
         opc_Event->accept();
      }
      break;
   default:
      //Nothing to do
      break;
   }

   if (q_CallOrig == true)
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle read action

   Prerequisites: mc_ListIds valid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_ReadElements(void)
{
   sint32 s32_Result = C_NO_ERR;

   if (this->mpc_ComDriver != NULL)
   {
      //Check all list IDs valid -> node active in view
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         for (uint32 u32_ItList = 0UL; u32_ItList < this->mc_ListIds.size(); ++u32_ItList)
         {
            const C_OSCNodeDataPoolListElementId & rc_ID = this->mc_ListIds[u32_ItList];
            const std::vector<uint8> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();

            if ((rc_ID.u32_NodeIndex < rc_ActiveFlags.size()) && (rc_ActiveFlags[rc_ID.u32_NodeIndex] == 1U))
            {
               //Valid
               bool q_Error = false;

               C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(this->mu32_ViewIndex,
                                                                                   rc_ID.u32_NodeIndex,
                                                                                   q_Error);

               if (q_Error == true)
               {
                  s32_Result = C_RANGE;
               }
            }
            else
            {
               s32_Result = C_RANGE;
            }

            if (s32_Result != C_NO_ERR)
            {
               break;
            }
         }

         if (s32_Result == C_NO_ERR)
         {
            this->mc_ListsWithCRCError.clear();
            this->mu32_ListCounter = 0U;

            if (this->mu32_ListCounter < this->mc_ListIds.size())
            {
               const C_OSCNodeDataPoolListElementId & rc_ID = this->mc_ListIds[this->mu32_ListCounter];
               this->mq_ReadActive = true;
               // Start the poll for the first list
               // All other lists will be handled by HandleManualOperationFinished and get triggered by the
               // finished signal of the poll thread
               Q_EMIT (this->SigNvmReadList(rc_ID));
               ++this->mu32_ListCounter;
               m_UpdateReadAllowedFlag(false);
               m_UpdateButtonToolTips();
            }
         }
      }
      else
      {
         s32_Result = C_CONFIG;
      }
   }
   else
   {
      s32_Result = C_CONFIG;
   }

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
   if (pc_ParamWidget != NULL)
   {
      if (s32_Result == C_RANGE)
      {
         C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("System Parametrization"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText(
                                           "Parametrization widget contains nodes, which are not active in current"
                                           " view or have the Dashboard flags disabled."));
         c_MessageResult.SetCustomMinHeight(180, 180);
         c_MessageResult.Execute();
         this->mq_ReadActive = false;
      }
      else if (s32_Result != C_NO_ERR)
      {
         C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("System Parametrization"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText("Failure on read list."));
         c_MessageResult.SetDetails(C_Uti::h_StwError(s32_Result));
         c_MessageResult.SetCustomMinHeight(180, 250);
         c_MessageResult.Execute();
         this->mq_ReadActive = false;
      }
      else
      {
         // no error -> no error handling
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle write action

   \param[in]  orc_ChangedIds    Changed IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_WriteElements(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedIds)
{
   //Step 1: Check the changed elements for all specified lists
   std::vector<C_OSCNodeDataPoolListId> c_InterestingInvalidLists;
   std::vector<C_OSCNodeDataPoolListElementId> c_InterestingChangedElements;
   const std::vector<C_OSCNodeDataPoolListElementId> c_ChangedElementsStart =
      this->mpc_Ui->pc_TreeView->GetChangedListElementIds();
   const std::vector<C_OSCNodeDataPoolListId> c_InvalidLists =
      this->mpc_Ui->pc_TreeView->GetInvalidListIds();
   for (uint32 u32_ItChanged = 0UL; u32_ItChanged < c_ChangedElementsStart.size(); ++u32_ItChanged)
   {
      const C_OSCNodeDataPoolListElementId & rc_ChangedId = c_ChangedElementsStart[u32_ItChanged];
      bool q_Found = false;
      for (uint32 u32_ItSelected = 0UL; (u32_ItSelected < orc_ChangedIds.size()) && (q_Found == false);
           ++u32_ItSelected)
      {
         const C_OSCNodeDataPoolListElementId & rc_Selected = orc_ChangedIds[u32_ItSelected];
         if (((rc_Selected.u32_NodeIndex == rc_ChangedId.u32_NodeIndex) &&
              (rc_Selected.u32_DataPoolIndex == rc_ChangedId.u32_DataPoolIndex)) &&
             (rc_Selected.u32_ListIndex == rc_ChangedId.u32_ListIndex))
         {
            q_Found = true;
         }
      }
      if (q_Found == true)
      {
         c_InterestingChangedElements.push_back(rc_ChangedId);
      }
   }
   for (uint32 u32_ItInvalid = 0UL; u32_ItInvalid < c_InvalidLists.size(); ++u32_ItInvalid)
   {
      const C_OSCNodeDataPoolListId & rc_InvalidId = c_InvalidLists[u32_ItInvalid];
      bool q_Found = false;
      for (uint32 u32_ItSelected = 0UL; (u32_ItSelected < orc_ChangedIds.size()) && (q_Found == false);
           ++u32_ItSelected)
      {
         const C_OSCNodeDataPoolListElementId & rc_Selected = orc_ChangedIds[u32_ItSelected];
         if (((rc_Selected.u32_NodeIndex == rc_InvalidId.u32_NodeIndex) &&
              (rc_Selected.u32_DataPoolIndex == rc_InvalidId.u32_DataPoolIndex)) &&
             (rc_Selected.u32_ListIndex == rc_InvalidId.u32_ListIndex))
         {
            q_Found = true;
         }
      }
      if (q_Found == true)
      {
         c_InterestingInvalidLists.push_back(rc_InvalidId);
      }
   }

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
   if (pc_ParamWidget != NULL)
   {
      if ((c_InterestingChangedElements.size() > 0) || (c_InterestingInvalidLists.size() > 0))
      {
         //Step 2: Check valid values
         if (this->mpc_Ui->pc_TreeView->CheckRange(c_InterestingChangedElements, c_InterestingInvalidLists) == true)
         {
            //Step 3: Write new values to data (required for write process)
            this->mpc_Ui->pc_TreeView->PrepareChangedValues(c_InterestingChangedElements);

            //Should only be possible if online
            tgl_assert(this->mpc_ComDriver != NULL);
            if (this->mpc_ComDriver != NULL)
            {
               //Step 4: Write process dialog
               QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(
                  pc_ParamWidget->GetPopUpParent(), pc_ParamWidget->GetPopUpParent());
               C_SyvDaItPaWriteWidget * const pc_Dialog = new C_SyvDaItPaWriteWidget(*c_New, *this->mpc_ComDriver,
                                                                                     c_InterestingChangedElements,
                                                                                     c_InterestingInvalidLists);

               Q_UNUSED(pc_Dialog)

               //Resize
               c_New->SetSize(QSize(900, 800));

               if (c_New->exec() != static_cast<sintn>(QDialog::Accepted))
               {
                  if (c_New != NULL)
                  {
                     if (pc_Dialog->GetStep() != C_SyvDaItPaWriteWidget::eBEFOREWRITE)
                     {
                        // Error or abortion of user
                        //Remove ECU values in list widget
                        this->mpc_Ui->pc_TreeView->ClearECUValues();
                     }
                  }
               }
               else
               {
                  std::map<C_OSCNodeDataPoolListElementId, C_SyvDaItPaValuePairs> c_AffectedElementValues;
                  if (pc_Dialog->GetChangedElements(c_AffectedElementValues) == C_NO_ERR)
                  {
                     for (std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                   stw_opensyde_gui_logic::C_SyvDaItPaValuePairs>::iterator c_It =
                             c_AffectedElementValues.begin();
                          c_It != c_AffectedElementValues.end(); ++c_It)
                     {
                        const C_PuiSvDbNodeDataPoolListElementId c_Id(c_It->first,
                                                                      C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false,
                                                                      0UL);
                        this->mpc_DataWidget->AddNewNvmValueIntoQueue(c_Id,
                                                                      c_It->second.c_Actual);
                     }
                  }
                  //Trigger ECU value update
                  UpdateECUValues();
                  //Update list CRCs (implicit)
                  for (uint32 u32_ItId = 0; u32_ItId < c_InterestingChangedElements.size(); ++u32_ItId)
                  {
                     const C_OSCNodeDataPoolListElementId & rc_CurId = c_InterestingChangedElements[u32_ItId];
                     this->mpc_Ui->pc_TreeView->SetCRCStatus(rc_CurId, true);
                  }
                  //Update list CRCs (explicit)
                  for (uint32 u32_ItId = 0; u32_ItId < c_InterestingInvalidLists.size(); ++u32_ItId)
                  {
                     const C_OSCNodeDataPoolListId & rc_CurId = c_InterestingInvalidLists[u32_ItId];
                     this->mpc_Ui->pc_TreeView->SetCRCStatus(rc_CurId, true);
                  }
               }

               if (c_New != NULL)
               {
                  c_New->HideOverlay();
                  c_New->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

            // Reset all previous set NVM changed flags in all cases
            // If a flag would not be reseted in case of an error or a cancel,
            // writing to an other datapool would cause writing in both datapools
            this->mpc_Ui->pc_TreeView->RemoveValuesChangedFlag(c_InterestingChangedElements);
         }
         else
         {
            C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent(),
                                                 C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("System Parametrization"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText(
                                              "At least one of the selected lists contains invalid set"
                                              " values. Enter valid set values and retry."));
            c_MessageResult.SetCustomMinHeight(180, 180);
            c_MessageResult.Execute();
         }
      }
      else
      {
         C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent());
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("System Parametrization"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText(
                                           "Nothing to do for selected lists. Values on target device are up to date."));
         c_MessageResult.SetCustomMinHeight(180, 180);
         c_MessageResult.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle load action for specified list IDs

   \param[in]  orc_ElementIds    Element IDs to load for
   \param[in]  orc_Id            Trigger source ID
   \param[in]  ou32_ValidLayers  Number of valid layers in trigger source ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_LoadElements(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ElementIds,
                                          const C_OSCNodeDataPoolListElementId & orc_Id,
                                          const uint32 ou32_ValidLayers) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
      if (pc_ParamWidget != NULL)
      {
         QString c_Folder;
         QString c_File;
         const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("openSYDE Parameter Set File")) + " (*" +
                                  mhc_FILE_EXTENSION_PARAMSET + ")";

         //User settings restore
         c_Folder = C_Uti::h_CheckAndReplaceWithExePathIfNecessary(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                                      pc_View->GetName()).c_ParamImportPath);

         c_File = C_OgeWiUtil::h_GetOpenFileName(pc_ParamWidget->GetPopUpParent(),
                                                 C_GtGetText::h_GetText("Load Parameter Set File"),
                                                 c_Folder, c_Filter, mhc_FILE_EXTENSION_PARAMSET);

         if (c_File.compare("") != 0)
         {
            C_OSCParamSetHandler c_ParamSetHandler;
            sint32 s32_Result;

            //User settings store
            C_UsHandler::h_GetInstance()->SetProjSvParamImport(pc_View->GetName(), c_File);

            s32_Result = c_ParamSetHandler.ReadFile(c_File.toStdString().c_str(), true, true);

            if (s32_Result == C_NO_ERR)
            {
               const QSize c_SIZE_IMPORT_REPORT(1210, 790);
               const C_OSCParamSetInterpretedData & rc_Data = c_ParamSetHandler.GetInterpretedData();
               std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> c_FloatRangeCheckInvalidValueIds;
               std::vector<QString> c_FloatRangeCheckInvalidValues;
               std::vector<QString> c_FloatRangeCheckNewValues;

               QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(
                  pc_ParamWidget->GetPopUpParent(), pc_ParamWidget->GetPopUpParent());
               C_SyvDaItPaImportReport * const pc_Dialog = new C_SyvDaItPaImportReport(*c_New,
                                                                                       rc_Data.c_InterpretedNodes,
                                                                                       orc_ElementIds,
                                                                                       orc_Id, ou32_ValidLayers,
                                                                                       c_File);
               Q_UNUSED(pc_Dialog)

               //Resize
               c_New->SetSize(c_SIZE_IMPORT_REPORT);

               pc_Dialog->GetFloatRangeCheckResults(c_FloatRangeCheckInvalidValueIds, c_FloatRangeCheckInvalidValues,
                                                    c_FloatRangeCheckNewValues);
               this->m_InformUserFloatRangeCheck(c_FloatRangeCheckInvalidValueIds, c_FloatRangeCheckInvalidValues,
                                                 c_FloatRangeCheckNewValues);

               if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
               {
                  std::vector<C_OSCNodeDataPoolListElementId> c_OutputListIds;
                  std::vector<C_OSCNodeDataPoolContent> c_OutputContent;
                  pc_Dialog->GetOutput(c_OutputListIds, c_OutputContent);
                  //only accept if output formatted as expected
                  if (c_OutputListIds.size() == c_OutputContent.size())
                  {
                     //prepare data copy
                     const C_PuiSvDbParam * const pc_ParamData = pc_ParamWidget->GetParamItem();
                     if (pc_ParamData != NULL)
                     {
                        C_PuiSvDbParam c_Copy = *pc_ParamData;
                        //For each output entry
                        for (uint32 u32_ItOutput = 0; u32_ItOutput < c_OutputListIds.size(); ++u32_ItOutput)
                        {
                           //Search in param widget
                           for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_ParamData->c_DataPoolElementsConfig.size();
                                ++u32_ItConfig)
                           {
                              const C_PuiSvDbNodeDataElementConfig & rc_Config =
                                 pc_ParamData->c_DataPoolElementsConfig[u32_ItConfig];
                              if (rc_Config.c_ElementId.GetIsValid() == true)
                              {
                                 if ((c_OutputListIds[u32_ItOutput] == rc_Config.c_ElementId) &&
                                     (u32_ItConfig < c_Copy.c_ListValues.size()))
                                 {
                                    //replace if correct entry found
                                    C_OSCNodeDataPoolContent & rc_Content = c_Copy.c_ListValues[u32_ItConfig];
                                    rc_Content = c_OutputContent[u32_ItOutput];
                                 }
                              }
                           }
                        }
                        tgl_assert(pc_ParamWidget->SetParamItem(c_Copy) == C_NO_ERR);
                        // Reload the widget with the new configuration
                        this->mpc_Ui->pc_TreeView->ReloadSetValues();
                     }
                  }
               }

               if (c_New != NULL)
               {
                  c_New->HideOverlay();
                  c_New->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
            else
            {
               QString c_Details;
               C_OgeWiCustomMessage c_MessageResultRead(
                  pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eERROR);

               C_OSCLoggingHandler::h_Flush();

               c_MessageResultRead.SetHeading(C_GtGetText::h_GetText("Import configuration"));
               c_MessageResultRead.SetDescription(C_GtGetText::h_GetText("Could not read the file."));
               c_Details = C_GtGetText::h_GetText(
                  "Possible reasons:<br/>"
                  "- Invalid xml format (e.g. wrong filetype selected)<br/>"
                  "- Invalid xml version (e.g. version incompatible with current tool version)<br/>"
                  "- Invalid content (e.g. values invalid)<br/>");
               c_Details += C_GtGetText::h_GetText("For more information see ");
               c_Details += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                             C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str());
               c_Details += ".";
               c_MessageResultRead.SetDetails(c_Details);
               c_MessageResultRead.SetCustomMinHeight(300, 300);
               c_MessageResultRead.Execute();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle save action for specified list IDs

   \param[in]  orc_ListIds       List IDs to save for
   \param[in]  orc_Id            Trigger source ID
   \param[in]  ou32_ValidLayers  Number of valid layers in trigger source ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_SaveElements(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds,
                                          const C_OSCNodeDataPoolListElementId & orc_Id,
                                          const uint32 ou32_ValidLayers) const
{
   sint32 s32_Result = C_NO_ERR;
   uint32 u32_ListCounter;
   uint32 u32_IntNodeCounter;
   C_OSCParamSetHandler c_ParamSetFileHandler;

   std::vector<C_OSCParamSetInterpretedNode> c_IntNodes;
   std::vector<uint32> c_UsedNodeIndices;
   // For each node which is synchronous to c_UsedNodeIndices
   std::vector<std::vector<uint32> > c_UsedDataPoolIndices;
   const C_OSCParamSetInterpretedFileInfoData c_FileInfo = C_SyvDaItPaImageRecordWidget::h_GetFileInfoData("");

   if (orc_ListIds.size() == 0)
   {
      return;
   }

   //General info
   c_ParamSetFileHandler.AddInterpretedFileData(c_FileInfo);

   // Convert all necessary information to the interpreted node format
   for (u32_ListCounter = 0U; u32_ListCounter < orc_ListIds.size(); ++u32_ListCounter)
   {
      const C_OSCNodeDataPoolListElementId & rc_CurElementId = orc_ListIds[u32_ListCounter];
      const C_OSCNode * const pc_SdNode =
         C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(rc_CurElementId.u32_NodeIndex);
      if ((pc_SdNode != NULL) &&
          (rc_CurElementId.u32_DataPoolIndex < pc_SdNode->c_DataPools.size()) &&
          (rc_CurElementId.u32_ListIndex < pc_SdNode->c_DataPools[rc_CurElementId.u32_DataPoolIndex].c_Lists.size()))
      {
         const C_OSCNodeDataPool & rc_SdDataPool = pc_SdNode->c_DataPools[rc_CurElementId.u32_DataPoolIndex];
         const C_OSCNodeDataPoolList & rc_SdList = rc_SdDataPool.c_Lists[rc_CurElementId.u32_ListIndex];
         C_OSCParamSetInterpretedNode * pc_CurIntNode = NULL;
         C_OSCParamSetInterpretedList c_NewIntList;
         uint32 u32_CurIntNodeIndex;
         uint32 u32_CurIntDataPoolIndex;
         uint32 u32_ElementCounter;
         bool q_NodeFound = false;
         std::vector<C_OSCNodeDataPoolContent> c_ListValues;

         // Was the node already added
         tgl_assert(c_UsedNodeIndices.size() == c_IntNodes.size());
         for (u32_CurIntNodeIndex = 0U; u32_CurIntNodeIndex < c_UsedNodeIndices.size(); ++u32_CurIntNodeIndex)
         {
            if (c_UsedNodeIndices[u32_CurIntNodeIndex] == rc_CurElementId.u32_NodeIndex)
            {
               pc_CurIntNode = &c_IntNodes[u32_CurIntNodeIndex];
               q_NodeFound = true;
               break;
            }
         }

         if (q_NodeFound == false)
         {
            // New interpreted node necessary
            c_UsedNodeIndices.push_back(rc_CurElementId.u32_NodeIndex);
            c_IntNodes.resize(c_IntNodes.size() + 1);
            c_UsedDataPoolIndices.resize(c_IntNodes.size());
            u32_CurIntNodeIndex = static_cast<uint32>(c_IntNodes.size() - 1);
            pc_CurIntNode = &c_IntNodes[u32_CurIntNodeIndex];

            // Fill node information
            pc_CurIntNode->c_Name = pc_SdNode->c_Properties.c_Name;
         }

         tgl_assert(pc_CurIntNode != NULL);
         if (pc_CurIntNode != NULL)
         {
            bool q_DataPoolFound = false;
            C_OSCParamSetInterpretedDataPool * pc_CurIntDataPool = NULL;
            // Was the datapool already added
            tgl_assert(c_UsedDataPoolIndices.size() == c_IntNodes.size());
            for (u32_CurIntDataPoolIndex = 0U;
                 u32_CurIntDataPoolIndex < c_UsedDataPoolIndices[u32_CurIntNodeIndex].size();
                 ++u32_CurIntDataPoolIndex)
            {
               if (c_UsedDataPoolIndices[u32_CurIntNodeIndex][u32_CurIntDataPoolIndex] ==
                   rc_CurElementId.u32_DataPoolIndex)
               {
                  tgl_assert(u32_CurIntDataPoolIndex < c_IntNodes[u32_CurIntNodeIndex].c_DataPools.size());
                  pc_CurIntDataPool = &c_IntNodes[u32_CurIntNodeIndex].c_DataPools[u32_CurIntDataPoolIndex];
                  q_DataPoolFound = true;
                  break;
               }
            }

            if (q_DataPoolFound == false)
            {
               // New interpreted datapool necessary
               c_UsedDataPoolIndices[u32_CurIntNodeIndex].push_back(rc_CurElementId.u32_DataPoolIndex);
               pc_CurIntNode->c_DataPools.resize(pc_CurIntNode->c_DataPools.size() + 1);
               u32_CurIntDataPoolIndex = static_cast<uint32>((pc_CurIntNode->c_DataPools.size() - 1));
               pc_CurIntDataPool = &pc_CurIntNode->c_DataPools[u32_CurIntDataPoolIndex];

               // Fill datapool information
               pc_CurIntDataPool->c_DataPoolInfo.c_Name = rc_SdDataPool.c_Name;
               rc_SdDataPool.CalcDefinitionHash(pc_CurIntDataPool->c_DataPoolInfo.u32_DataPoolCrc);
               memcpy(pc_CurIntDataPool->c_DataPoolInfo.au8_Version, rc_SdDataPool.au8_Version, 3);
            }

            // Prepare the list
            c_NewIntList.c_Name = rc_SdList.c_Name;
            c_ListValues.clear();
            // Get the current values of the list for each element
            this->mpc_Ui->pc_TreeView->GetListSetValues(orc_ListIds[u32_ListCounter], c_ListValues);

            tgl_assert(c_ListValues.size() == rc_SdList.c_Elements.size());
            if (c_ListValues.size() == rc_SdList.c_Elements.size())
            {
               // Add all elements
               for (u32_ElementCounter = 0U; u32_ElementCounter < rc_SdList.c_Elements.size(); ++u32_ElementCounter)
               {
                  C_OSCParamSetInterpretedElement c_IntNewElement;

                  c_IntNewElement.c_Name = rc_SdList.c_Elements[u32_ElementCounter].c_Name;

                  // We need the actual content of the list
                  c_IntNewElement.c_NvmValue = c_ListValues[u32_ElementCounter];

                  c_NewIntList.c_Elements.push_back(c_IntNewElement);
               }

               // Add the list
               tgl_assert(pc_CurIntDataPool != NULL);
               if (pc_CurIntDataPool != NULL)
               {
                  pc_CurIntDataPool->c_Lists.push_back(c_NewIntList);
               }
            }
         }
      }
      else
      {
         s32_Result = C_CONFIG;
      }

      if (s32_Result != C_NO_ERR)
      {
         break;
      }
   }

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
   if (pc_ParamWidget != NULL)
   {
      if (s32_Result == C_NO_ERR)
      {
         // Prepare the param set handler
         for (u32_IntNodeCounter = 0U; u32_IntNodeCounter < c_IntNodes.size(); ++u32_IntNodeCounter)
         {
            s32_Result = c_ParamSetFileHandler.AddInterpretedDataForNode(c_IntNodes[u32_IntNodeCounter]);

            if (s32_Result != C_NO_ERR)
            {
               break;
            }
         }

         if (s32_Result == C_NO_ERR)
         {
            const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
            if (pc_View != NULL)
            {
               // Save the file
               QString c_Folder;
               QString c_FileName;
               const C_UsSystemView c_View = C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                  pc_View->GetName());

               //User settings restore
               c_Folder = C_Uti::h_CheckAndReplaceWithExePathIfNecessary(c_View.c_ParamExportPath);
               {
                  //prepare default name
                  const QString c_File = mh_GetDefaultFileName(this->mu32_ViewIndex, orc_Id, ou32_ValidLayers);
                  //Get base folder
                  const QFileInfo c_Info(c_Folder);
                  if (c_Info.isFile() == true)
                  {
                     c_Folder = c_Info.dir().absolutePath();
                  }
                  //Append default file name
                  c_Folder += static_cast<QString>("/%1%2").arg(c_File, mhc_FILE_EXTENSION_PARAMSET);
               }

               c_FileName = C_OgeWiUtil::h_GetSaveFileName(pc_ParamWidget->GetPopUpParent(),
                                                           C_GtGetText::h_GetText("Save Parameter Set File"),
                                                           c_Folder,
                                                           static_cast<QString>(C_GtGetText::h_GetText(
                                                                                   "openSYDE Parameter Set File")) +
                                                           " (*" + mhc_FILE_EXTENSION_PARAMSET + ")", "");

               if (c_FileName.compare("") != 0)
               {
                  // Check if file exists already
                  QFile c_File;
                  bool q_Continue = true;

                  //User settings store
                  C_UsHandler::h_GetInstance()->SetProjSvParamExport(pc_View->GetName(), c_FileName);

                  // Remove old file
                  c_File.setFileName(c_FileName);
                  if (c_File.exists() == true)
                  {
                     q_Continue = c_File.remove();
                  }

                  if (q_Continue == true)
                  {
                     s32_Result = c_ParamSetFileHandler.CreateCleanFileWithoutCRC(
                        c_FileName.toStdString().c_str(), true);
                  }
                  else
                  {
                     s32_Result = C_RD_WR;
                  }

                  if (s32_Result != C_NO_ERR)
                  {
                     // TODO: Error handling
                     C_OgeWiCustomMessage c_MessageResultSave(
                        pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eERROR);
                     c_MessageResultSave.SetHeading(C_GtGetText::h_GetText("Configuration export"));
                     c_MessageResultSave.SetDescription(C_GtGetText::h_GetText("Could not save the file."));
                     c_MessageResultSave.SetDetails(C_GtGetText::h_GetText("Error code: \n") +
                                                    QString::number(s32_Result));
                     c_MessageResultSave.SetCustomMinHeight(180, 250);
                     c_MessageResultSave.Execute();
                  }
                  else
                  {
                     const QString c_Details =
                        static_cast<QString>("File saved at: %1").arg(C_Uti::h_GetLink(c_FileName,
                                                                                       mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                                                       "file:" + c_FileName));
                     C_OgeWiCustomMessage c_MessageResultSave(
                        pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eINFORMATION);
                     c_MessageResultSave.SetHeading(C_GtGetText::h_GetText("Configuration export"));
                     c_MessageResultSave.SetDescription(C_GtGetText::h_GetText(
                                                           "Parameter Set File successfully created."));
                     c_MessageResultSave.SetDetails(c_Details);
                     c_MessageResultSave.SetCustomMinHeight(180, 250);
                     c_MessageResultSave.Execute();
                  }
               }
            }
         }
         else
         {
            // TODO: Error handling
            C_OgeWiCustomMessage c_MessageResult(pc_ParamWidget->GetPopUpParent(),
                                                 C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageResult.SetHeading(C_GtGetText::h_GetText("Configuration export"));
            c_MessageResult.SetDescription(C_GtGetText::h_GetText("Could not create the file!"));
            c_MessageResult.SetDetails(C_GtGetText::h_GetText("Error code: \n") + QString::number(s32_Result));
            c_MessageResult.SetCustomMinHeight(180, 250);
            c_MessageResult.Execute();
         }
      }
      else
      {
         // TODO: Error handling
         C_OgeWiCustomMessage c_MessageResultCreate(
            pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResultCreate.SetHeading(C_GtGetText::h_GetText("Configuration export"));
         c_MessageResultCreate.SetDescription(C_GtGetText::h_GetText("Could not create the file!"));
         c_MessageResultCreate.SetDetails(C_GtGetText::h_GetText("Error code: \n") + QString::number(s32_Result));
         c_MessageResultCreate.SetCustomMinHeight(180, 250);
         c_MessageResultCreate.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle record action for specified list IDs

   \param[in]  orc_ListIds    List IDs to record for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_RecordElements(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      //Should only be possible if online
      tgl_assert(this->mpc_ComDriver != NULL);
      if ((pc_View != NULL) && (this->mpc_ComDriver != NULL))
      {
         QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(
            pc_ParamWidget->GetPopUpParent(), pc_ParamWidget->GetPopUpParent());
         C_SyvDaItPaImageRecordWidget * const pc_Dialog = new C_SyvDaItPaImageRecordWidget(*c_New, *this->mpc_ComDriver,
                                                                                           orc_ListIds,
                                                                                           pc_View->GetName());

         Q_UNUSED(pc_Dialog)

         //Resize
         c_New->SetSize(QSize(900, 801));

         if (c_New->exec() != static_cast<sintn>(QDialog::Accepted))
         {
            // Error or abortion of user
         }

         if (c_New != NULL)
         {
            pc_Dialog->SaveUserSettings();
            c_New->HideOverlay();
            c_New->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selection items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_UpdateButtons(void)
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == false)
   {
      //Buttons
      if (this->mq_IsConnected == false)
      {
         m_UpdateReadAllowedFlag(false);
      }
      else
      {
         m_UpdateReadAllowedFlag(true);
      }
   }
   else
   {
      //Buttons
      m_UpdateReadAllowedFlag(false);
   }
   m_UpdateButtonToolTips();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dynamic button tool tips
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_UpdateButtonToolTips(void) const
{
   const QString c_ReasonEditMode =
      C_GtGetText::h_GetText(
         "Disabled because this action is only allowed in edit content mode activated by the context menu.");
   QString c_Heading;
   QString c_Description;

   c_Heading = C_GtGetText::h_GetText("Add List");
   if (this->mpc_Ui->pc_PushButtonAdd->isEnabled() == true)
   {
      c_Description = "";
   }
   else
   {
      c_Description = c_ReasonEditMode;
   }
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(c_Heading, c_Description);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle tree reinit action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_HandleTreeInitAction(void)
{
   this->mpc_Ui->pc_TreeView->Init(this->mpc_DataWidget);
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_GroupBoxTree->setVisible(false);
      this->mpc_Ui->pc_GroupBoxTreeEmpty->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxTree->setVisible(true);
      this->mpc_Ui->pc_GroupBoxTreeEmpty->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   handle the tree read action

   \param[in]  orc_ListIds    Current list IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_HandleTreeReadAction(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   //Deactivate any left over active write actions
   this->mq_WriteActive = false;
   //Start read
   this->mc_ListIds = orc_ListIds;
   m_ReadElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle list IDs write trigger

   \param[in]  orc_ListIds    List IDs to write
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_HandleWriteProcessTrigger(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   if (this->mpc_Ui->pc_TreeView->CheckListsRead(orc_ListIds) == true)
   {
      m_WriteElements(orc_ListIds);
   }
   else
   {
      //Activate flag to continue write after read
      this->mq_WriteActive = true;
      this->mc_ElementsToWriteAfterRead = orc_ListIds;
      //Start read
      this->mc_ListIds = orc_ListIds;
      m_ReadElements();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete the specified list IDs

   \param[in]  orc_ListIds    List IDs to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_HandleRemoveTrigger(const std::vector<C_OSCNodeDataPoolListElementId> & orc_ListIds)
{
   C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
   if (pc_ParamWidget != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parametrization Widget delete lists"));
      c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Do you really want to delete %1 list(s) of"
                                                          " the Parametrization Widget?")).
                                  arg(orc_ListIds.size()));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
      {
         this->mpc_Ui->pc_TreeView->DeleteSpecified(orc_ListIds);
         //Update selection
         if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
         {
            this->m_HandleTreeInitAction();
         }
         this->m_UpdateButtons();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the read allowed flag

   \param[in]  oq_NewValue    Flag if read action is allowed now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_UpdateReadAllowedFlag(const bool oq_NewValue)
{
   this->mq_ReadAllowed = oq_NewValue;
   this->mpc_Ui->pc_TreeView->SetActionActive(!oq_NewValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get default file name proposition for specified layer

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID to get default file name for
   \param[in]  ou32_ValidLayers  Layer in ID to get default file name for

   \return
   Default file name proposition for specified layer
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWidgetNew::mh_GetDefaultFileName(const uint32 ou32_ViewIndex,
                                                    const C_OSCNodeDataPoolListElementId & orc_Id,
                                                    const uint32 ou32_ValidLayers)
{
   QString c_Retval;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      const QString c_ViewPart1 = static_cast<QString>(C_GtGetText::h_GetText("View_%1_")).arg(ou32_ViewIndex + 1UL);
      const QString c_ViewPart1File = C_OSCUtils::h_NiceifyStringForFileName(c_ViewPart1.toStdString().c_str()).c_str();
      const QString c_ViewPart2 = C_PuiSdHandler::h_AutomaticCStringAdaptation(pc_View->GetName());
      const QString c_DataElementFileName = mh_GetFile(orc_Id, ou32_ValidLayers);
      const QString c_ViewFileName = static_cast<QString>("%1%2").arg(c_ViewPart1File).arg(c_ViewPart2);
      if (c_DataElementFileName.isEmpty() == false)
      {
         c_Retval = static_cast<QString>("%1_%2").arg(c_ViewFileName).arg(c_DataElementFileName);
      }
      else
      {
         c_Retval = c_ViewFileName;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get file name for index

   \param[in]  orc_Id      ID to get file for
   \param[in]  ou32_Value  Index to get file for

   \return
   Get Name for current file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWidgetNew::mh_GetFile(const C_OSCNodeDataPoolListElementId & orc_Id, const uint32 ou32_Value)
{
   QString c_Retval;

   //Special case: top layer
   if (ou32_Value == 0UL)
   {
      c_Retval = "";
   }
   else if (ou32_Value <= 4UL)
   {
      const C_OSCNode * pc_Node;
      const C_OSCNodeDataPool * pc_DataPool;
      const C_OSCNodeDataPoolList * pc_List;
      const C_OSCNodeDataPoolListElement * pc_Element;
      //Get specific name for current layer
      switch (ou32_Value)
      {
      case 4UL:
         pc_Element = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
            orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex,
            orc_Id.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            c_Retval =
               static_cast<QString>(C_GtGetText::h_GetText("_%1")).arg(C_OSCUtils::h_NiceifyStringForFileName(
                                                                          pc_Element->c_Name.c_str()).c_str());
         }
         break;
      case 3UL:
         pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex);
         if (pc_List != NULL)
         {
            c_Retval =
               static_cast<QString>(C_GtGetText::h_GetText("_%1")).arg(C_OSCUtils::h_NiceifyStringForFileName(
                                                                          pc_List->c_Name.c_str()).c_str());
         }
         break;
      case 2UL:
         pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_Id.u32_NodeIndex,
                                                                       orc_Id.u32_DataPoolIndex);
         if (pc_DataPool != NULL)
         {
            c_Retval =
               static_cast<QString>(C_GtGetText::h_GetText("_%1")).arg(C_OSCUtils::h_NiceifyStringForFileName(
                                                                          pc_DataPool->c_Name.c_str()).c_str());
         }
         break;
      case 1UL:
         pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Id.u32_NodeIndex);
         if (pc_Node != NULL)
         {
            c_Retval =
               static_cast<QString>(C_GtGetText::h_GetText("%1")).arg(C_OSCUtils::h_NiceifyStringForFileName(
                                                                         pc_Node->c_Properties.c_Name.c_str()).c_str());
         }
         break;
      default:
         break;
      }
      //If not at top prepend previous layer names
      if (ou32_Value > 1UL)
      {
         c_Retval = mh_GetFile(orc_Id, ou32_Value - 1UL) + c_Retval;
      }
   }
   else
   {
      //Invalid
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Inform user about value changes

   \param[in,out]  orc_InvalidValueIds    Invalid value ids
   \param[in,out]  orc_InvalidValues      Invalid values
   \param[in,out]  orc_NewValues          New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWidgetNew::m_InformUserFloatRangeCheck(
   const std::vector<C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds,
   const std::vector<QString> & orc_InvalidValues, const std::vector<QString> & orc_NewValues) const
{
   tgl_assert((orc_InvalidValueIds.size() == orc_InvalidValues.size()) &&
              (orc_InvalidValueIds.size() == orc_NewValues.size()));
   if ((orc_InvalidValueIds.size() == orc_InvalidValues.size()) && (orc_InvalidValueIds.size() == orc_NewValues.size()))
   {
      if (orc_InvalidValueIds.size() > 0UL)
      {
         const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<C_GiSvDaParam * const>(this->mpc_DataWidget);
         if (pc_ParamWidget != NULL)
         {
            QString c_Content;
            C_OgeWiCustomMessage c_Message(pc_ParamWidget->GetPopUpParent(), C_OgeWiCustomMessage::eWARNING);

            c_Message.SetHeading(C_GtGetText::h_GetText("Value changes"));
            c_Message.SetDescription(C_GtGetText::h_GetText(
                                        "Some values were changed due to invalid float values (-inf, inf, nan)"));

            for (uint32 u32_It = 0UL; u32_It < orc_InvalidValueIds.size(); ++u32_It)
            {
               c_Content += static_cast<QString>(C_GtGetText::h_GetText(
                                                    "Changed value of parameter \"%1\" from \"%2\" to \"%3\"\n")).
                            arg(C_PuiSdUtil::h_GetNamespace(orc_InvalidValueIds[u32_It])).
                            arg(orc_InvalidValues[u32_It]).
                            arg(orc_NewValues[u32_It]);
            }

            c_Message.SetDetails(c_Content);
            c_Message.SetCustomMinHeight(180, 350);
            c_Message.SetCustomMinWidth(750);
            c_Message.Execute();
         }
      }
   }
}

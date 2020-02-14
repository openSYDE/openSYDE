//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of writing changed NVM elements to the server.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <list>

#include "C_SyvDaItPaWriteWidget.h"
#include "ui_C_SyvDaItPaWriteWidget.h"

#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OSCLoggingHandler.h"
#include "C_Uti.h"
#include "C_SyvDaItUtil.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_PuiSvDbDataElementHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
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

   \param[in,out]  orc_Parent             Reference to parent
   \param[in]      orc_ComDriver          Reference to the com driver
   \param[in]      orc_ChangedElements    All changed datapool elements
   \param[in]      orc_InvalidLists       All lists to update CRC for
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaWriteWidget::C_SyvDaItPaWriteWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                               C_SyvComDriverDiag & orc_ComDriver,
                                               const std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedElements,
                                               const std::vector<C_OSCNodeDataPoolListId> & orc_InvalidLists) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaItPaWriteWidget),
   mpc_ParentDialog(&orc_Parent),
   mrc_ComDriver(orc_ComDriver),
   me_Step(eBEFOREWRITE),
   mu32_CurrentNode(0U),
   mq_RequestPending(false)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   //Register close
   this->mpc_ParentDialog->SetNotifyAndBlockClose(true);

   // init gui
   this->mpc_Ui->pc_BopperleWrite->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
   this->mpc_Ui->pc_LabelRead->setEnabled(false);
   this->mpc_Ui->pc_LabelConfirm->setEnabled(false);
   this->mpc_Ui->pc_LabelCrc->setEnabled(false);
   this->mpc_Ui->pc_LabelFinished->setEnabled(false);
   this->mpc_Ui->pc_CbConfirm->setVisible(false);
   this->mpc_Ui->pc_WiNotification->setEnabled(false);
   this->mpc_Ui->pc_WiNotification->setVisible(false);
   this->mpc_Ui->pc_BushButtonOk->setVisible(false);

   this->InitText();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDaItPaWriteWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDaItPaWriteWidget::m_OnCancel);
   connect(this->mpc_Ui->pc_PbConfirm, &QPushButton::clicked, this, &C_SyvDaItPaWriteWidget::m_ConfirmClicked);
   connect(this->mpc_ParentDialog, &C_OgePopUpDialog::SigCloseIgnored, this,
           &C_SyvDaItPaWriteWidget::m_OnCancel);
   connect(this->mpc_Ui->pc_CbConfirm, &stw_opensyde_gui_elements::C_OgeChxProperties::stateChanged, this,
           &C_SyvDaItPaWriteWidget::m_ConfirmCheckBoxChanged);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDaItPaWriteWidget::m_Timer);
   this->mc_Timer.setInterval(10);

   // start writing of elements
   this->m_StartWriteChangedElements(orc_ChangedElements, orc_InvalidLists);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaWriteWidget::~C_SyvDaItPaWriteWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::InitText(void)
{
   // set title
   this->mpc_ParentDialog->SetTitle(QString(C_GtGetText::h_GetText("Parametrization")));
   this->mpc_ParentDialog->SetSubTitle(QString(C_GtGetText::h_GetText("Write Parameters to System")));

   this->mpc_Ui->pc_LabelFinished->setText(QString(C_GtGetText::h_GetText("Finished")));
   this->mpc_Ui->pc_LabelConfirm->setText(QString(C_GtGetText::h_GetText("Confirm")));
   this->mpc_Ui->pc_LabelWrite->setText(QString(C_GtGetText::h_GetText("Write")));
   this->mpc_Ui->pc_LabelRead->setText(QString(C_GtGetText::h_GetText("Read")));
   this->mpc_Ui->pc_LabelCrc->setText(QString(C_GtGetText::h_GetText("Write CRC")));

   this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Write Parameter Values"));
   this->mpc_Ui->pc_LabelStepDescription->setText(
      C_GtGetText::h_GetText("The following parameter values will be written to NVM memory. "
                             "Click \"Write\" to start this process."));

   this->mpc_Ui->pc_LabelHeadingConfirm->setText(QString(C_GtGetText::h_GetText("Parameters")));
   this->mpc_Ui->pc_LabelHeadingNotification->setText(QString(C_GtGetText::h_GetText("Application Notification")));
   this->mpc_Ui->pc_PbConfirm->setText(QString(C_GtGetText::h_GetText("Write")));
   this->mpc_Ui->pc_CbConfirm->setText(QString(C_GtGetText::h_GetText("Confirmed, parameter values are valid")));
   this->mpc_Ui->pc_CbNotifiction->setText(QString(C_GtGetText::h_GetText("Trigger Application Notification")));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current step

   \return
   Current step
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaWriteWidget::E_Step C_SyvDaItPaWriteWidget::GetStep(void) const
{
   return this->me_Step;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns all changed elements with successful adapted CRCs

   \param[out]  orc_AffectedElementValues    All possibly changed elements

   \return
   C_NO_ERR    Process was finished successful and all changed element ids returned
   C_RD_WR     An error occurred. The data on the server may not be valid.
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItPaWriteWidget::GetChangedElements(std::map<C_OSCNodeDataPoolListElementId,
                                                           C_SyvDaItPaValuePairs> & orc_AffectedElementValues) const
{
   sint32 s32_Return = C_RD_WR;

   orc_AffectedElementValues.clear();

   if (this->me_Step == eFINISHED)
   {
      orc_AffectedElementValues = this->mc_AllAffectedValues;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle escape key

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<sintn>(Qt::Key_Escape))
   {
      m_OnCancel();
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start of writing all changed elements

   Prepares step 1.

   \param[in]  orc_ChangedElements  All changed datapool elements (Is used to detect the affected nodes)
   \param[in]  orc_InvalidLists     All lists to update CRC for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_StartWriteChangedElements(
   const std::vector<C_OSCNodeDataPoolListElementId> & orc_ChangedElements,
   const std::vector<C_OSCNodeDataPoolListId> & orc_InvalidLists)
{
   // Prepare the writing for each node
   uint32 u32_Counter;

   this->mc_AllNodeIndexes.clear();
   this->mc_ReadNodeValues.clear();
   this->mc_ChangedElementsForEachNode.clear();
   this->mc_RelevantLists.clear();
   this->mc_InvalidListsForEachNode.clear();

   this->mu32_CurrentNode = 0U;

   for (u32_Counter = 0U; u32_Counter < orc_ChangedElements.size(); ++u32_Counter)
   {
      const C_OSCNodeDataPoolListElementId & rc_Id = orc_ChangedElements[u32_Counter];
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         rc_Id.u32_NodeIndex, rc_Id.u32_DataPoolIndex, rc_Id.u32_ListIndex);
      // Check if a changed element for this node was registered already
      uint32 u32_NodeCounter;
      bool q_NodeFound = false;

      for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_AllNodeIndexes.size(); ++u32_NodeCounter)
      {
         if (this->mc_AllNodeIndexes[u32_NodeCounter] == orc_ChangedElements[u32_Counter].u32_NodeIndex)
         {
            q_NodeFound = true;
            break;
         }
      }

      if (q_NodeFound == false)
      {
         // Both indexes must be synchronous
         // Add the node index
         this->mc_AllNodeIndexes.push_back(orc_ChangedElements[u32_Counter].u32_NodeIndex);
         //  Adapt size of vector for each node
         this->mc_ReadNodeValues.resize(this->mc_ReadNodeValues.size() + 1);
         this->mc_ChangedElementsForEachNode.resize(this->mc_ChangedElementsForEachNode.size() + 1);
         //Add vector for each node
         this->mc_InvalidListsForEachNode.resize(this->mc_InvalidListsForEachNode.size() + 1);
      }

      // Save all lists
      this->mc_RelevantLists.insert(static_cast<C_OSCNodeDataPoolListId>(orc_ChangedElements[u32_Counter]));

      //Parse original values
      if (pc_List != NULL)
      {
         for (uint32 u32_ItElement = 0UL; u32_ItElement < pc_List->c_Elements.size(); ++u32_ItElement)
         {
            const bool q_Changed = (u32_ItElement == rc_Id.u32_ElementIndex);
            const C_OSCNodeDataPoolListElementId c_Id(rc_Id.u32_NodeIndex, rc_Id.u32_DataPoolIndex,
                                                      rc_Id.u32_ListIndex, u32_ItElement);
            //Add if not containing or if it is more important (changed!)
            if ((this->mc_AllAffectedValues.find(c_Id) == this->mc_AllAffectedValues.end()) ||
                (q_Changed == true))
            {
               const C_OSCNodeDataPoolListElement & rc_Element = pc_List->c_Elements[u32_ItElement];
               C_SyvDaItPaValuePairs c_Pair(rc_Element.c_NvmValue, q_Changed);
               this->mc_AllAffectedValues[c_Id] = c_Pair;
            }
         }
      }
   }

   for (u32_Counter = 0U; u32_Counter < orc_InvalidLists.size(); ++u32_Counter)
   {
      const C_OSCNodeDataPoolListId & rc_CurListId = orc_InvalidLists[u32_Counter];
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         rc_CurListId.u32_NodeIndex, rc_CurListId.u32_DataPoolIndex, rc_CurListId.u32_ListIndex);
      uint32 u32_NodeCounter;
      bool q_NodeFound = false;

      for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_AllNodeIndexes.size(); ++u32_NodeCounter)
      {
         if (this->mc_AllNodeIndexes[u32_NodeCounter] == rc_CurListId.u32_NodeIndex)
         {
            q_NodeFound = true;
            break;
         }
      }

      if (q_NodeFound == false)
      {
         std::vector<C_OSCNodeDataPoolListId> c_NewList;
         c_NewList.push_back(rc_CurListId);
         // Both indexes must be synchronous
         // Add the node index
         this->mc_AllNodeIndexes.push_back(rc_CurListId.u32_NodeIndex);
         //  Adapt size of vector for each node
         this->mc_ReadNodeValues.resize(this->mc_ReadNodeValues.size() + 1);
         this->mc_ChangedElementsForEachNode.resize(this->mc_ChangedElementsForEachNode.size() + 1);
         //Add vector for each node
         this->mc_InvalidListsForEachNode.push_back(c_NewList);
      }
      else
      {
         this->mc_InvalidListsForEachNode[u32_NodeCounter].push_back(rc_CurListId);
      }
      // Save all lists
      this->mc_RelevantLists.insert(rc_CurListId);

      //Parse original values
      if (pc_List != NULL)
      {
         for (uint32 u32_ItElement = 0UL; u32_ItElement < pc_List->c_Elements.size(); ++u32_ItElement)
         {
            const C_OSCNodeDataPoolListElementId c_ElementId(rc_CurListId.u32_NodeIndex,
                                                             rc_CurListId.u32_DataPoolIndex,
                                                             rc_CurListId.u32_ListIndex, u32_ItElement);
            //Only add if not already containing
            if (this->mc_AllAffectedValues.find(c_ElementId) == this->mc_AllAffectedValues.end())
            {
               const C_OSCNodeDataPoolListElement & rc_Element = pc_List->c_Elements[u32_ItElement];
               C_SyvDaItPaValuePairs c_Pair(rc_Element.c_NvmValue, false);
               this->mc_AllAffectedValues[c_ElementId] = c_Pair;
            }
         }
      }
   }

   // Start the timer
   this->mc_Timer.start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Take a break before writing values to nvm.

   Implemented steps: 0
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_PrepareForWrite(void)
{
   // all the GUI stuff is already done in constructor (because first state is default state)
   this->mc_Timer.stop(); // wait for click on "Write"-Button
   m_ShowParameterValues(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write all changed elements

   Implemented steps: 1
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_WriteChangedElementsOfNode(void)
{
   if (this->mu32_CurrentNode < this->mc_AllNodeIndexes.size())
   {
      sint32 s32_Return;

      if (this->mq_RequestPending == false)
      {
         s32_Return =
            this->mrc_ComDriver.PollSafeNvmWriteChangedElements(this->mc_AllNodeIndexes[this->mu32_CurrentNode],
                                                                this->mc_InvalidListsForEachNode[this->mu32_CurrentNode]);

         if (s32_Return == C_NO_ERR)
         {
            this->mq_RequestPending = true;
         }
         else if (s32_Return == C_RANGE)
         {
            // Error
            this->mc_Timer.stop();
            this->m_ReportError("NvmSafeWriteChangedValues", "Wrong node index.", s32_Return);
         }
         else
         {
            // On C_BUSY is nothing to do
         }
      }
      else
      {
         sint32 s32_ServiceResult;
         s32_Return = this->mrc_ComDriver.GetPollResults(s32_ServiceResult);

         if (s32_Return == C_NO_ERR)
         {
            // Thread finished
            if (s32_ServiceResult == C_NO_ERR)
            {
               // Get the output parameter
               this->mrc_ComDriver.GetPollSafeNvmWriteChangedElementsOutput(
                  this->mc_ChangedElementsForEachNode[this->mu32_CurrentNode]);

               // Prepare the next node
               this->mq_RequestPending = false;
               ++this->mu32_CurrentNode;

               if (this->mu32_CurrentNode >= this->mc_AllNodeIndexes.size())
               {
                  // No further nodes
                  //                  this->mc_Timer.thread()->sleep(2);
                  this->mpc_Ui->pc_ProgressWrite->SetProgress(100);
                  this->mu32_CurrentNode = 0U;
                  this->mpc_Ui->pc_LabelRead->setEnabled(true);
                  this->mpc_Ui->pc_BopperleReadBack->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21,
                                                                          mc_STYLE_GUIDE_COLOR_13);
                  this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Read Back"));
                  this->mpc_Ui->pc_LabelStepDescription->setText(
                     C_GtGetText::h_GetText("Reading back the read parameter values for verification... "));
                  this->mpc_Ui->pc_CbConfirm->setVisible(false);
                  this->mpc_Ui->pc_PbConfirm->setVisible(false);
                  this->me_Step = eREADBACK;
               }
               else
               {
                  this->mpc_Ui->pc_ProgressWrite->SetProgress((this->mu32_CurrentNode * 100U) /
                                                              this->mc_AllNodeIndexes.size());
               }
            }
            else
            {
               // Error of service
               this->mc_Timer.stop();
               m_ReportErrorNvmSafeWriteChangedValues(s32_ServiceResult);
            }
         }
      }
   }
   else
   {
      // Error of service
      this->mc_Timer.stop();
      this->m_ReportError("NvmSafeWriteChangedValues", "No changed elements found.", C_RANGE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads the relevant lists back

   Implemented steps: 2, 3
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReadBackElementsOfNode(void)
{
   if (this->mu32_CurrentNode < this->mc_AllNodeIndexes.size())
   {
      sint32 s32_Return;

      if (this->mq_RequestPending == false)
      {
         s32_Return = this->mrc_ComDriver.PollSafeNvmReadValues(this->mc_AllNodeIndexes[this->mu32_CurrentNode]);

         if (s32_Return == C_NO_ERR)
         {
            this->mq_RequestPending = true;
         }
         else if (s32_Return == C_RANGE)
         {
            // Error
            this->mc_Timer.stop();
            this->m_ReportError("NvmSafeReadValues", "Wrong node index.", s32_Return);
         }
         else
         {
            // On C_BUSY is nothing to do
         }
      }
      else
      {
         sint32 s32_ServiceResult;
         s32_Return = this->mrc_ComDriver.GetPollResults(s32_ServiceResult);

         if (s32_Return == C_NO_ERR)
         {
            // Thread finished
            if (s32_ServiceResult == C_NO_ERR)
            {
               // Get the output parameter
               this->mrc_ComDriver.GetPollNvmSafeReadValuesOutput(this->mc_ReadNodeValues[this->mu32_CurrentNode]);

               // Prepare the next node
               this->mq_RequestPending = false;
               ++this->mu32_CurrentNode;

               if (this->mu32_CurrentNode >= this->mc_AllNodeIndexes.size())
               {
                  QString c_SuspectElementsReport;
                  // No further nodes
                  this->mpc_Ui->pc_ProgressRead->SetProgress(100);
                  this->mu32_CurrentNode = 0U;
                  this->mc_Timer.stop();
                  this->mpc_Ui->pc_WiConfirm->setEnabled(true);
                  this->mpc_Ui->pc_LabelConfirm->setEnabled(true);
                  this->mpc_Ui->pc_BopperleConfirm->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21,
                                                                         mc_STYLE_GUIDE_COLOR_13);
                  this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Confirm"));
                  this->mpc_Ui->pc_LabelStepDescription->setText(
                     C_GtGetText::h_GetText(
                        "Parameter values have been written to NVM memory. Verify the read back values. "
                        "Activate \"Parameter values are valid\" when the values are OK and click \"Continue\". "
                        "If list checksum is enabled in list definition, the checksum will be updated and the "
                        "NVM data is marked as valid. "));
                  this->mpc_Ui->pc_PbConfirm->setText(QString(C_GtGetText::h_GetText("Continue")));
                  this->mpc_Ui->pc_CbConfirm->setVisible(true);
                  this->mpc_Ui->pc_PbConfirm->setVisible(true);
                  this->mpc_Ui->pc_PbConfirm->setEnabled(false);

                  c_SuspectElementsReport = m_ReadAndStoreUpdatedValues();

                  // Step 3
                  this->m_ShowParameterValues(true);
                  this->me_Step = eCONFIRM;
                  //                  this->mc_Timer.thread()->sleep(2);

                  //Mark suspect values
                  if (c_SuspectElementsReport.isEmpty() == false)
                  {
                     C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
                     c_Message.SetHeading(C_GtGetText::h_GetText("Verify parameter values"));
                     c_Message.SetCustomMinWidth(700);
                     c_Message.SetDescription(C_GtGetText::h_GetText(
                                                 "There were some suspect values detected.\n "
                                                 "In these cases the read values from target are different from "
                                                 "the written set values."));
                     c_Message.SetDetails(c_SuspectElementsReport);
                     c_Message.SetCustomMinHeight(230, 300);
                     c_Message.Execute();
                  }
               }
               else
               {
                  this->mpc_Ui->pc_ProgressRead->SetProgress((this->mu32_CurrentNode * 100U) /
                                                             this->mc_AllNodeIndexes.size());
               }
            }
            else
            {
               // Error of service
               this->mc_Timer.stop();
               m_ReportErrorNvmSafeReadValues(s32_ServiceResult);
            }
         }
      }
   }
   else
   {
      // Error of service
      this->mc_Timer.stop();
      this->m_ReportError("NvmSafeReadValues", "No changed elements found.", C_RANGE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Displays the read values

   Implemented steps: 3

   \param[in]  oq_ShowReadValues    Flag if read values are to be displayed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ShowParameterValues(const bool oq_ShowReadValues)
{
   // Show the read list in the confirm area
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListId>::const_iterator c_ItItem;
   // List for sorting all nodes
   std::list<QString> c_NodeText;
   std::list<QString>::const_iterator c_ItNodeText;
   QString c_Text = "";
   uint32 u32_CurNodeIndex = 0xFFFFFFFFU;
   uint32 u32_CurDataPoolIndex = 0xFFFFFFFFU;
   const C_OSCNode * pc_OSCNode = NULL;

   for (c_ItItem = this->mc_RelevantLists.begin(); c_ItItem != this->mc_RelevantLists.end(); ++c_ItItem)
   {
      // Heading for each node
      if (u32_CurNodeIndex != (*c_ItItem).u32_NodeIndex)
      {
         // A new node
         u32_CurNodeIndex = (*c_ItItem).u32_NodeIndex;
         pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_CurNodeIndex);

         if (c_Text != "")
         {
            // Not the first initial node
            c_NodeText.push_back(c_Text);
            //Reset text after node is finished (should only be for one node)
            c_Text = "";
         }

         if (pc_OSCNode != NULL)
         {
            c_Text += "<u>" + QString(C_GtGetText::h_GetText("Node")) + " - " +
                      QString(pc_OSCNode->c_Properties.c_Name.c_str()) + "</u>";
         }
      }

      if (pc_OSCNode != NULL)
      {
         const C_PuiSdNodeDataPoolList * const pc_UIList =
            C_PuiSdHandler::h_GetInstance()->GetUIDataPoolList(u32_CurNodeIndex,
                                                               (*c_ItItem).u32_DataPoolIndex,
                                                               (*c_ItItem).u32_ListIndex);
         const C_OSCNodeDataPoolList * const pc_OSCList =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(u32_CurNodeIndex,
                                                                (*c_ItItem).u32_DataPoolIndex,
                                                                (*c_ItItem).u32_ListIndex);

         if (u32_CurDataPoolIndex != (*c_ItItem).u32_DataPoolIndex)
         {
            // A new datapool
            const C_OSCNodeDataPool * pc_OSCDataPool;
            u32_CurDataPoolIndex = (*c_ItItem).u32_DataPoolIndex;

            pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(u32_CurNodeIndex,
                                                                             u32_CurDataPoolIndex);

            if (pc_OSCDataPool != NULL)
            {
               c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(1UL));
               c_Text += QString(C_GtGetText::h_GetText("Datapool")) +
                         " - " + QString(pc_OSCDataPool->c_Name.c_str()) + "</div>";
            }
         }

         if (((pc_OSCList != NULL) && (pc_UIList != NULL)) &&
             (pc_OSCList->c_Elements.size() == pc_UIList->c_DataPoolListElements.size()))
         {
            uint32 u32_ElementCounter;
            // Heading for list
            c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(2UL));
            c_Text += QString(C_GtGetText::h_GetText("List")) + " - " + QString(pc_OSCList->c_Name.c_str()) + "</div>";
            c_Text += QString("<table width=\"100%\" %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(3UL));

            for (u32_ElementCounter = 0U; u32_ElementCounter < pc_OSCList->c_Elements.size(); ++u32_ElementCounter)
            {
               const C_OSCNodeDataPoolListElement * pc_OSCElement;
               const C_PuiSdNodeDataPoolListElement & rc_CurUiListElement =
                  pc_UIList->c_DataPoolListElements[u32_ElementCounter];
               std::vector<QString> c_NvmValues;
               QString c_ElementNumber;
               QString c_ElementText;
               QString c_SpanChangedStart = "";
               QString c_SpanChangedEnd = "";

               // Mark the changed elements
               const bool q_Changed = this->m_WasElementChanged(u32_CurNodeIndex, (*c_ItItem).u32_DataPoolIndex,
                                                                (*c_ItItem).u32_ListIndex, u32_ElementCounter,
                                                                oq_ShowReadValues);

               if (q_Changed == true)
               {
                  // Formatting of changed values
                  c_SpanChangedStart = "<span style=\" font-weight: bold;\">";
                  c_SpanChangedEnd = "</span>";
               }

               c_ElementNumber = "#" + QString::number(u32_ElementCounter + 1U);
               c_ElementText = pc_OSCList->c_Elements[u32_ElementCounter].c_Name.c_str();

               if (oq_ShowReadValues == true)
               {
                  pc_OSCElement = m_GetReadElementById(u32_CurNodeIndex,
                                                       (*c_ItItem).u32_DataPoolIndex,
                                                       (*c_ItItem).u32_ListIndex,
                                                       u32_ElementCounter);
                  tgl_assert(pc_OSCElement != NULL);
               }
               else
               {
                  pc_OSCElement = &pc_OSCList->c_Elements[u32_ElementCounter];
               }
               if (pc_OSCElement != NULL)
               {
                  if (rc_CurUiListElement.q_InterpretAsString == true)
                  {
                     const QString c_Tmp = "\"" + C_SdNdeDpUtil::h_ConvertToString(pc_OSCElement->c_NvmValue) +
                                           "\"";
                     c_NvmValues.push_back(c_Tmp);
                  }
                  else
                  {
                     C_SdNdeDpContentUtil::h_GetValuesAsScaledString(
                        pc_OSCElement->c_NvmValue,
                        pc_OSCList->c_Elements[u32_ElementCounter].f64_Factor,
                        pc_OSCList->c_Elements[u32_ElementCounter].f64_Offset,
                        c_NvmValues);
                  }
               }
               // Filling of all table data cells
               if (c_NvmValues.size() >= 1)
               {
                  // HTML row in table start
                  c_Text += "<tr>";
                  // Filling of all table data cells
                  c_Text += "<td width=\"5%\">" + c_SpanChangedStart + c_ElementNumber + c_SpanChangedEnd + "</td>";
                  c_Text += "<td width=\"15%\">" + c_SpanChangedStart + c_ElementText + c_SpanChangedEnd + "</td>";

                  // Array
                  c_Text += "<td>" + c_SpanChangedStart;

                  for (uintn un_ArrayCounter = 0U; un_ArrayCounter < c_NvmValues.size(); ++un_ArrayCounter)
                  {
                     c_Text += c_NvmValues[un_ArrayCounter] + ";";
                  }
                  c_Text.chop(1); // remove last ";"
                  c_Text += " " + QString(pc_OSCList->c_Elements[u32_ElementCounter].c_Unit.c_str()) +
                            c_SpanChangedEnd + "</td>";
                  c_Text += "</tr>";
               }
               else
               {
                  // HTML row in table start
                  c_Text += "<tr>";
                  // Filling of all table data cells
                  c_Text += "<td width=\"5%\">" + c_SpanChangedStart + c_ElementNumber + c_SpanChangedEnd + "</td>"
                            "<td width=\"15%\">" + c_SpanChangedStart + c_ElementText + c_SpanChangedEnd + "</td>"
                            "<td width=\"15%\">Error</td>";
                  c_Text += "</tr>";
               }
            }
            c_Text += "</table>";
            c_Text += "<br/><br/>";
         }
      }
   }

   // Add the last node too
   if (c_Text != "")
   {
      c_NodeText.push_back(c_Text);
   }

   // Sort for node names. The node name is always at the start of each string and must be unique.
   c_NodeText.sort();
   // Font and style configuration. Stylesheets does not work with html. Color is STYLE_GUIDE_COLOR_6
   c_Text = "<span style=\" font-family:Segoe UI; font-size:13px; font-weight: normal; color:#565668;\">";

   for (c_ItNodeText = c_NodeText.begin(); c_ItNodeText != c_NodeText.end(); ++c_ItNodeText)
   {
      c_Text += *c_ItNodeText;
   }

   c_Text += "</span>";

   this->mpc_Ui->pc_TextEditConfirm->setHtml(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Writes the CRC of relevant lists

   Implemented steps: 5
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_WriteCrcOfNode(void)
{
   if (this->mu32_CurrentNode < this->mc_AllNodeIndexes.size())
   {
      sint32 s32_Return;

      if (this->mq_RequestPending == false)
      {
         s32_Return = this->mrc_ComDriver.PollSafeNvmSafeWriteCrcs(this->mc_AllNodeIndexes[this->mu32_CurrentNode]);

         if (s32_Return == C_NO_ERR)
         {
            this->mq_RequestPending = true;
         }
         else if (s32_Return == C_RANGE)
         {
            // Error
            this->mc_Timer.stop();
            this->m_ReportError("NvmSafeWriteCrcs", "Wrong node index.", s32_Return);
         }
         else
         {
            // On C_BUSY is nothing to do
         }
      }
      else
      {
         sint32 s32_ServiceResult;
         s32_Return = this->mrc_ComDriver.GetPollResults(s32_ServiceResult);

         if (s32_Return == C_NO_ERR)
         {
            // Thread finished
            if (s32_ServiceResult == C_NO_ERR)
            {
               this->mq_RequestPending = false;
               ++this->mu32_CurrentNode;

               if (this->mu32_CurrentNode >= this->mc_AllNodeIndexes.size())
               {
                  // No further nodes
                  //                  this->mc_Timer.thread()->sleep(2);
                  this->mu32_CurrentNode = 0U;
                  this->mc_Timer.stop();
                  this->mpc_Ui->pc_WiConfirm->setEnabled(false);
                  this->mpc_Ui->pc_WiConfirm->hide();
                  this->mpc_Ui->pc_WiNotification->show();
                  this->mpc_Ui->pc_WiNotification->setEnabled(true);
                  this->mpc_Ui->pc_ProgressCrc->SetProgress(100);
                  this->mpc_Ui->pc_LabelFinished->setEnabled(true);
                  this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Parametrization Finished"));
                  this->mpc_Ui->pc_LabelStepDescription->setText(
                     C_GtGetText::h_GetText(
                        "Parametrization process has finished.\nATTENTION: The values have been written to "
                        "NVM memory, but RAM copies were not updated. To update the RAM "
                        "copies choose one of these options: \n"
                        "Option 1: Use the following application notification (This option "
                        "needs an implemented hook function \"osy_app_nvm_data_was_changed_event\".\n"
                        "Option 2: Reset your System."));
                  this->mpc_Ui->pc_BushButtonOk->setVisible(true);
                  this->mpc_Ui->pc_BushButtonCancel->setVisible(false);
                  this->mpc_Ui->pc_BopperleFinished->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21,
                                                                          mc_STYLE_GUIDE_COLOR_13);
               }
               else
               {
                  this->mpc_Ui->pc_ProgressCrc->SetProgress((this->mu32_CurrentNode * 100U) /
                                                            this->mc_AllNodeIndexes.size());
               }
            }
            else
            {
               // Error of service
               this->mc_Timer.stop();
               m_ReportErrorNvmSafeWriteCrcs(s32_ServiceResult);
            }
         }
      }
   }
   else
   {
      // Error of service
      this->mc_Timer.stop();
      this->m_ReportError("NvmSafeWriteCrcs", "No changed elements found.", C_RANGE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaWriteWidget::m_SendNotification(void)
{
   const bool q_Retval = this->mpc_Ui->pc_CbNotifiction->isChecked();

   if ((q_Retval == true) && (this->mc_RelevantLists.size() > 0UL))
   {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      this->mc_NotificationResult.clear();
      this->mc_NotificationResult.reserve(this->mc_RelevantLists.size());
      this->mc_NotificationIterator = this->mc_RelevantLists.begin();

      this->me_Step = eSEND_NOTIFICATION;
      this->mrc_ComDriver.PollNvmNotifyOfChanges((*this->mc_NotificationIterator).u32_NodeIndex,
                                                 static_cast<uint8>((*this->mc_NotificationIterator).u32_DataPoolIndex),
                                                 static_cast<uint16>((*this->mc_NotificationIterator).u32_ListIndex));
      //Important iterator step (strange notation because of lint)
      this->mc_NotificationIterator.operator ++();
      this->mc_Timer.start();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle next notification request or result handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_SendNextNotification(void)
{
   sint32 s32_Result;

   if (this->mrc_ComDriver.GetPollResults(s32_Result) == C_NO_ERR)
   {
      if (s32_Result == C_NO_ERR)
      {
         bool q_Ack;
         tgl_assert(this->mrc_ComDriver.GetPollNvmNotifyOfChangesOutput(q_Ack) == C_NO_ERR);
         this->mc_NotificationResult.push_back(q_Ack);
         if (this->mc_NotificationIterator != this->mc_RelevantLists.end())
         {
            if (this->mrc_ComDriver.PollNvmNotifyOfChanges((*this->mc_NotificationIterator).u32_NodeIndex,
                                                           static_cast<uint8>((*this->mc_NotificationIterator).
                                                                              u32_DataPoolIndex),
                                                           static_cast<uint16>((*this->mc_NotificationIterator).
                                                                               u32_ListIndex)) ==
                C_NO_ERR)
            {
               //Important iterator step (strange notation because of lint)
               this->mc_NotificationIterator.operator ++();
            }
         }
         else
         {
            std::set<stw_opensyde_core::C_OSCNodeDataPoolListId>::const_iterator c_NotificationIterator =
               this->mc_RelevantLists.begin();
            QString c_Text;
            bool q_AnyNonAcks = false;

            //Last one
            this->mc_Timer.stop();

            tgl_assert(this->mc_NotificationResult.size() == this->mc_RelevantLists.size());

            for (uint32 u32_ItAck = 0; u32_ItAck < this->mc_NotificationResult.size(); ++u32_ItAck)
            {
               QString c_Result;
               if (this->mc_NotificationResult[u32_ItAck] == false)
               {
                  c_Result = C_GtGetText::h_GetText("FAILURE");
                  q_AnyNonAcks = true;
               }
               else
               {
                  c_Result = C_GtGetText::h_GetText("OK");
               }
               c_Text +=
                  QString(C_GtGetText::h_GetText("%1 ended with %2\n")).arg(mh_GetId(*c_NotificationIterator)).arg(
                     c_Result);
               //Important iterator step
               ++c_NotificationIterator;
            }
            QApplication::restoreOverrideCursor();
            if (q_AnyNonAcks == true)
            {
               C_OgeWiCustomMessage c_Message(this);
               c_Message.SetDescription(QString(C_GtGetText::h_GetText("At least one application did not acknowledge "
                                                                       "the application notification")));
               c_Message.SetDetails(c_Text);
               c_Message.SetCustomMinHeight(180, 250);
               c_Message.Execute();
            }

            this->mpc_ParentDialog->accept();

            // For the result function
            this->me_Step = eFINISHED;
         }
      }
      else
      {
         this->mc_Timer.stop();
         QApplication::restoreOverrideCursor();
         m_ReportErrorNvmNotifyOfChanges(s32_Result);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_OkClicked(void)
{
   if (this->m_SendNotification() == false)
   {
      this->mpc_ParentDialog->accept();

      // For the result function
      this->me_Step = eFINISHED;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_OnCancel(void)
{
   if ((me_Step == eCONFIRM) || (me_Step == eREADBACK) || (me_Step == eWRITE))
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Parametrization interrupt "));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to interrupt the process?\nWithout writing "
                                                      "CRC the lists will become invalid!"));
      c_Message.SetNOButtonText("Interrupt");
      c_Message.SetOKButtonText("Don't Interrupt");
      c_Message.SetCustomMinHeight(180, 180);
      C_OgeWiCustomMessage::E_Outputs e_Output;
      e_Output = c_Message.Execute();
      if (e_Output == C_OgeWiCustomMessage::eNO)
      {
         this->mpc_ParentDialog->reject();
      }
   }
   else
   {
      this->mpc_ParentDialog->reject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change of confirm status of correctness of the NVM lists

   Implemented steps: 4
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ConfirmCheckBoxChanged(void) const
{
   this->mpc_Ui->pc_PbConfirm->setEnabled(this->mpc_Ui->pc_CbConfirm->isChecked());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   User confirms the correctness of the NVM lists

   Implemented steps: 4
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ConfirmClicked(void)
{
   if (this->me_Step == eBEFOREWRITE)
   {
      this->mpc_Ui->pc_ProgressWrite->SetProgress(50);
      this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Write Parameter Values"));
      this->mpc_Ui->pc_LabelStepDescription->setText(
         C_GtGetText::h_GetText("Writing parameter values to NVM memory (without lists CRC). Lists CRC "
                                "will be handled later in this process."));
      this->mpc_Ui->pc_PbConfirm->setVisible(false);
      this->mpc_Ui->pc_CbConfirm->setVisible(false);
      this->me_Step = eWRITE;
      this->mc_Timer.start();
   }
   else if (this->me_Step == eCONFIRM)
   {
      if (this->mpc_Ui->pc_CbConfirm->isChecked() == true)
      {
         this->mpc_Ui->pc_ProgressConfirm->SetProgress(100);
         this->mpc_Ui->pc_WiConfirm->setEnabled(false);
         this->mpc_Ui->pc_LabelCrc->setEnabled(true);
         this->mpc_Ui->pc_PbConfirm->setVisible(false);
         this->mpc_Ui->pc_CbConfirm->setVisible(false);
         this->mpc_Ui->pc_BopperleCrc->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
         this->mpc_Ui->pc_LabelHeadingSteps->setText(C_GtGetText::h_GetText("Write Lists CRC"));
         this->mpc_Ui->pc_LabelStepDescription->setText(
            C_GtGetText::h_GetText("Updating lists CRC..."));
         this->me_Step = eWRITECRC;

         this->mc_Timer.start();
      }
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the element was changed

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oq_CheckReadValues   Flag if read values should be checked

   \return
   true     Element was changed
   false    Element was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaWriteWidget::m_WasElementChanged(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                 const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex,
                                                 const bool oq_CheckReadValues)
{
   bool q_Changed = false;

   if (oq_CheckReadValues == true)
   {
      bool q_NodeFound = false;

      uint32 u32_VectorNodeIndex;

      // Search the matching node vector
      for (u32_VectorNodeIndex = 0U; u32_VectorNodeIndex < this->mc_AllNodeIndexes.size(); ++u32_VectorNodeIndex)
      {
         if (this->mc_AllNodeIndexes[u32_VectorNodeIndex] == ou32_NodeIndex)
         {
            // Node found
            q_NodeFound = true;
            break;
         }
      }

      if (q_NodeFound == true)
      {
         uint32 u32_ElementCounter;

         for (u32_ElementCounter = 0U;
              u32_ElementCounter < this->mc_ChangedElementsForEachNode[u32_VectorNodeIndex].size();
              ++u32_ElementCounter)
         {
            C_OSCNodeDataPoolListElementId & rc_ElementId =
               this->mc_ChangedElementsForEachNode[u32_VectorNodeIndex][u32_ElementCounter];
            if ((rc_ElementId.u32_DataPoolIndex == ou32_DataPoolIndex) &&
                (rc_ElementId.u32_ListIndex == ou32_ListIndex) &&
                (rc_ElementId.u32_ElementIndex == ou32_ElementIndex))
            {
               q_Changed = true;
               break;
            }
         }
      }
   }
   else
   {
      const C_OSCNodeDataPoolListElementId c_ElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                       ou32_ElementIndex);
      const std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                     stw_opensyde_gui_logic::C_SyvDaItPaValuePairs>::const_iterator c_It =
         this->mc_AllAffectedValues.find(c_ElementId);
      tgl_assert(c_It != this->mc_AllAffectedValues.end());
      if (c_It != this->mc_AllAffectedValues.end())
      {
         q_Changed = c_It->second.q_Changed;
      }
   }

   return q_Changed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get suspect element report

   \return
   String with details for suspect elements (empty if nothing to report)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWriteWidget::m_GetSuspectElementReport(void) const
{
   QString c_Retval;

   for (std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                 stw_opensyde_gui_logic::C_SyvDaItPaValuePairs>::const_iterator c_It =
           this->mc_AllAffectedValues.begin();
        c_It != this->mc_AllAffectedValues.end(); ++c_It)
   {
      if (c_It->second.c_Actual == c_It->second.c_Expected)
      {
         //Skip: valid
      }
      else
      {
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_It->first);
         tgl_assert(pc_Element != NULL);
         if (pc_Element != NULL)
         {
            const QString c_Namespace =
               C_PuiSdHandler::h_GetInstance()->GetNamespace(c_It->first);
            QString c_Expected;
            QString c_Actual;
            QString c_Entry;
            //Mark as suspect
            C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(c_It->second.c_Expected,
                                                                    pc_Element->f64_Factor,
                                                                    pc_Element->f64_Offset, c_Expected);
            c_Expected += QString(" ") + pc_Element->c_Unit.c_str();
            C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(c_It->second.c_Actual,
                                                                    pc_Element->f64_Factor,
                                                                    pc_Element->f64_Offset, c_Actual);
            c_Actual += QString(" ") + pc_Element->c_Unit.c_str();
            c_Entry = QString(C_GtGetText::h_GetText("%1 written: %2, read: %3")).arg(c_Namespace).arg(c_Expected).arg(
               c_Actual);
            c_Retval += c_Entry + "\n";
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_Timer(void)
{
   if (this->me_Step == eBEFOREWRITE)
   {
      this->m_PrepareForWrite();
   }
   if (this->me_Step == eWRITE)
   {
      this->m_WriteChangedElementsOfNode();
   }
   else if (this->me_Step == eREADBACK)
   {
      this->m_ReadBackElementsOfNode();
   }
   else if (this->me_Step == eWRITECRC)
   {
      this->m_WriteCrcOfNode();
   }
   else if (this->me_Step == eSEND_NOTIFICATION)
   {
      this->m_SendNextNotification();
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText,
                                           const stw_types::sint32 os32_ErrorCode)
{
   QString c_Text = "Function " + orc_FunctionName +
                    " ended with error code \"" + C_Uti::h_StwError(os32_ErrorCode) + "\"";

   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);

   osc_write_log_info("Write NVM Parameters",
                      c_Text.toStdString().c_str());

   c_Message.SetDescription(QString(C_GtGetText::h_GetText("Function %1 ended with error.")).arg(orc_FunctionName));
   c_Message.SetDetails(QString(C_GtGetText::h_GetText("Error code:\n %1\nError text: \n %2"))
                        .arg(os32_ErrorCode).arg(orc_ErrorText));
   c_Message.SetCustomMinHeight(180, 270);
   c_Message.Execute();

   // Close dialog on error
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error for function "NvmSafeWriteChangedValues"

   \param[in]  os32_ErrorCode    Function result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReportErrorNvmSafeWriteChangedValues(const sint32 os32_ErrorCode)
{
   const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
   uint8 u8_NRC;
   QString c_Details;
   QString c_Description;
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
   sint32 s32_Return = this->mrc_ComDriver.GetPollResultNRC(u8_NRC);

   tgl_assert(s32_Return == C_NO_ERR);
   switch (os32_ErrorCode)
   {
   case C_NO_ERR:
      c_Description += "Writing successful";
      break;
   case C_CONFIG:
      c_Description += "No node or diagnostic protocol are known (was this class properly initialized?)\n";
      c_Description += "Protocol driver reported configuration error (was the protocol driver properly initialized?)";
      break;
   case C_OVERFLOW:
      c_Description += "At least one value is out of the defined minimum and maximum range.";
      break;
   case C_BUSY:
      c_Description += "Client: No changed value found and no additional lists specified";
      break;
   case C_TIMEOUT:
      c_Description += "Expected server response not received within timeout";
      break;
   case C_NOACT:
      c_Description += "Could not send request (e.g. Tx buffer full)";
      break;
   // Using <br/> as new line symbol instead of \n due to problems with the h_GetLink syntax
   case C_WARN:
      switch (u8_NRC)
      {
      case 0x13:
         c_Details = C_GtGetText::h_GetText("Incorrect length of request<br/>");
         break;
      case 0x31:
         c_Details = C_GtGetText::h_GetText("Address or length format invalid (> 4 bytes)<br/>"
                                            "Requested memory range specified by address and size invalid<br/>");
         break;
      case 0x72:
         c_Details = C_GtGetText::h_GetText("Server writing NVM failed<br/>");
         break;
      case 0x33:
         c_Details = C_GtGetText::h_GetText("Required security level was not unlocked<br/>");
         break;
      case 0x14:
         c_Details =
            C_GtGetText::h_GetText("The total length of the response message exceeds the available buffer size<br/>");
         break;
      case 0x7F:
         c_Details =
            C_GtGetText::h_GetText("Server is not in the correct diagnostic session<br/>");
         break;
      default:
         c_Details = QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1<br/>")).arg(QString::number(u8_NRC, 16));
         break;
      }
      c_Description += "Server sent error response";
      break;
   case C_RD_WR:
      c_Description += "Unexpected content in server response";
      break;
   case C_COM:
      c_Description += "Communication driver reported error";
      break;
   default:
      //Should not happen
      c_Description += "Unknown";
      break;
   }

   C_OSCLoggingHandler::h_Flush();
   c_Details += QString(C_GtGetText::h_GetText("See log file for details: ")) +
                C_Uti::h_GetLink(c_Log, mc_STYLESHEET_GUIDE_COLOR_LINK, c_Log);

   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(230, 300);
   c_Message.Execute();

   // Close dialog on error
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error for function "NvmSafeReadValues"

   \param[in]  os32_ErrorCode    Function result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReportErrorNvmSafeReadValues(const sint32 os32_ErrorCode)
{
   const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
   uint8 u8_NRC;
   QString c_Details;
   QString c_Description;
   C_OgeWiCustomMessage c_Message(this);
   sint32 s32_Return = this->mrc_ComDriver.GetPollResultNRC(u8_NRC);

   tgl_assert(s32_Return == C_NO_ERR);

   switch (os32_ErrorCode)
   {
   case C_NO_ERR:
      c_Description += "Values read successful";
      break;
   case C_CONFIG:
      c_Description += "No node or diagnostic protocol are known (was this class properly initialized?)\n";
      c_Description += "Protocol driver reported configuration error (was the protocol driver properly initialized?)";
      break;
   case C_OVERFLOW:
      c_Description += "At least one list has no elements.";
      break;
   case C_RD_WR:
      c_Description += "No list contains Datapool elements that were";
      c_Description += "written by the preceding call to \"NvmSafeWriteChangedValues\"";
      break;
   case C_RANGE:
      c_Description += "At least one index of a Datapool or a list of changed lists is invalid or";
      c_Description += "Datapool element size configuration does not match with count of read bytes";
      break;
   case C_TIMEOUT:
      c_Description += "Expected server response not received within timeout";
      break;
   case C_NOACT:
      c_Description += "Could not send request (e.g. Tx buffer full)";
      break;
   // Using <br/> as new line symbol instead of \n due to problems with the h_GetLink syntax
   case C_WARN:
      c_Description += "Server sent error response or malformed protocol response";
      switch (u8_NRC)
      {
      case 0x13:
         c_Details = C_GtGetText::h_GetText("Incorrect length of request<br/>");
         break;
      case 0x31:
         c_Details = C_GtGetText::h_GetText("Address or length format invalid (> 4 bytes)<br/>"
                                            "Requested memory range specified by address and size invalid<br/>");
         break;
      case 0x72:
         c_Details = C_GtGetText::h_GetText("Server reading NVM failed<br/>");
         break;
      case 0x33:
         c_Details = C_GtGetText::h_GetText("Required security level was not unlocked<br/>");
         break;
      case 0x14:
         c_Details =
            C_GtGetText::h_GetText("The total length of the response message exceeds the available buffer size<br/>");
         break;
      case 0x7F:
         c_Details =
            C_GtGetText::h_GetText("Server is not in the correct diagnostic session<br/>");
         break;
      default:
         c_Details = QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1<br/>")).arg(QString::number(u8_NRC, 16));
         break;
      }
      break;
   case C_COM:
      c_Description += "Communication driver reported error";
      break;
   default:
      //Should not happen
      c_Description += "Unknown";
      break;
   }

   C_OSCLoggingHandler::h_Flush();
   c_Details += QString(C_GtGetText::h_GetText("See log file for details: ")) +
                C_Uti::h_GetLink(c_Log, mc_STYLESHEET_GUIDE_COLOR_LINK, c_Log);

   c_Message.SetType(C_OgeWiCustomMessage::eERROR);
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(230, 300);
   c_Message.Execute();

   // Close dialog on error
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error for function "NvmSafeWriteCrcs"

   \param[in]  os32_ErrorCode    Function result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReportErrorNvmSafeWriteCrcs(const sint32 os32_ErrorCode)
{
   const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
   uint8 u8_NRC;
   QString c_Details;
   QString c_Description;
   C_OgeWiCustomMessage c_Message(this);
   sint32 s32_Return = this->mrc_ComDriver.GetPollResultNRC(u8_NRC);

   tgl_assert(s32_Return == C_NO_ERR);

   switch (os32_ErrorCode)
   {
   case C_NO_ERR:
      c_Description += "CRCs written successfully";
      break;
   case C_CONFIG:
      c_Description += "No node or diagnostic protocol are known (was this class properly initialized?)\n";
      c_Description += "protocol driver reported configuration error (was the protocol driver properly initialized?)";
      break;
   case C_CHECKSUM:
      c_Description += "At least one Datapool of type \"NVM\" has the flag q_IsSafety set to true and";
      c_Description += "at least one of its lists has the flag q_NvMCRCActive set to false.";
      break;
   case C_RANGE:
      c_Description +=
         "The size of at least one list of NVM Datapools of the copy of C_OSCNode differs to the original instance.\n";
      c_Description +=
         "The count of lists of NVM Datapools of the copy of C_OSCNode differs from the original instance.\n";
      c_Description += "The count of the Datapools of the copy of C_OSCNode differs from the original instance.";
      break;
   case C_BUSY:
      c_Description += "No list contains Datapool elements that were";
      c_Description += "written by the preceding call to \"NvmSafeWriteChangedValues\"";
      break;
   case C_OVERFLOW:
      c_Description += "At least one changed list has no elements.";
      break;
   case C_DEFAULT:
      c_Description += "At least one element of the changed lists has the flag q_IsValid set to false.";
      break;
   case C_TIMEOUT:
      c_Description += "Expected server response not received within timeout";
      break;
   case C_NOACT:
      c_Description += "Could not send request (e.g. Tx buffer full)";
      break;
   // Using <br/> as new line symbol instead of \n due to problems with the h_GetLink syntax
   case C_WARN:
      c_Description += "Server sent error response";
      switch (u8_NRC)
      {
      case 0x13:
         c_Details = C_GtGetText::h_GetText("Incorrect length of request<br/>");
         break;
      case 0x31:
         c_Details = C_GtGetText::h_GetText("Address or length format invalid (> 4 bytes)<br/>"
                                            "Requested memory range specified by address and size invalid<br/>");
         break;
      case 0x72:
         c_Details = C_GtGetText::h_GetText("Server writing NVM failed<br/>");
         break;
      case 0x33:
         c_Details = C_GtGetText::h_GetText("Required security level was not unlocked<br/>");
         break;
      case 0x14:
         c_Details =
            C_GtGetText::h_GetText("The total length of the response message exceeds the available buffer size<br/>");
         break;
      case 0x7F:
         c_Details =
            C_GtGetText::h_GetText("Server is not in the correct diagnostic session<br/>");
         break;
      default:
         c_Details = QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1<br/>")).arg(QString::number(u8_NRC, 16));
         break;
      }
      break;
   case C_RD_WR:
      c_Description += "Unexpected content in server response";
      break;
   case C_COM:
      c_Description += "Communication driver reported error";
      break;
   default:
      //Should not happen
      c_Description += "Unknown";
      break;
   }

   C_OSCLoggingHandler::h_Flush();
   c_Details += QString(C_GtGetText::h_GetText("See log file for details: ")) +
                C_Uti::h_GetLink(c_Log, mc_STYLESHEET_GUIDE_COLOR_LINK, c_Log);

   c_Message.SetType(C_OgeWiCustomMessage::eERROR);
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(250, 300);
   c_Message.Execute();

   // Close dialog on error
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error for function "NvmNotifyOfChanges"

   \param[in]  os32_ErrorCode    Function result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaWriteWidget::m_ReportErrorNvmNotifyOfChanges(const sint32 os32_ErrorCode)
{
   const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
   uint8 u8_NRC;
   QString c_Details;
   QString c_Description;
   C_OgeWiCustomMessage c_Message(this);
   sint32 s32_Return = this->mrc_ComDriver.GetPollResultNRC(u8_NRC);

   tgl_assert(s32_Return == C_NO_ERR);

   switch (os32_ErrorCode)
   {
   case C_NO_ERR:
      c_Description += "Request sent, positive response received";
      break;
   case C_CONFIG:
      c_Description += "No node or diagnostic protocol are known (was this class properly initialized?)\n";
      c_Description += "Protocol driver reported configuration error (was the protocol driver properly initialized?)";
      break;
   case C_TIMEOUT:
      c_Description += "Expected server response not received within timeout";
      break;
   case C_NOACT:
      c_Description += "Could not send request (e.g. Tx buffer full)";
      break;
   case C_WARN:
      c_Description += "Server sent error response";
      switch (u8_NRC)
      {
      case 0x13:
         c_Details = C_GtGetText::h_GetText("Incorrect length of request");
         break;
      case 0x31:
         c_Details = C_GtGetText::h_GetText("Invalid Datapool index or list index");
         break;
      case 0x33:
         c_Details = C_GtGetText::h_GetText("Required security level was not unlocked");
         break;
      case 0x12:
         c_Details = C_GtGetText::h_GetText(
            "RoutineControl sub-function (= routineControlType) not supported for this routine identifier "
            "(see description of individual routines for supported sub-functions)");
         break;
      case 0x14:
         c_Details =
            C_GtGetText::h_GetText("The total length of the response message exceeds the available buffer size");
         break;
      case 0x7F:
         c_Details =
            C_GtGetText::h_GetText("Server is not in the correct diagnostic session<br/>");
         break;
      default:
         c_Details = QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(u8_NRC, 16));
         break;
      }
      break;
   case C_RD_WR:
      c_Description += "Unexpected content in server response (here: wrong Datapool index)";
      break;
   case C_COM:
      c_Description += "Communication driver reported error";
      break;
   default:
      //Should not happen
      c_Description += "Unknown";
      break;
   }

   C_OSCLoggingHandler::h_Flush();
   c_Details += QString(C_GtGetText::h_GetText("See log file for details: ")) +
                C_Uti::h_GetLink(c_Log, mc_STYLESHEET_GUIDE_COLOR_LINK, c_Log);

   c_Message.SetType(C_OgeWiCustomMessage::eERROR);
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(250, 300);
   c_Message.Execute();

   // Close dialog on error
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read and store updated values to use later in param widget

   \return
   String with details for suspect elements (empty if nothing to report)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWriteWidget::m_ReadAndStoreUpdatedValues(void)
{
   for (std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                 stw_opensyde_gui_logic::C_SyvDaItPaValuePairs>::iterator c_It =
           this->mc_AllAffectedValues.begin();
        c_It != this->mc_AllAffectedValues.end(); ++c_It)
   {
      const C_OSCNodeDataPoolListElement * const pc_Element = m_GetReadElementById(c_It->first.u32_NodeIndex,
                                                                                   c_It->first.u32_DataPoolIndex,
                                                                                   c_It->first.u32_ListIndex,
                                                                                   c_It->first.u32_ElementIndex);
      tgl_assert(pc_Element != NULL);
      if (pc_Element != NULL)
      {
         c_It->second.c_Actual = pc_Element->c_NvmValue;
      }
   }
   return m_GetSuspectElementReport();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get list name (and containers) as string

   \param[in]  orc_Id   List ID

   \return
   List name (and containers) as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaWriteWidget::mh_GetId(const C_OSCNodeDataPoolListId & orc_Id)
{
   QString c_Default = C_GtGetText::h_GetText("Unknown");
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Id.u32_NodeIndex);
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_Id.u32_NodeIndex,
                                                                                                 orc_Id.u32_DataPoolIndex);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex);

   if (((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL))
   {
      c_Default = QString("%1::%2::%3").arg(pc_Node->c_Properties.c_Name.c_str()).arg(pc_DataPool->c_Name.c_str()).arg(
         pc_List->c_Name.c_str());
   }
   return c_Default;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Element for specified ID from read values

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index

   \return
   NULL Element not found
   Else Valid element
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_SyvDaItPaWriteWidget::m_GetReadElementById(const uint32 ou32_NodeIndex,
                                                                                  const uint32 ou32_DataPoolIndex,
                                                                                  const uint32 ou32_ListIndex,
                                                                                  const uint32 ou32_ElementIndex) const
{
   const C_OSCNodeDataPoolListElement * pc_Retval = NULL;

   if (this->mc_AllNodeIndexes.size() == this->mc_ReadNodeValues.size())
   {
      for (uint32 u32_ItKnownIndices = 0UL; u32_ItKnownIndices < this->mc_AllNodeIndexes.size(); ++u32_ItKnownIndices)
      {
         if (this->mc_AllNodeIndexes[u32_ItKnownIndices] == ou32_NodeIndex)
         {
            const stw_opensyde_core::C_OSCNode * const pc_Node = this->mc_ReadNodeValues[u32_ItKnownIndices];
            if (pc_Node != NULL)
            {
               if (ou32_DataPoolIndex < pc_Node->c_DataPools.size())
               {
                  const C_OSCNodeDataPool & rc_Datapool = pc_Node->c_DataPools[ou32_DataPoolIndex];
                  if (ou32_ListIndex < rc_Datapool.c_Lists.size())
                  {
                     const C_OSCNodeDataPoolList & rc_List = rc_Datapool.c_Lists[ou32_ListIndex];
                     if (ou32_ElementIndex < rc_List.c_Elements.size())
                     {
                        pc_Retval = &rc_List.c_Elements[ou32_ElementIndex];
                     }
                  }
               }
            }
         }
      }
   }
   return pc_Retval;
}

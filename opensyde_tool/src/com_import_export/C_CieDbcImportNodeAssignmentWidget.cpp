//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget where user can select which DBC node corresponds to which openSYDE node

   Popup widget where user can select which DBC node corresponds to which openSYDE node

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_CieDbcImportNodeAssignmentWidget.h"
#include "ui_C_CieDbcImportNodeAssignmentWidget.h"

#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "C_Uti.h"
#include "C_CieConverter.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::uint8 C_CieDbcImportNodeAssignmentWidget::mhu8_INDEX_TRANSMIT = 0;
const stw_types::uint8 C_CieDbcImportNodeAssignmentWidget::mhu8_INDEX_RECEIVE = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent        Reference to parent
   \param[in]     ou32_BusIndex     Index of bus where messages get imported
   \param[in]     orc_CIECommDef    Data structure read from DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDbcImportNodeAssignmentWidget::C_CieDbcImportNodeAssignmentWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,  const QString & orc_FilePath,  const uint32 ou32_BusIndex,
   const C_CieConverter::C_CIECommDefinition & orc_CIECommDef) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieDbcImportNodeAssignmentWidget),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // Initialize content
   this->InitStaticNames();
   this->mpc_Ui->pc_LabPath->setText(orc_FilePath);
   this->mpc_Ui->pc_LabPath->setText(
      C_Uti::h_MinimizePath(orc_FilePath, C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13), 520, 0));
   this->mpc_Ui->pc_LabPath->SetToolTipInformation("", orc_FilePath);

   this->m_InitNodes(ou32_BusIndex, orc_CIECommDef);

   this->mpc_Ui->pc_LabDbcNodeTitle->SetFontPixel(14, true);
   this->mpc_Ui->pc_LabInfoTitle->SetFontPixel(14, true);
   this->mpc_Ui->pc_LabOsyNodeTitle->SetFontPixel(14, true);

   this->mpc_Ui->pc_LabDbcNodeTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabInfoTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabOsyNodeTitle->SetForegroundColor(4);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked,
           this, &C_CieDbcImportNodeAssignmentWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_CieDbcImportNodeAssignmentWidget::m_CancelClicked);

   //lint -e{929} Cast required to avoid ambiguous signal of Qt interface
   connect(this->mpc_Ui->pc_CbxAssignee, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CieDbcImportNodeAssignmentWidget::m_OnUnmappedCbxIndexChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDbcImportNodeAssignmentWidget::~C_CieDbcImportNodeAssignmentWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Import Messages"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("DBC File"));

   // file info group box
   this->mpc_Ui->pc_LabHeadingFileInfo->setText(C_GtGetText::h_GetText("Source File Information"));
   this->mpc_Ui->pc_LabPathTitle->setText(C_GtGetText::h_GetText("Path:"));

   // mapped messages
   this->mpc_Ui->pc_LabHeadingMapped->setText(C_GtGetText::h_GetText("Mapped Messages"));
   this->mpc_Ui->pc_LabDbcNodeTitle->setText(C_GtGetText::h_GetText("DBC Node"));
   this->mpc_Ui->pc_LabDbcNodeTitle->SetToolTipInformation(C_GtGetText::h_GetText("DBC Node"),
                                                           C_GtGetText::h_GetText("Node from DBC file description."));
   this->mpc_Ui->pc_LabInfoTitle->setText(C_GtGetText::h_GetText("Information"));
   this->mpc_Ui->pc_LabInfoTitle->SetToolTipInformation(
      C_GtGetText::h_GetText("Information"),
      C_GtGetText::h_GetText("Number of Tx and Rx messages of this node in DBC file."));
   this->mpc_Ui->pc_LabOsyNodeTitle->setText(C_GtGetText::h_GetText("Assignee"));
   this->mpc_Ui->pc_LabOsyNodeTitle->SetToolTipInformation(
      C_GtGetText::h_GetText("Assignee"),
      C_GtGetText::h_GetText("Select a node from Network Topology. Corresponding DBC messages get imported "
                             "to this node."));
   this->mpc_Ui->pc_LabNoMapped->setText(C_GtGetText::h_GetText("No mapped messages found."));

   // unmapped messages
   this->mpc_Ui->pc_LabHeadingUnmapped->setText(C_GtGetText::h_GetText("Unmapped Messages"));
   this->mpc_Ui->pc_LabAssignee->setText(C_GtGetText::h_GetText("Assignee"));
   this->mpc_Ui->pc_LabAssignee->SetToolTipInformation(
      C_GtGetText::h_GetText("Assignee"),
      C_GtGetText::h_GetText("Select a node from Network Topology. All unmapped DBC messages get imported "
                             "to this node."));
   this->mpc_Ui->pc_LabDirection->setText(C_GtGetText::h_GetText("Direction"));
   this->mpc_Ui->pc_LabDirection->SetToolTipInformation(
      C_GtGetText::h_GetText("Direction"),
      C_GtGetText::h_GetText("Choose if the selected node is transmitter or receiver of the unmapped messages."));
   this->mpc_Ui->pc_LabNoUnmapped->setText(C_GtGetText::h_GetText("No unmapped messages found."));
   this->mpc_Ui->pc_LabNumberUnmapped->setText("");

   // combobox direction
   this->mpc_Ui->pc_CbxDirection->addItem("dummy");
   this->mpc_Ui->pc_CbxDirection->addItem("dummy");
   this->mpc_Ui->pc_CbxDirection->setItemText(mhu8_INDEX_TRANSMIT, C_GtGetText::h_GetText("Transmit"));
   this->mpc_Ui->pc_CbxDirection->setItemText(mhu8_INDEX_RECEIVE, C_GtGetText::h_GetText("Receive"));

   // buttons
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all tuples of DBC node, system definition node index and interface index.

   Only returns those tuple the user selected (checkbox checked).

  \return
   class package of DBC nodes an its assigned node indexes and interface indexes
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_CieDbcOsyNodeAssignment> C_CieDbcImportNodeAssignmentWidget::GetNodeAssignments(void) const
{
   std::vector<C_CieDbcOsyNodeAssignment> c_Return;
   std::vector<C_CieDbcImportNodeAssignmentItemWidget *>::const_iterator c_It;

   const sint32 s32_UnmappedIndex = this->mpc_Ui->pc_CbxAssignee->currentIndex();

   // collect data from entry widgets
   for (c_It = this->mc_Entries.begin(); c_It != this->mc_Entries.end(); ++c_It)
   {
      const C_CieDbcImportNodeAssignmentItemWidget & rc_CurEntry = **c_It;
      const C_CieDbcOsyNodeAssignment & rc_CurAssignment = rc_CurEntry.GetNodeAssignment();

      // skip negative index as it means not assigned
      if (rc_CurAssignment.s32_AssignedOsyNodeIndex >= 0)
      {
         c_Return.push_back(rc_CurAssignment);
      }
   }

   // add unmapped messages
   if (s32_UnmappedIndex > 0) // index 0 is "ignore"
   {
      C_CieDbcOsyNodeAssignment c_UnmappedMessages;

      // subtract one of current index because of ignore item in combobox
      const uint32 u32_DataIndex = static_cast<uint32>(s32_UnmappedIndex - 1);

      // messages
      c_UnmappedMessages.c_CieNode.c_Properties.c_Name = C_GtGetText::h_GetText("Unmapped");
      if (this->mpc_Ui->pc_CbxDirection->currentIndex() == mhu8_INDEX_RECEIVE)
      {
         c_UnmappedMessages.c_CieNode.c_RxMessages = this->mc_UnmappedMessages;
      }
      else
      {
         c_UnmappedMessages.c_CieNode.c_TxMessages = this->mc_UnmappedMessages;
      }

      // assigned indexes
      tgl_assert(this->mc_InterfaceIndexes.size() == this->mc_NodeIndexes.size());
      if (u32_DataIndex < this->mc_NodeIndexes.size())
      {
         c_UnmappedMessages.s32_AssignedOsyNodeIndex = this->mc_NodeIndexes[u32_DataIndex];
         c_UnmappedMessages.s32_AssignedOsyInterfaceIndex = this->mc_InterfaceIndexes[u32_DataIndex];
      }

      c_Return.push_back(c_UnmappedMessages);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::m_OkClicked(void)
{
   std::vector<C_CieDbcImportNodeAssignmentItemWidget *>::const_iterator c_It;
   bool q_AtLeastOneSelected = false;

   // check all widgets to find out if user selected anything
   for (c_It = this->mc_Entries.begin(); c_It != this->mc_Entries.end(); ++c_It)
   {
      const C_CieDbcImportNodeAssignmentItemWidget & rc_CurEntry = **c_It;
      if (rc_CurEntry.IsAssigned() == true)
      {
         q_AtLeastOneSelected = true;
         break;
      }
   }

   // check unmapped messages for user selection
   if (this->mpc_Ui->pc_CbxAssignee->currentIndex() > 0)
   {
      q_AtLeastOneSelected = true;
   }

   if (q_AtLeastOneSelected == true)
   {
      this->mrc_ParentDialog.accept();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this);
      c_Message.SetHeading(C_GtGetText::h_GetText("Import Messages"));
      c_Message.SetDescription(C_GtGetText::h_GetText("No assigned node found. Select at least one assigned node to "
                                                      "continue the message import."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   User clicked on cancel button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Wrapper for inserting all item widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::m_InitNodes(const uint32 ou32_BusIndex,
                                                     const C_CieConverter::C_CIECommDefinition & orc_CIECommDef)
{
   const bool q_UnmappedMessagesFound = (orc_CIECommDef.c_UnmappedMessages.size() > 0);

   std::vector<QString> c_NodeNames;

   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex,
                                                                                      this->mc_NodeIndexes,
                                                                                      this->mc_InterfaceIndexes);
   tgl_assert(C_SdUtil::h_GetNames(this->mc_NodeIndexes, this->mc_InterfaceIndexes, c_NodeNames, false) ==
              stw_errors::C_NO_ERR);

   // mapped messages section: insert item widgets
   for (std::vector<C_CieConverter::C_CIENode>::const_iterator c_It = orc_CIECommDef.c_Nodes.begin();
        c_It != orc_CIECommDef.c_Nodes.end(); ++c_It)
   {
      C_CieDbcImportNodeAssignmentItemWidget * const pc_NewItem =
         new C_CieDbcImportNodeAssignmentItemWidget(*c_It, c_NodeNames, this->mc_NodeIndexes,
                                                    this->mc_InterfaceIndexes, this);

      // insert new item before spacer -> "-1"
      this->mpc_Ui->pc_LayoutContents->insertWidget(this->mpc_Ui->pc_LayoutContents->count() - 1, pc_NewItem);

      // connect combobox item change (for disabling)
      connect(pc_NewItem, &C_CieDbcImportNodeAssignmentItemWidget::SigUpdateOtherComboboxes,
              this, &C_CieDbcImportNodeAssignmentWidget::m_UpdateComboboxEntries);

      // remember
      this->mc_Entries.push_back(pc_NewItem);
   } //lint !e429  //no memory leak because of the parent of pc_NewItem and the Qt memory management

   // handle no mapped messages case
   this->mpc_Ui->pc_ScrollAreaMapped->setVisible(orc_CIECommDef.c_Nodes.size() > 0);
   this->mpc_Ui->pc_WiMappedTitles->setVisible(orc_CIECommDef.c_Nodes.size() > 0);
   this->mpc_Ui->pc_WiNoMapped->setVisible(orc_CIECommDef.c_Nodes.size() == 0);

   // unmapped messages section:
   this->mpc_Ui->pc_WiNoUnmapped->setVisible(!q_UnmappedMessagesFound);
   this->mpc_Ui->pc_WiUnmapped->setVisible(q_UnmappedMessagesFound);
   this->mpc_Ui->pc_LabNumberUnmapped->setVisible(q_UnmappedMessagesFound);
   if (q_UnmappedMessagesFound == true)
   {
      this->mpc_Ui->pc_LabNumberUnmapped->setText(static_cast<QString>("(%1)").arg(orc_CIECommDef.c_UnmappedMessages.
                                                                                   size()));

      // fill combobox with all connected topology nodes
      this->mpc_Ui->pc_CbxAssignee->addItem(C_GtGetText::h_GetText("<ignore>"));
      for (std::vector<QString>::const_iterator c_It = c_NodeNames.begin(); c_It != c_NodeNames.end(); ++c_It)
      {
         this->mpc_Ui->pc_CbxAssignee->addItem(*c_It);
      }

      // remember messages
      this->mc_UnmappedMessages = orc_CIECommDef.c_UnmappedMessages;

      // disable direction combobox on start
      this->mpc_Ui->pc_CbxDirection->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of assignee combobox index change.

   Disable Direction combobox if "ignore" is selected.

   \param[in]       os32_Index   new index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::m_OnUnmappedCbxIndexChanged(const sint32 os32_Index)
{
   this->mpc_Ui->pc_CbxDirection->setEnabled(os32_Index != 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of signal from item combobox.

   Enable or disable specified combobox item in all subwidgets.

   \param[in]       ou32_Index    Combobox index of item that should be enabled/disabled
   \param[in]       oq_Enable     flag of enable or disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentWidget::m_UpdateComboboxEntries(const uint32 ou32_Index, const bool oq_Enable,
                                                                 const C_CieDbcImportNodeAssignmentItemWidget * const opc_Sender)
{
   std::vector<C_CieDbcImportNodeAssignmentItemWidget *>::const_iterator c_It;

   for (c_It = this->mc_Entries.begin(); c_It != this->mc_Entries.end(); ++c_It)
   {
      if (*c_It != opc_Sender) // skip the sender item
      {
         const C_CieDbcImportNodeAssignmentItemWidget & rc_CurEntry = **c_It;
         rc_CurEntry.UpdateComboboxEntries(ou32_Index, oq_Enable);
      }
   }
}

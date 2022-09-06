//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver edit entry (implementation)

   One message receiver edit entry

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "TGLUtils.h"
#include "C_Uti.h"
#include "C_SdUtil.h"
#include "C_GtGetText.h"
#include "C_SdBueMessageRxEntry.h"
#include "ui_C_SdBueMessageRxEntry.h"

#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_OgeLabGroupItem.h"
#include "C_SdBueMessageRxTimeoutConfig.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxEntry::C_SdBueMessageRxEntry(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageRxEntry),
   mq_HasChildren(false),
   mq_NodeLayer(true),
   mq_ModeSingleNode(false),
   mu32_NodeIndex(0U),
   mu32_InterfaceIndex(0U),
   mu32_DatapoolIndex(0U),
   mu32_LastKnownCycleTimeValue(10U),
   mu32_AutoReceiveTimeoutValue(40U),
   me_ReceiveTimeoutMode(C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM),
   mu32_ReceiveTimeoutValue(0U),
   mq_TxMethodOnEvent(false),
   mq_DisableOptionPossible(false),
   mq_TimeoutConfigurationReadOnly(false)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();
   m_HandleInactiveStates();

   //Icon
   this->mc_NodeActive = C_SdUtil::h_InitStaticIcon("://images/system_definition/IconNode.svg");
   this->mc_NodeInactive = C_SdUtil::h_InitStaticIcon("://images/system_definition/IconNodeInactive.svg");
   this->mc_DatapoolActive = C_SdUtil::h_InitStaticIcon("://images/system_definition/IconDataPoolSmall.svg");
   this->mc_DatapoolInactive = C_SdUtil::h_InitStaticIcon("://images/system_definition/IconDataPoolSmallInactive.svg");

   connect(this->mpc_Ui->pc_CheckBoxActive, &stw_opensyde_gui_elements::C_OgeChxDefaultSmall::stateChanged,
           this, &C_SdBueMessageRxEntry::m_OnCheckBoxStateChanged);
   connect(this->mpc_Ui->pc_LabelTimeoutLink, &QLabel::linkActivated,
           this, &C_SdBueMessageRxEntry::m_OnTimeoutConfigLinkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxEntry::~C_SdBueMessageRxEntry(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelTimeoutLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LabelTimeoutLink->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LabelTimeoutLink->setFocusPolicy(Qt::NoFocus);

   this->mpc_Ui->pc_LabelTimeoutLink->SetToolTipInformation(
      C_GtGetText::h_GetText("Timeout"),
      C_GtGetText::h_GetText("This property is relevant for source code file generation. "
                             "\nWithin this interval a valid message should be received. "
                             "\nOtherwise the COMM stack (implemented on device) will report an "
                             "error."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init

   \param[in]  orc_EntryName              Entry name
   \param[in]  ou32_NodeIndex             Node index (ID)
   \param[in]  ou32_InterfaceIndex        Interface Index (ID)
   \param[in]  orc_ReceiveTimeoutModes    Mode whether to use auto receive timeout, custom or disabled for
                                          each Rx messages associated to each Datapool
   \param[in]  orc_ReceiveTimeoutValues   Receive timeout values for each Rx messages associated to each Datapool
   \param[in]  orc_DatapoolIndexes        All Datapool indexes the Rx message is associated to
   \param[in]  orc_DatapoolNames          All Datapool names the Rx message is associated to
   \param[in]  oq_NodeLayer               Flag if Entry represents an entire node or a specific Datapool
   \param[in]  oq_ReadOnly                Flag if elements shall be read only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::Init(const QString & orc_EntryName, const uint32 ou32_NodeIndex,
                                 const uint32 ou32_InterfaceIndex,
                                 const std::vector<C_PuiSdNodeCanMessage::E_RxTimeoutMode> & orc_ReceiveTimeoutModes,
                                 const std::vector<uint32> & orc_ReceiveTimeoutValues,
                                 const std::vector<uint32> & orc_DatapoolIndexes,
                                 const std::vector<QString> & orc_DatapoolNames, const bool oq_NodeLayer,
                                 const bool oq_ReadOnly)
{
   tgl_assert(orc_DatapoolIndexes.size() > 0);
   tgl_assert(orc_DatapoolIndexes.size() == orc_ReceiveTimeoutValues.size());
   tgl_assert(orc_DatapoolIndexes.size() == orc_ReceiveTimeoutModes.size());
   tgl_assert(orc_DatapoolIndexes.size() == orc_DatapoolNames.size());

   this->mpc_Ui->pc_CheckBoxActive->setEnabled(!oq_ReadOnly);

   this->mq_NodeLayer = oq_NodeLayer;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_InterfaceIndex = ou32_InterfaceIndex;

   if (this->mq_NodeLayer == true)
   {
      this->mpc_Ui->pc_CheckBoxActive->setText(orc_EntryName);
      this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeInactive);
   }
   else
   {
      // It is an entry specific for one Datapool on Datapool layer
      this->mpc_Ui->pc_CheckBoxActive->setText(orc_DatapoolNames[0]);
      this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_DatapoolInactive);
   }

   if (orc_DatapoolIndexes.size() == 1)
   {
      // Only one Datapool. No sub entries necessary.
      this->mq_HasChildren = false;
      this->mu32_DatapoolIndex = orc_DatapoolIndexes[0];
      this->me_ReceiveTimeoutMode = orc_ReceiveTimeoutModes[0];
      this->mu32_ReceiveTimeoutValue = orc_ReceiveTimeoutValues[0];
      this->mpc_Ui->pc_GroupBoxSubEntries->hide();
      this->m_UpdateTimeoutLink();
   }
   else
   {
      uint32 u32_ItEntry;

      // The Datapool layer can not have sub entries
      tgl_assert(this->mq_NodeLayer == true);
      this->mq_HasChildren = true;

      // Create for each Datapool an own sub entry
      for (u32_ItEntry = 0U; u32_ItEntry < orc_DatapoolIndexes.size(); ++u32_ItEntry)
      {
         std::vector<uint32> c_NodeDatapoolIndexes;
         std::vector<QString> c_NodeDatapoolNames;
         std::vector<C_PuiSdNodeCanMessage::E_RxTimeoutMode> c_ReceiveTimeoutModes;
         std::vector<uint32> c_ReceiveTimeoutValues;
         C_SdBueMessageRxEntry * const pc_Entry = new C_SdBueMessageRxEntry(this);

         // Special handling for node toggled signal
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeToggled, this,
                 &C_SdBueMessageRxEntry::m_OnNodeDatapoolToggled);
         // Forwarding all other signals
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeReceiveTimeout, this,
                 &C_SdBueMessageRxEntry::SigNodeReceiveTimeout);
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeReceiveTimeoutMode, this,
                 &C_SdBueMessageRxEntry::SigNodeReceiveTimeoutMode);

         c_NodeDatapoolIndexes.push_back(orc_DatapoolIndexes[u32_ItEntry]);
         c_NodeDatapoolNames.push_back(orc_DatapoolNames[u32_ItEntry]);
         c_ReceiveTimeoutModes.push_back(orc_ReceiveTimeoutModes[u32_ItEntry]);
         c_ReceiveTimeoutValues.push_back(orc_ReceiveTimeoutValues[u32_ItEntry]);

         pc_Entry->Init(orc_EntryName, this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                        c_ReceiveTimeoutModes, c_ReceiveTimeoutValues, c_NodeDatapoolIndexes,
                        c_NodeDatapoolNames, false, oq_ReadOnly);
         pc_Entry->SetLastKnownCycleTimeValue(this->mu32_LastKnownCycleTimeValue);
         pc_Entry->SetRxTimeoutPreconditions(this->mq_TxMethodOnEvent, this->mq_DisableOptionPossible);
         pc_Entry->SetRxTimeoutConfigurationReadOnly(this->mq_TimeoutConfigurationReadOnly);
         this->mpc_Ui->pc_VerticalLayout->insertWidget(this->mpc_Ui->pc_VerticalLayout->count() - 1, pc_Entry);
         this->mc_Entries.push_back(pc_Entry);
      } //lint !e429 //cleanup handled by Qt engine
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update last known cycle time value

   \param[in]  ou32_Value  Last known cycle time value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetLastKnownCycleTimeValue(const uint32 ou32_Value)
{
   this->mu32_LastKnownCycleTimeValue = ou32_Value;

   this->mu32_AutoReceiveTimeoutValue = (this->mu32_LastKnownCycleTimeValue * 3UL) + 10UL;
   //limit to a maximum of 65535 so it can fit into 16bit:
   if (this->mu32_AutoReceiveTimeoutValue > 65535U)
   {
      this->mu32_AutoReceiveTimeoutValue = 65535U;
   }

   if (this->mq_HasChildren == false)
   {
      m_UpdateAndAdaptRxTimeoutValue();

      // Update Link Text
      this->m_UpdateTimeoutLink();
   }
   else
   {
      uint32 u32_Counter;

      // Inform the children
      for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
      {
         C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_Counter];

         if (pc_Entry != NULL)
         {
            pc_Entry->SetLastKnownCycleTimeValue(ou32_Value);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set flags of Tx method

   \param[in]  oq_TxMethodOnEvent              Flag if Tx message mode is on event
   \param[in]  oq_DisableOptionPossible        Flag if timeout option disable is available
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetRxTimeoutPreconditions(const bool oq_TxMethodOnEvent,
                                                      const bool oq_DisableOptionPossible)
{
   if ((this->mq_TimeoutConfigurationReadOnly == false) &&
       (oq_TxMethodOnEvent == false) &&
       (this->mq_TxMethodOnEvent != oq_TxMethodOnEvent) &&
       (this->me_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED))
   {
      // Special case: Switching from on event with disabled receive timeout to an other Tx method
      // Disabled state is not possible anymore
      // Set default values
      this->me_ReceiveTimeoutMode = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
      this->SetLastKnownCycleTimeValue(100U);
      this->m_UpdateTimeoutLink();
   }

   this->mq_TxMethodOnEvent = oq_TxMethodOnEvent;
   this->mq_DisableOptionPossible = oq_DisableOptionPossible;

   m_HandleInactiveStates();

   uint32 u32_Counter;

   // Inform the children too
   if (this->mq_HasChildren == true)
   {
      // Inform the children
      for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
      {
         C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_Counter];

         if (pc_Entry != NULL)
         {
            pc_Entry->SetRxTimeoutPreconditions(oq_TxMethodOnEvent, oq_DisableOptionPossible);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the timeout configuration enabled or disabled

   \param[in] oq_TimeoutConfigurationReadOnly   Flag if timeout configuration is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetRxTimeoutConfigurationReadOnly(const bool oq_TimeoutConfigurationReadOnly)
{
   this->mq_TimeoutConfigurationReadOnly = oq_TimeoutConfigurationReadOnly;

   if (oq_TimeoutConfigurationReadOnly == true)
   {
      // Special case: The entire time configuration is deactivated.
      this->m_UpdateTimeoutLink();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked state

   \param[in]  ou32_DatapoolIndex   Datapool Index (ID)
   \param[in]  oq_Checked           New state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetChecked(const uint32 ou32_DatapoolIndex, const bool oq_Checked) const
{
   if (this->mq_HasChildren == false)
   {
      // Entry represents only one element
      this->mpc_Ui->pc_CheckBoxActive->setChecked(oq_Checked);
   }
   else
   {
      uint32 u32_Counter;

      // Check for the correct entry with the correct Datapool index
      for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
      {
         C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_Counter];

         if (pc_Entry != NULL)
         {
            if (pc_Entry->GetDatapoolIndex() == ou32_DatapoolIndex)
            {
               pc_Entry->SetChecked(ou32_DatapoolIndex, oq_Checked);
               break;
            }
         }
      }
   }

   m_HandleInactiveStates();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the check box enabled or disabled

   \param[in]  oq_Enabled  Flag if the check box is enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetEnabled(const bool oq_Enabled) const
{
   this->mpc_Ui->pc_CheckBoxActive->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a specific tooltip

   \param[in]       orc_Tooltip     Text for showing tool tip (Empty string for no change)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetSpecificToolTip(const QString & orc_Tooltip)
{
   if (orc_Tooltip != "")
   {
      this->mpc_Ui->pc_LabelTimeoutLink->SetToolTipInformation(
         C_GtGetText::h_GetText("Timeout"),
         orc_Tooltip);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state if the entry has children

   \retval   true    Entry has children
   \retval   false   Entry has no children
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageRxEntry::HasChildren(void) const
{
   return this->mq_HasChildren;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if match on node layer

   \param[in]  ou32_NodeIndex       Node index (ID)
   \param[in]  ou32_InterfaceIndex  Interface Index (ID)

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageRxEntry::DoesMatch(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex) const
{
   bool q_Retval;

   if ((this->mu32_NodeIndex == ou32_NodeIndex) &&
       (this->mu32_InterfaceIndex == ou32_InterfaceIndex))
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
/*! \brief   Sets the flag for the single node mode

   \param[in]  oq_ModeSingleNode    Flag of single node mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetModeSingleNode(const bool oq_ModeSingleNode)
{
   this->mq_ModeSingleNode = oq_ModeSingleNode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the active state

   \retval   true    The Rx message of the node/interface and Datapool is active
   \retval   false   Detailed description of 2nd return value
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageRxEntry::IsChecked(void) const
{
   return this->mpc_Ui->pc_CheckBoxActive->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the associated Datapool index

   \return
   Datapool index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdBueMessageRxEntry::GetDatapoolIndex(void) const
{
   return this->mu32_DatapoolIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of check box state change

   Similar to slot for toggle, but also reacts to click of partially checked checkbox.

   \param[in]  osn_CheckState    check state checked/unchecked/partially checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnCheckBoxStateChanged(const sintn osn_CheckState)
{
   // Adapt icon (blue/grey/datapool/node)
   this->m_ToggleIcon(this->mpc_Ui->pc_CheckBoxActive->isChecked()); // use Qt function to map partial check state
                                                                     // correctly

   // If node without children or datapool was toggled: just forward change
   if (this->mq_HasChildren == false)
   {
      m_HandleInactiveStates();
      Q_EMIT (this->SigNodeToggled(this->mu32_NodeIndex, this->mu32_InterfaceIndex, this->mu32_DatapoolIndex,
                                   this->mpc_Ui->pc_CheckBoxActive->isChecked()));
   }
   else
   {
      // Avoid partially checked after user click
      if ((this->mq_NodeLayer == true) && (osn_CheckState == static_cast<sintn>(Qt::PartiallyChecked)))
      {
         this->mpc_Ui->pc_CheckBoxActive->setChecked(true);
      }
      else
      {
         // check or uncheck all children
         switch (static_cast<Qt::CheckState>(osn_CheckState))
         {
         case Qt::Unchecked:
            this->m_ToggleSubItems(false);
            break;
         case Qt::PartiallyChecked:
            // only visual update - do nothing
            break;
         case Qt::Checked:
            this->m_ToggleSubItems(true);
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Toggle all sub items

   \param[in]  oq_Checked  Checked vs unchecked flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_ToggleSubItems(const bool oq_Checked)
{
   // Adapt all sub entries
   uint32 u32_Counter;

   // Check for the correct entry with the correct Datapool index
   for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
   {
      C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_Counter];

      if (pc_Entry != NULL)
      {
         // In case of sub entries the Datapool index is not relevant
         pc_Entry->SetChecked(0U, oq_Checked);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for a toggled sub entry

   \param[in]  ou32_NodeIndex       Node index (ID)
   \param[in]  ou32_InterfaceIndex  Interface index (ID)
   \param[in]  ou32_DatapoolIndex   Datapool index (ID)
   \param[in]  oq_Checked           Flag if checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnNodeDatapoolToggled(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                    const uint32 ou32_DatapoolIndex, const bool oq_Checked)
{
   if (this->mq_HasChildren == true)
   {
      // Adapt the checkbox of the parent item if necessary
      this->m_AdaptParentCheckBoxState();

      if (this->mq_ModeSingleNode == true)
      {
         uint32 u32_Counter;
         bool q_SecondCheckedEntryFound = false;
         C_SdBueMessageRxEntry * pc_LastCheckedEntry = NULL;

         // Check if only one checked Datapool is left.
         for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
         {
            C_SdBueMessageRxEntry * const pc_SubEntry = this->mc_Entries[u32_Counter];

            if (pc_SubEntry->IsChecked() == true)
            {
               if (pc_LastCheckedEntry != NULL)
               {
                  // When two entries are checked, no sub entry has to be disabled
                  q_SecondCheckedEntryFound = true;
               }
               else
               {
                  pc_LastCheckedEntry = pc_SubEntry;
               }
            }

            // Reactivate all other entries. The found entry for disabling will be disabled in the next step,
            // if necessary
            pc_SubEntry->SetEnabled(true);
         }

         if ((pc_LastCheckedEntry != NULL) &&
             (q_SecondCheckedEntryFound == false))
         {
            // Only one entry is left
            pc_LastCheckedEntry->SetEnabled(false);
         }
      }

      // Forwarding the signal
      Q_EMIT (this->SigNodeToggled(ou32_NodeIndex, ou32_InterfaceIndex, ou32_DatapoolIndex, oq_Checked));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the state of the active checkbox dependent of the state of all sub entries

   Works only if mq_HasChildren == true
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_AdaptParentCheckBoxState(void) const
{
   if (this->mq_HasChildren == true)
   {
      uint32 u32_Counter;
      bool q_AtLeastOneActive = false;
      bool q_AtLeastOneInactive = false;

      // Check for the three scenarios
      // 1: No entry is active
      // 2: All entries are active
      // 3: Only a part of all entries is active
      for (u32_Counter = 0U; u32_Counter < this->mc_Entries.size(); ++u32_Counter)
      {
         if (this->mc_Entries[u32_Counter]->IsChecked() == true)
         {
            q_AtLeastOneActive = true;
         }
         else
         {
            q_AtLeastOneInactive = true;
         }

         if ((q_AtLeastOneActive == true) && (q_AtLeastOneInactive == true))
         {
            // No more information necessary
            break;
         }
      }

      // Disconnect due to avoiding informing the sub entries again
      disconnect(this->mpc_Ui->pc_CheckBoxActive, &stw_opensyde_gui_elements::C_OgeChxDefaultSmall::stateChanged,
                 this, &C_SdBueMessageRxEntry::m_OnCheckBoxStateChanged);

      // Adapt the checkbox of the parent
      if ((q_AtLeastOneActive == true) && (q_AtLeastOneInactive == true))
      {
         // Scenario 3
         this->mpc_Ui->pc_CheckBoxActive->setEnabled(true);
         this->mpc_Ui->pc_CheckBoxActive->setCheckState(Qt::PartiallyChecked);
         this->m_ToggleIcon(true);
      }
      else if ((q_AtLeastOneActive == true) && (q_AtLeastOneInactive == false))
      {
         // Scenario 2
         this->mpc_Ui->pc_CheckBoxActive->setChecked(true);
         this->m_ToggleIcon(true);

         // In case of a not partial checked or not checked check box and the mode single node,
         // deactivating of all Datapools is not allowed
         this->mpc_Ui->pc_CheckBoxActive->setEnabled(!this->mq_ModeSingleNode);
      }
      else
      {
         // Scenario 1
         this->mpc_Ui->pc_CheckBoxActive->setEnabled(true);
         this->mpc_Ui->pc_CheckBoxActive->setChecked(false);
         this->m_ToggleIcon(false);
      }

      connect(this->mpc_Ui->pc_CheckBoxActive, &stw_opensyde_gui_elements::C_OgeChxDefaultSmall::stateChanged,
              this, &C_SdBueMessageRxEntry::m_OnCheckBoxStateChanged);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt checkbox icon (the image between checkbox and text)

   \param[in]  oq_Checked  new state of checkbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_ToggleIcon(const bool oq_Checked) const
{
   if (oq_Checked == true)
   {
      if (this->mq_NodeLayer == true)
      {
         this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeActive);
      }
      else
      {
         this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_DatapoolActive);
      }
   }
   else
   {
      if (this->mq_NodeLayer == true)
      {
         this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeInactive);
      }
      else
      {
         this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_DatapoolInactive);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Opens the dialog for timeout configuration of Rx message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnTimeoutConfigLinkClicked(void)
{
   if (this->mq_HasChildren == false)
   {
      QPointer<C_OgePopUpDialog> c_New;
      C_SdBueMessageRxTimeoutConfig * pc_Dialog;
      QString c_NodeName = "";

      if (this->mq_NodeLayer == false)
      {
         // add node name if and only if Datapool case
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_Node != NULL)
         {
            c_NodeName = pc_Node->c_Properties.c_Name.c_str();
            c_NodeName += ".";
         }
      }

      //Show dialog
      c_New = new C_OgePopUpDialog(this, this);
      pc_Dialog = new C_SdBueMessageRxTimeoutConfig(*c_New, this->me_ReceiveTimeoutMode,
                                                    this->mq_TxMethodOnEvent,
                                                    this->mq_DisableOptionPossible,
                                                    this->mu32_ReceiveTimeoutValue, this->mu32_LastKnownCycleTimeValue,
                                                    this->mu32_AutoReceiveTimeoutValue,
                                                    c_NodeName + this->mpc_Ui->pc_CheckBoxActive->text());

      Q_UNUSED(pc_Dialog)

      //Resize
      c_New->SetSize(QSize(750, 380));

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         const C_PuiSdNodeCanMessage::E_RxTimeoutMode e_NewReceiveTimeoutMode = pc_Dialog->GetReceiveTimeoutMode();
         const uint32 u32_NewReceiveTimeoutValue = pc_Dialog->GetReceiveTimeoutValue();

         // Check change of receive timeout flag
         if (e_NewReceiveTimeoutMode != this->me_ReceiveTimeoutMode)
         {
            this->me_ReceiveTimeoutMode = e_NewReceiveTimeoutMode;
            Q_EMIT (this->SigNodeReceiveTimeoutMode(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                                    this->mu32_DatapoolIndex, this->me_ReceiveTimeoutMode));

            this->m_UpdateAndAdaptRxTimeoutValue();
            // Update Link Text
            this->m_UpdateTimeoutLink();
         }

         // Check of change of timeout value in case of not the auto timeout mode
         if ((this->me_ReceiveTimeoutMode != C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO) &&
             (this->mu32_ReceiveTimeoutValue != u32_NewReceiveTimeoutValue))
         {
            this->mu32_ReceiveTimeoutValue = u32_NewReceiveTimeoutValue;
            Q_EMIT (this->SigNodeReceiveTimeout(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                                this->mu32_DatapoolIndex,
                                                this->mu32_ReceiveTimeoutValue));

            // Update Link Text
            this->m_UpdateTimeoutLink();
         }
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the inactive states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_HandleInactiveStates(void) const
{
   const bool q_Active = (this->mpc_Ui->pc_CheckBoxActive->isChecked()) &&
                         (this->mq_HasChildren == false);

   this->mpc_Ui->pc_LabelTimeoutLink->setVisible(q_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update automatic receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_UpdateAndAdaptRxTimeoutValue(void)
{
   if ((this->mpc_Ui->pc_CheckBoxActive->isChecked() == true) &&
       (this->mq_HasChildren == false))
   {
      bool q_DoEmit = false;

      if (this->me_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO)
      {
         this->mu32_ReceiveTimeoutValue = this->mu32_AutoReceiveTimeoutValue;
         q_DoEmit = true;
      }
      else if (this->me_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM)
      {
         if (this->mu32_LastKnownCycleTimeValue != 0U)
         {
            //we need to limit the maximum to cycle_time * 100 to prevent "TxGap" overflow (only has uint8)
            if (this->mu32_ReceiveTimeoutValue > (this->mu32_LastKnownCycleTimeValue * 100U))
            {
               this->mu32_ReceiveTimeoutValue = this->mu32_LastKnownCycleTimeValue * 100U;
               q_DoEmit = true;
            }
            //we need to limit the minimum too
            if (this->mu32_ReceiveTimeoutValue < this->mu32_LastKnownCycleTimeValue)
            {
               this->mu32_ReceiveTimeoutValue = this->mu32_LastKnownCycleTimeValue;
               q_DoEmit = true;
            }
         }
      }
      else
      {
         //nothing to do ...
      }

      if (q_DoEmit == true)
      {
         Q_EMIT (this->SigNodeReceiveTimeout(this->mu32_NodeIndex, this->mu32_InterfaceIndex, this->mu32_DatapoolIndex,
                                             this->mu32_ReceiveTimeoutValue));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set timeout link text to current timeout data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_UpdateTimeoutLink(void) const
{
   QString c_LinkText;

   // Timeout info as link text
   c_LinkText = C_GtGetText::h_GetText("Timeout: ");

   if (this->me_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED)
   {
      c_LinkText += C_GtGetText::h_GetText("Disabled");
   }
   else
   {
      if (this->me_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO)
      {
         c_LinkText += QString::number(this->mu32_AutoReceiveTimeoutValue);
         c_LinkText += C_GtGetText::h_GetText(" ms (Auto)");
      }
      else
      {
         c_LinkText += QString::number(this->mu32_ReceiveTimeoutValue);
         c_LinkText += C_GtGetText::h_GetText(" ms (Custom)");
      }
   }

   if (this->mq_TimeoutConfigurationReadOnly == false)
   {
      this->mpc_Ui->pc_LabelTimeoutLink->setText(static_cast<QString>(C_Uti::h_GetLink(c_LinkText,
                                                                                       mc_STYLE_GUIDE_COLOR_6,
                                                                                       "Link")));
   }
   else
   {
      this->mpc_Ui->pc_LabelTimeoutLink->setText(static_cast<QString>(C_Uti::h_GetLink(c_LinkText,
                                                                                       mc_STYLE_GUIDE_COLOR_36,
                                                                                       "Link")));
   }

   this->mpc_Ui->pc_LabelTimeoutLink->setEnabled(!this->mq_TimeoutConfigurationReadOnly);
}

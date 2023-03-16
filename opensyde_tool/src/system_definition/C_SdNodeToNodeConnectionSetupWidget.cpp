//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node to Node connection setup pop up dialog

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNodeToNodeConnectionSetupWidget.hpp"
#include "ui_C_SdNodeToNodeConnectionSetupWidget.h"
#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      oru32_Node1Index    Node 1 index
   \param[in]      oru32_Node2Index    Node 2 index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeToNodeConnectionSetupWidget::C_SdNodeToNodeConnectionSetupWidget(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t & oru32_Node1Index,
   const uint32_t & oru32_Node2Index) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNodeToNodeConnectionSetupWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_Node1Index(oru32_Node1Index),
   mu32_Node2Index(oru32_Node2Index),
   mu32_NodeId1(0),
   mu32_NodeId2(0),
   mq_InteractionPossible(false),
   mc_BusTypeCan(C_GtGetText::h_GetText("CAN")),
   mc_BusTypeEthernet(C_GtGetText::h_GetText("Ethernet"))
{
   // init UI
   this->mpc_Ui->setupUi(this);

   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();
   m_InitFromData();

   //Remove debug string
   this->mpc_Ui->pc_GroupBoxInterfaces1->setTitle("");
   this->mpc_Ui->pc_GroupBoxInterfaces2->setTitle("");

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SdNodeToNodeConnectionSetupWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_RadioButtonCreateNew, &stw::opensyde_gui_elements::C_OgeRabProperties::toggled, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnNewOrExistingChange);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_ComboBoxBusType, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNodeToNodeConnectionSetupWidget::m_OnBusTypeChange);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_ComboBoxExistingBus,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnExistingBusChange);
   connect(this->mpc_Ui->pc_LineEditBusName, &stw::opensyde_gui_elements::C_OgeLeProperties::textChanged, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnBusNameChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeToNodeConnectionSetupWidget::~C_SdNodeToNodeConnectionSetupWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Node to Node"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Link Setup"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelInterfaceHeading->setText(C_GtGetText::h_GetText("2. Select COMM Interface"));
   this->mpc_Ui->pc_LabelBusHeading->setText(C_GtGetText::h_GetText("1. Select Bus"));
   this->mpc_Ui->pc_LabelBusType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabelBusName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_RadioButtonCreateNew->setText(C_GtGetText::h_GetText("Create New Bus"));
   this->mpc_Ui->pc_RadioButtonSelectExisting->setText(C_GtGetText::h_GetText("Select Existing Bus"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNodeToNodeConnectionSetupWidget::GetSelectedInterface1(void) const
{
   return C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf1, this->mu32_Node1Index,
                                             this->GetBusType());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNodeToNodeConnectionSetupWidget::GetSelectedInterface2(void) const
{
   return C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf2, this->mu32_Node2Index,
                                             this->GetBusType());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current active bus type

   \return
   Current active bus type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSystemBus::E_Type C_SdNodeToNodeConnectionSetupWidget::GetBusType(void) const
{
   C_OscSystemBus::E_Type e_Retval;

   if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
   {
      if (this->mpc_Ui->pc_ComboBoxBusType->currentText() == this->mc_BusTypeEthernet)
      {
         e_Retval = C_OscSystemBus::eETHERNET;
      }
      else
      {
         e_Retval = C_OscSystemBus::eCAN;
      }
   }
   else
   {
      const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->GetBusIndex());
      tgl_assert(pc_Bus != NULL);
      if (pc_Bus != NULL)
      {
         e_Retval = pc_Bus->e_Type;
      }
      else
      {
         e_Retval = C_OscSystemBus::eCAN;
      }
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current selected bus index

   Warning: Only use if user has selected "use existing bus" option

   \return
   Current selected bus index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNodeToNodeConnectionSetupWidget::GetBusIndex(void) const
{
   uint32_t u32_Retval = static_cast<uint32_t>(this->mpc_Ui->pc_ComboBoxExistingBus->currentIndex());

   if (u32_Retval < this->mc_BusIndices.size())
   {
      u32_Retval = this->mc_BusIndices[u32_Retval];
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current bus name

   Warning: Only use if user has selected "use existing bus" option

   \return
   Current bus name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNodeToNodeConnectionSetupWidget::GetBusName(void) const
{
   return this->mpc_Ui->pc_LineEditBusName->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if creation of new bus requested

   \return
   True  Create new
   False Use existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::CheckIfCreateNew(void) const
{
   return this->mpc_Ui->pc_RadioButtonCreateNew->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interaction possible flag

   \return
   True  Interaction possible
   False Invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::GetInteractionPossible(void) const
{
   return this->mq_InteractionPossible;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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
void C_SdNodeToNodeConnectionSetupWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_InitFromData(void)
{
   const C_OscNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node1Index);
   const C_OscNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node2Index);

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      bool q_NewBusPossible;
      bool q_ExistingBusExists;
      bool q_NewBusRestricted;
      bool q_ExistingBusRestricted;
      stw::opensyde_core::C_OscSystemBus::E_Type e_NewBusRestrictedType;
      stw::opensyde_core::C_OscSystemBus::E_Type e_ExistingBusRestrictedType;

      this->m_CheckTypeRestrictions(q_NewBusPossible,  q_NewBusRestricted, e_NewBusRestrictedType, q_ExistingBusExists,
                                    q_ExistingBusRestricted,
                                    e_ExistingBusRestrictedType);

      this->mq_InteractionPossible = q_NewBusPossible || q_ExistingBusExists;

      // If no interaction is possible, this dialog will not be shown
      if (this->mq_InteractionPossible == true)
      {
         // Adapt enable states if necessary
         if (q_NewBusPossible == true)
         {
            const C_OscSystemBus c_OscBus;

            if (q_ExistingBusExists == false)
            {
               this->mpc_Ui->pc_RadioButtonSelectExisting->setEnabled(false);
            }

            //Init name
            this->mpc_Ui->pc_LineEditBusName->setText(C_Uti::h_GetUniqueName(C_PuiSdHandler::h_GetInstance()->
                                                                             GetExistingBusNames(),
                                                                             c_OscBus.c_Name).c_str());
         }
         else
         {
            // Only a connect to an already existing bus is possible. No new bus can be created, the target node
            // has no further free interfaces
            this->mpc_Ui->pc_RadioButtonCreateNew->setEnabled(false);
            this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(true);
            this->mpc_Ui->pc_LabelBusType->setEnabled(false);
            this->mpc_Ui->pc_ComboBoxBusType->setEnabled(false);
            this->mpc_Ui->pc_LabelBusName->setEnabled(false);
            this->mpc_Ui->pc_LineEditBusName->setEnabled(false);
            this->mpc_Ui->pc_RadioButtonSelectExisting->setChecked(true);
         }

         // Combo box for bus type
         this->mpc_Ui->pc_ComboBoxBusType->clear();
         if (q_NewBusPossible == true)
         {
            if (q_NewBusRestricted == true)
            {
               if (e_NewBusRestrictedType == C_OscSystemBus::eCAN)
               {
                  this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BusTypeCan);
               }
               else
               {
                  this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BusTypeEthernet);
               }
            }
            else
            {
               this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BusTypeCan);
               this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BusTypeEthernet);
            }
         }

         //Dynamic labels
         this->mpc_Ui->pc_LabelNode1Interfaces->setText(C_PuiSdUtil::h_GetNodeBaseNameOrName(this->mu32_Node1Index));
         this->mpc_Ui->pc_LabelNode2Interfaces->setText(C_PuiSdUtil::h_GetNodeBaseNameOrName(this->mu32_Node2Index));

         //Existing buses
         this->mpc_Ui->pc_ComboBoxExistingBus->clear();
         if (q_ExistingBusExists == true)
         {
            const uint32_t u32_BusCount = C_PuiSdHandler::h_GetInstance()->GetOscBusesSize();

            this->mc_BusIndices.clear();
            for (uint32_t u32_ItBus = 0; u32_ItBus < u32_BusCount; ++u32_ItBus)
            {
               uint32_t u32_InterfaceCounter;
               const C_OscSystemBus * pc_Bus = NULL;

               // Check if this bus is already connected to the target
               for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < pc_Node2->c_Properties.c_ComInterfaces.size();
                    ++u32_InterfaceCounter)
               {
                  if ((pc_Node2->c_Properties.c_ComInterfaces[u32_InterfaceCounter].GetBusConnectedRawValue() ==
                       true) &&
                      (pc_Node2->c_Properties.c_ComInterfaces[u32_InterfaceCounter].u32_BusIndex == u32_ItBus))
                  {
                     // Bus is connected to at least one interface of the node
                     pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(u32_ItBus);
                     break;
                  }
               }

               if (pc_Bus != NULL)
               {
                  QString c_Text = pc_Bus->c_Name.c_str();

                  if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
                  {
                     c_Text += " (" + QString::number(pc_Bus->u64_BitRate / 1000ULL);
                     c_Text += " kbit/s)";
                  }

                  if ((q_ExistingBusRestricted == false) ||
                      (pc_Bus->e_Type == e_ExistingBusRestrictedType))
                  {
                     // Add only if no restrictions for an existing bus exists or the bus matches
                     this->mpc_Ui->pc_ComboBoxExistingBus->addItem(c_Text);
                     this->mc_BusIndices.push_back(u32_ItBus);
                  }
               }
            }
         }

         m_HandleTypeChange();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of current bus type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_HandleTypeChange(void) const
{
   const C_OscNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node1Index);
   const C_OscNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node2Index);

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      const C_OscSystemBus::E_Type e_Type = this->GetBusType();

      // Node 1
      C_SdUtil::h_InitNodeInterfaceComboBox(*pc_Node1,
                                            e_Type,
                                            this->mpc_Ui->pc_ComboBoxComIntf1, -1);

      // Node 2
      if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
      {
         // Target node has at least one free interface and "create new bus" is selected
         C_SdUtil::h_InitNodeInterfaceComboBox(
            *pc_Node2, e_Type, this->mpc_Ui->pc_ComboBoxComIntf2, -1);

         this->mpc_Ui->pc_ComboBoxComIntf2->setEnabled(true);
      }
      else
      {
         const uint32_t u32_CurBusIndex = this->GetBusIndex();
         uint32_t u32_Intfcounter;

         // Select the used interface for the current bus which is not changeable for the target node in this case
         for (u32_Intfcounter = 0U; u32_Intfcounter < pc_Node2->c_Properties.c_ComInterfaces.size(); ++u32_Intfcounter)
         {
            const C_OscNodeComInterfaceSettings & rc_Interface =
               pc_Node2->c_Properties.c_ComInterfaces[u32_Intfcounter];

            if ((rc_Interface.GetBusConnectedRawValue() == true) &&
                (rc_Interface.u32_BusIndex == u32_CurBusIndex))
            {
               // Target node can have no free interfaces and "select existing bus" is checked
               C_SdUtil::h_InitNodeInterfaceComboBox(
                  *pc_Node2, e_Type, this->mpc_Ui->pc_ComboBoxComIntf2,
                  static_cast<int32_t>(rc_Interface.u8_InterfaceNumber));

               this->mpc_Ui->pc_ComboBoxComIntf2->setEnabled(false);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new bus name value

   \param[in]  orc_Value   Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnBusNameChange(const QString & orc_Value) const
{
   const bool q_Valid = C_PuiSdHandler::h_GetInstance()->CheckBusNameAvailable(orc_Value.toStdString().c_str());

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditBusName, "Valid", q_Valid);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of new or existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnNewOrExistingChange(void)
{
   m_HandleTypeChange();
   if (this->CheckIfCreateNew() == true)
   {
      this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(false);
      this->mpc_Ui->pc_LabelBusType->setEnabled(true);
      this->mpc_Ui->pc_ComboBoxBusType->setEnabled(true);
      this->mpc_Ui->pc_LabelBusName->setEnabled(true);
      this->mpc_Ui->pc_LineEditBusName->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(true);
      this->mpc_Ui->pc_LabelBusType->setEnabled(false);
      this->mpc_Ui->pc_ComboBoxBusType->setEnabled(false);
      this->mpc_Ui->pc_LabelBusName->setEnabled(false);
      this->mpc_Ui->pc_LineEditBusName->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of new bus type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnBusTypeChange(void) const
{
   m_HandleTypeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnExistingBusChange(void)
{
   m_HandleTypeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any bus type restrictions necessary

   \param[out]  orq_NewBusPossible           Flag if a new bus is possible
   \param[out]  orq_NewBusRestricted         Flag if a new bus type is restricted
   \param[out]  ore_NewBusType               New bus type to restrict to (Only valid if orq_NewBusRestricted is true)
   \param[out]  orq_ExistingBusExists        Flag if at least one existing bus exists
   \param[out]  orq_ExistingBusRestricted    Flag if an existing bus type is restricted
   \param[out]  ore_ExistingBusType          Existing bus type to restrict to
                                             (Only valid if orq_ExistingBusRestricted is true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_CheckTypeRestrictions(bool & orq_NewBusPossible,
                                                                  bool & orq_NewBusRestricted,
                                                                  C_OscSystemBus::E_Type & ore_NewBusType,
                                                                  bool & orq_ExistingBusExists,
                                                                  bool & orq_ExistingBusRestricted,
                                                                  C_OscSystemBus::E_Type & ore_ExistingBusType) const
{
   const C_OscNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node1Index);
   const C_OscNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Node2Index);

   orq_NewBusPossible = false;
   orq_NewBusRestricted = false;
   orq_ExistingBusExists = false;
   orq_ExistingBusRestricted = false;

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      // Flags for the source node only
      const bool q_SourceCanWorks = C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node1->c_Properties.c_ComInterfaces,
                                                                            C_OscSystemBus::eCAN);
      const bool q_SourceEthWorks = C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node1->c_Properties.c_ComInterfaces,
                                                                            C_OscSystemBus::eETHERNET);
      // Flags for creating a new bus
      const bool q_NewCanWorks = q_SourceCanWorks &&
                                 C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node2->c_Properties.c_ComInterfaces,
                                                                         C_OscSystemBus::eCAN);
      const bool q_NewEthernetWorks = q_SourceEthWorks &&
                                      C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node2->c_Properties.c_ComInterfaces,
                                                                              C_OscSystemBus::eETHERNET);

      // Flags for using an existing bus
      const bool q_ExistingCanWorks = q_SourceCanWorks &&
                                      C_SdUtil::h_CheckNodeInterfaceConnected(pc_Node2->c_Properties.c_ComInterfaces,
                                                                              C_OscSystemBus::eCAN);
      const bool q_ExistingEthernetWorks = q_SourceEthWorks &&
                                           C_SdUtil::h_CheckNodeInterfaceConnected(
         pc_Node2->c_Properties.c_ComInterfaces, C_OscSystemBus::eETHERNET);

      // Adapt output parameter for new bus
      if ((q_NewCanWorks == true) && (q_NewEthernetWorks == true))
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = false;
      }
      else if (q_NewCanWorks == true)
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = true;
         ore_NewBusType = C_OscSystemBus::eCAN;
      }
      else if (q_NewEthernetWorks == true)
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = true;
         ore_NewBusType = C_OscSystemBus::eETHERNET;
      }
      else
      {
         // Creating a new bus is not possible
      }

      // Adapt output parameter for existing bus
      if ((q_ExistingCanWorks == true) && (q_ExistingEthernetWorks == true))
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = false;
      }
      else if (q_ExistingCanWorks == true)
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = true;
         ore_ExistingBusType = C_OscSystemBus::eCAN;
      }
      else if (q_ExistingEthernetWorks == true)
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = true;
         ore_ExistingBusType = C_OscSystemBus::eETHERNET;
      }
      else
      {
         // Using an existing bus is not possible
      }
   }
}

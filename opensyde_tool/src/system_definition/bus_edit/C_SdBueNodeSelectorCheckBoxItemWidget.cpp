//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       A concrete node selector widget for C_SdBueNodeSelectorCheckBoxListWidget (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_SdBueNodeSelectorCheckBoxItemWidget.h"
#include "ui_C_SdBueNodeSelectorCheckBoxItemWidget.h"

#include "C_OSCNode.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_GtGetText.h"
#include "TGLUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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

   \param[in]     ou32_NodeIndex      Index of node
   \param[in]     ou32_InterfaceIndex Index of node interface
   \param[in]     orc_Name            Name of node
   \param[in,out] opc_Parent          Optional pointer to parent

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxItemWidget::C_SdBueNodeSelectorCheckBoxItemWidget(const stw_types::uint32 ou32_NodeIndex,
                                                                             const stw_types::uint32 ou32_InterfaceIndex, const QString & orc_Name,
                                                                             QWidget * const opc_Parent) :
   C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdBueNodeSelectorCheckBoxItemWidget),
   mc_IconActive(":/images/system_definition/IconNode.svg"),
   mc_IconInactive(":/images/system_definition/IconNodeInactive.svg"),
   me_ProtocolType(stw_opensyde_core::C_OSCCanProtocol::eLAYER2)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   mpc_CheckBox = new C_OgeChxDefaultSmall(ou32_NodeIndex, ou32_InterfaceIndex, this);
   this->mpc_CheckBox->setText(orc_Name);
   this->mpc_CheckBox->setIcon(this->mc_IconInactive);

   this->mpc_Ui->pc_PushButtonImport->SetCustomIcons("://images/IconImportFile.svg",  // enabled
                                                     "://images/IconImportFile.svg",  // TODO: hovered
                                                     "://images/IconImportFile.svg",  // TODO: clicked
                                                     "://images/IconImportFile.svg"); // TODO: disabled

   //lint -e{64, 918, 1025, 1703}  false positive because of C++11 use of Qt
   connect(this->mpc_CheckBox, &C_OgeChxDefaultSmall::SigCheckBoxToggled,
           this, &C_SdBueNodeSelectorCheckBoxItemWidget::m_NodeToggled);
   connect(this->mpc_Ui->pc_PushButtonImport, &C_OgePubIconOnly::clicked, this,
           &C_SdBueNodeSelectorCheckBoxItemWidget::m_ImportClicked);

   this->mpc_Ui->pc_HorizontalLayout->insertWidget(0, this->mpc_CheckBox);

   this->UpdateToolTip();
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueNodeSelectorCheckBoxItemWidget::~C_SdBueNodeSelectorCheckBoxItemWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_CheckBox and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     23.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::InitStaticNames(void) const
{
   //Tool tips
   this->mpc_Ui->pc_PushButtonImport->SetToolTipInformation(C_GtGetText::h_GetText("Import"),
                                                            C_GtGetText::h_GetText(
                                                               "Import messages and signals from standard file formats (*.dbc, *.eds, *.dcf)"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the configured indexes

   \param[out]    ou32_Index       First index
   \param[out]    ou32_SubIndex    Second index

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::GetIndexes(stw_types::uint32 & oru32_Index,
                                                       stw_types::uint32 & oru32_SubIndex) const
{
   this->mpc_CheckBox->GetIndexes(oru32_Index, oru32_SubIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setting checked state of check box

   \param[in]     oq_Checked       Check flag

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetChecked(const bool oq_Checked)
{
   this->mpc_CheckBox->setChecked(oq_Checked);
   this->m_AdaptIcon(oq_Checked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returning the check state of the check box

   \return
   true: checked
   false: not checked

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdBueNodeSelectorCheckBoxItemWidget::IsChecked(void) const
{
   return this->mpc_CheckBox->isChecked();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]     oe_Protocol     Protocol id

   \created     09.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol)
{
   this->me_ProtocolType = oe_Protocol;

   this->UpdateToolTip();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the tool tip of the widget

   \created     09.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::UpdateToolTip(void)
{
   uint32 u32_NodeIndex;
   uint32 u32_InterfaceIndex;
   C_OSCNode * pc_Node;

   this->mpc_CheckBox->GetIndexes(u32_NodeIndex, u32_InterfaceIndex);
   pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      // Prepare tooltip
      QString c_Content = "";
      uint32 u32_DataPoolIndex = 0U;
      bool q_Found = false;

      c_Content += C_GtGetText::h_GetText("Interface: ");
      tgl_assert(u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      if (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCCanProtocol * pc_Protocol = NULL;
         uint32 u32_ProtocolCounter;

         // Get the interface name
         c_Content += C_PuiSdUtil::h_GetInterfaceName(
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex].e_InterfaceType,
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex].u8_InterfaceNumber);

         for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < pc_Node->c_ComProtocols.size(); ++u32_ProtocolCounter)
         {
            pc_Protocol = &pc_Node->c_ComProtocols[u32_ProtocolCounter];
            if (pc_Protocol->e_Type == this->me_ProtocolType)
            {
               break;
            }
         }

         if (pc_Protocol != NULL)
         {
            const C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[u32_InterfaceIndex];

            // Get the message count
            const uint32 u32_RxSize = rc_MessageContainer.c_RxMessages.size();
            const uint32 u32_TxSize = rc_MessageContainer.c_TxMessages.size();
            if ((u32_RxSize > 0U) ||
                (u32_TxSize > 0U))
            {
               c_Content += QString(C_GtGetText::h_GetText("\nMessages: %1 Tx, %2 Rx")).
                            arg(u32_TxSize).arg(u32_RxSize);
            }
            else
            {
               c_Content += C_GtGetText::h_GetText("\nNo Messages");
            }

            // Save the datapool index for the read out of the signal count
            u32_DataPoolIndex = pc_Protocol->u32_DataPoolIndex;
            q_Found = true;
         }
      }

      // Get the signal count
      if (q_Found == true)
      {
         const C_OSCNodeDataPoolList * const pc_DataPoolList1 = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            u32_NodeIndex,
            u32_DataPoolIndex,
            (u32_InterfaceIndex * 2U));
         const C_OSCNodeDataPoolList * const pc_DataPoolList2 = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            u32_NodeIndex,
            u32_DataPoolIndex,
            (u32_InterfaceIndex * 2U) + 1);

         if ((pc_DataPoolList1 != NULL) && (pc_DataPoolList2 != NULL))
         {
            const uint32 u32_SignalSize =
               static_cast<uint32>(pc_DataPoolList1->c_Elements.size() + pc_DataPoolList2->c_Elements.size());

            if (u32_SignalSize > 0U)
            {
               c_Content += QString(C_GtGetText::h_GetText("\nSignals: %1")).arg(u32_SignalSize);
            }
            else
            {
               c_Content += C_GtGetText::h_GetText("\nNo Signals");
            }
         }
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("\nNo Messages\nNo Signals");
      }

      this->mpc_CheckBox->SetToolTipInformation(pc_Node->c_Properties.c_Name.c_str(), c_Content);
   }
}

//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_NodeToggled(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                          const bool oq_Checked)
{
   this->m_AdaptIcon(oq_Checked);

   Q_EMIT this->SigNodeToggled(ou32_NodeIndex, ou32_InterfaceIndex, oq_Checked);
}

//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_ImportClicked(void)
{
   uint32 u32_NodeIndex;
   uint32 u32_InterfaceIndex;

   this->mpc_CheckBox->GetIndexes(u32_NodeIndex, u32_InterfaceIndex);

   Q_EMIT this->SigComImport(u32_NodeIndex, u32_InterfaceIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the icon of the check box

   \param[in]     oq_Checked      Flag if check box is checked

   \created     07.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorCheckBoxItemWidget::m_AdaptIcon(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_CheckBox->setIcon(this->mc_IconActive);
   }
   else
   {
      this->mpc_CheckBox->setIcon(this->mc_IconInactive);
   }
}

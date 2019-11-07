//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a node of a DBC file import (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <vector>

#include "stwerrors.h"
#include "CSCLString.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_CieDBCImportNodeSelectWidget.h"
#include "ui_C_CieDBCImportNodeSelectWidget.h"
#include "C_PuiProject.h"
#include "constants.h"
#include "C_OSCLoggingHandler.h"
#include "C_CieConverter.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_scl;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] orc_Parent            Reference to parent
   \param[in]     orc_CIECommDefinition dbc comm definition to read node information of node
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDBCImportNodeSelectWidget::C_CieDBCImportNodeSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                               C_CieConverter::C_CIECommDefinition & orc_CIECommDefinition)
   :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CieDBCImportNodeSelectWidget),
   mrc_ParentDialog(orc_Parent),
   mrc_CIECommDefinition(orc_CIECommDefinition),
   ms32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set standard configurations
   InitStaticNames();   // for language support (at the moment only US)
   m_FillUpComboBox();  // get node elements
   m_DisplayNodeInfo(); // display information of first node

   // hide close button of widget window
   setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);

   //Connects
   connect(this->mpc_Ui->pc_PushButtonOK, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_CieDBCImportNodeSelectWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_CieDBCImportNodeSelectWidget::m_CancelClicked);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxDBCNode,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CieDBCImportNodeSelectWidget::m_DisplayNodeInfo);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDBCImportNodeSelectWidget::~C_CieDBCImportNodeSelectWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDBCImportNodeSelectWidget::InitStaticNames(void)
{
   // window title
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("DBC File Import"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Node selection"));

   // labels
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Select DBC node"));
   this->mpc_Ui->pc_LabelNode->setText(C_GtGetText::h_GetText("Node:"));
   this->mpc_Ui->pc_LabelMessages->setText(C_GtGetText::h_GetText("")); // clear
   this->mpc_Ui->pc_LabelSignals->setText(C_GtGetText::h_GetText(""));  // clear

   // buttons
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PushButtonOK->setText(C_GtGetText::h_GetText("Continue"));
}

//-------------------------------------------------------------------------------------------f---------------------------
/*! \brief   Get user selected node.
 *
 * \return index of selected node
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CieDBCImportNodeSelectWidget::GetNodeSelection(void) const
{
   return ms32_NodeIndex; // selected node of pc_ComboBoxDBCNode
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Shows TX/RX messages and signals of a node.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDBCImportNodeSelectWidget::m_DisplayNodeInfo(void)
{
   this->ms32_NodeIndex = this->mpc_Ui->pc_ComboBoxDBCNode->currentIndex();

   // get messages and signals of node
   if (ms32_NodeIndex > -1)
   {
      const C_CieConverter::C_CIENode c_CurrentNode = this->mrc_CIECommDefinition.c_Nodes[ms32_NodeIndex];
      C_SCLString c_NumMessagesTx = C_SCLString(c_CurrentNode.c_TxMessages.size());
      C_SCLString c_NumMessagesRx = C_SCLString(c_CurrentNode.c_RxMessages.size());

      // sum up all signals of TX and RX messages
      std::vector<C_CieConverter::C_CIENodeMessage> c_TxMessages = c_CurrentNode.c_TxMessages;
      std::vector<C_CieConverter::C_CIENodeMessage> c_RxMessages = c_CurrentNode.c_RxMessages;
      sintn sn_NumOfSignals = 0;
      std::vector<C_CieConverter::C_CIENodeMessage>::iterator c_Iter;
      // signals of TX messages
      for (c_Iter = c_TxMessages.begin(); c_Iter != c_TxMessages.end(); ++c_Iter)
      {
         sn_NumOfSignals += static_cast<sintn>(c_Iter->c_CanMessage.c_Signals.size());
      }
      // signals of RX messages
      for (c_Iter = c_RxMessages.begin(); c_Iter != c_RxMessages.end(); ++c_Iter)
      {
         sn_NumOfSignals += static_cast<sintn>(c_Iter->c_CanMessage.c_Signals.size());
      }

      C_SCLString c_NumOfSignals(sn_NumOfSignals);

      // update info
      this->mpc_Ui->pc_LabelMessages->setText(C_GtGetText::h_GetText("Messages: ") + QString(c_NumMessagesTx.c_str()) +
                                              " Tx, " + QString(c_NumMessagesRx.c_str()) + " Rx");
      this->mpc_Ui->pc_LabelSignals->setText(C_GtGetText::h_GetText("Signals: ") + QString(c_NumOfSignals.c_str()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   User clicked on ok button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDBCImportNodeSelectWidget::m_OkClicked(void) const
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   User clicked on cancel button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDBCImportNodeSelectWidget::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize ComboBox for node selection.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDBCImportNodeSelectWidget::m_FillUpComboBox(void)
{
   // cleanup
   this->mpc_Ui->pc_ComboBoxDBCNode->clear();

   // fill up
   vector<C_CieConverter::C_CIENode>::const_iterator c_Iter;
   for (c_Iter = this->mrc_CIECommDefinition.c_Nodes.begin();
        c_Iter != this->mrc_CIECommDefinition.c_Nodes.end();
        ++c_Iter)
   {
      const C_SCLString c_NodeName = c_Iter->c_Properties.c_Name;
      this->mpc_Ui->pc_ComboBoxDBCNode->addItem(QString(c_NodeName.c_str()));
   }
}

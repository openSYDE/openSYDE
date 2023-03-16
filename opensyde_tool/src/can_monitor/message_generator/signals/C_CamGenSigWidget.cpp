//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Top widget for message generator signal edit (implementation)

   Top widget for message generator signal edit

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_CamDbHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamGenSigUtil.hpp"
#include "C_CamGenSigWidget.hpp"
#include "ui_C_CamGenSigWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenSigWidget::C_CamGenSigWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamGenSigWidget),
   mu32_NumSelectedItems(0UL),
   mu32_Row(0UL)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   //Initially: no selection
   this->UpdateSelection(0UL, 0UL);

   //Style
   this->SetBackgroundColor(27);
   this->mpc_Ui->pc_LabelHeading->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelHeading->SetFontPixel(14, true, false);
   this->mpc_Ui->pc_LabelHeadingDynamic->SetForegroundColor(0);
   this->mpc_Ui->pc_LabelHeadingDynamic->SetFontPixel(14, true, false); // also used in m_SetDynamicNameElided()
   this->mpc_Ui->pc_LabelNoDatabase->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNoDatabase->SetFontPixel(12, false, false);

   connect(this->mpc_Ui->pc_TableView, &C_CamGenSigTableView::SigUpdateMessageData, this,
           &C_CamGenSigWidget::SigUpdateMessageData);
   connect(this->mpc_Ui->pc_TableView, &C_CamGenSigTableView::SigTriggerModelUpdateCyclicMessage, this,
           &C_CamGenSigWidget::SigTriggerModelUpdateCyclicMessage);
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration, this,
           &C_CamGenSigWidget::m_Reset);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenSigWidget::~C_CamGenSigWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Signals of Message:"));
   this->mpc_Ui->pc_LabelNoDatabase->setText(C_GtGetText::h_GetText("Database loading..."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_TableView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::LoadUserSettings(void)
{
   this->mpc_Ui->pc_TableView->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signal reload on external trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::TriggerSignalReload(void)
{
   this->mpc_Ui->pc_TableView->TriggerSignalReload();
   //Restore last known selection -> also handles check if database is still available
   this->UpdateSelection(this->mu32_NumSelectedItems, this->mu32_Row);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the message DLC

   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::UpdateMessageDlc(const uint32_t ou32_MessageIndex) const
{
   this->mpc_Ui->pc_TableView->UpdateMessageDlc(ou32_MessageIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update current selected message

   \param[in]  ou32_NumSelectedItems   Number of selected items
   \param[in]  ou32_Row                Selected row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::UpdateSelection(const uint32_t ou32_NumSelectedItems, const uint32_t ou32_Row)
{
   QString c_Name = "-";

   this->mu32_NumSelectedItems = ou32_NumSelectedItems;
   this->mu32_Row = ou32_Row;
   if (ou32_NumSelectedItems == 0UL)
   {
      this->mpc_Ui->pc_GroupBoxNoSignals->setVisible(true);
      this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }
   else if (ou32_NumSelectedItems > 1UL)
   {
      this->mpc_Ui->pc_GroupBoxNoSignals->setVisible(true);
      this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }
   else
   {
      //Set the index first (should locate the message in the database, if not done already)
      this->mpc_Ui->pc_TableView->SetMessage(ou32_Row);
      //Afterwards handle message specific sections
      if (ou32_Row < C_CamProHandler::h_GetInstance()->GetMessages().size())
      {
         const C_CamProMessageData & rc_Message = C_CamProHandler::h_GetInstance()->GetMessages()[ou32_Row];
         c_Name = C_CamProHandler::h_GetCompleteMessageName(rc_Message);
         //Either DBC or OSY message should return a valid message
         if (rc_Message.c_DataBaseFilePath.IsEmpty() == false)
         {
            if (((C_CamDbHandler::h_GetInstance()->GetOscMessage(rc_Message.c_DataBaseFilePath.c_str(),
                                                                 rc_Message.c_Name.c_str(),
                                                                 rc_Message.q_ContainsValidHash,
                                                                 rc_Message.u32_Hash) != NULL)) ||
                (C_CamDbHandler::h_GetInstance()->GetDbcMessage(rc_Message.c_DataBaseFilePath.c_str(),
                                                                rc_Message.c_Name.c_str(),
                                                                rc_Message.q_ContainsValidHash,
                                                                rc_Message.u32_Hash) != NULL))
            {
               //Case: database and message found
               this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(false);
               this->mpc_Ui->pc_TableView->setVisible(true);
            }
            else
            {
               //Case: database and no message found
               this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(true);
               this->mpc_Ui->pc_TableView->setVisible(false);
            }
         }
         else
         {
            //Case: no database and message found
            this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(false);
            this->mpc_Ui->pc_TableView->setVisible(true);
         }
      }
      else
      {
         //Case: no database and no message found
         c_Name = C_GtGetText::h_GetText("Unknown");
         this->mpc_Ui->pc_GroupBoxNoDatabase->setVisible(false);
         this->mpc_Ui->pc_TableView->setVisible(true);
      }
      this->mpc_Ui->pc_GroupBoxNoSignals->setVisible(false);
   }

   // update label
   this->mpc_Ui->pc_LabelHeadingDynamic->SetCompleteText(c_Name);
   this->mpc_Ui->pc_LabelHeadingDynamic->SetToolTipInformation("", c_Name);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigWidget::m_Reset(void)
{
   this->UpdateSelection(0UL, 0UL);
}

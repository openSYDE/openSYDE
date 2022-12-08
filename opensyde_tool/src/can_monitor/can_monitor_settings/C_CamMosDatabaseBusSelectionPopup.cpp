//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus selection for database type openSYDE system definition. (implementation)

   In openSYDE system definition there might be several buses defined. Show information about all buses and
   let the user select one.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QComboBox>

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_CamProHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_CamMosDatabaseBusSelectionPopup.hpp"
#include "ui_C_CamMosDatabaseBusSelectionPopup.h"
#include "C_Uti.hpp"
#include "C_CamUti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]    orc_Parent        Reference to parent
   \param[in]        orc_Busses        Possible buses found in openSYDE system definition
   \param[in]        orc_Database      Database for which the bus needs to get selected
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseBusSelectionPopup::C_CamMosDatabaseBusSelectionPopup(
   const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses,
   const stw::opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CamMosDatabaseBusSelectionPopup),
   mrc_ParentDialog(orc_Parent),
   mc_Database(orc_Database)
{
   this->mpc_Ui->setupUi(this);

   // initialize static names
   this->m_InitStaticNames();

   // initialize GUI
   this->m_InitStaticGuiElements();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // initialize data
   this->mc_Busses = orc_Busses;
   this->m_InitComboBox();

   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->mpc_Ui->pc_LabelDatabaseValue->setText(C_Uti::h_MinimizePath(orc_Database.c_Name,
                                                                      mc_STYLE_GUIDE_FONT_REGULAR_12, 750, 4));

   this->mpc_Ui->pc_LabelDatabaseValue->SetToolTipInformation("", C_CamUti::h_GetAbsPathFromProj(orc_Database.c_Name));

   // after combo box initialization
   int32_t s32_IndexLastKnownBus = orc_Database.s32_BusIndex;

   if (s32_IndexLastKnownBus == -1)
   {
      s32_IndexLastKnownBus = 0;
   }

   // convert bus index to combo box index (combo box only shows CAN buses)
   s32_IndexLastKnownBus = m_BusIndexToComboboxIndex(s32_IndexLastKnownBus);

   // catch invalid indices and use first combo box entry as default
   if ((s32_IndexLastKnownBus == -1) || (s32_IndexLastKnownBus > this->mpc_Ui->pc_CbxBus->count()))
   {
      s32_IndexLastKnownBus = 0;
   }

   this->mpc_Ui->pc_CbxBus->setCurrentIndex(s32_IndexLastKnownBus);

   // manually trigger UI features adaption
   this->m_OnComboBoxChanged(s32_IndexLastKnownBus);

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_CamMosDatabaseBusSelectionPopup::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_CamMosDatabaseBusSelectionPopup::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseBusSelectionPopup::~C_CamMosDatabaseBusSelectionPopup(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get index of selected bus.

   \return
   selected bus index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_CamMosDatabaseBusSelectionPopup::GetSelectedBus() const
{
   return this->m_ComboboxIndexToBusIndex(this->mpc_Ui->pc_CbxBus->currentIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_ParentDialog.accept();
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
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("openSYDE Database Import"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Bus Selection"));
   this->mpc_Ui->pc_LabelSelectBus->setText(C_GtGetText::h_GetText("Select Bus"));
   this->mpc_Ui->pc_LabelDatabase->setText(C_GtGetText::h_GetText("Database:"));
   this->mpc_Ui->pc_LabelDatabase->SetToolTipInformation(C_GtGetText::h_GetText("Database"),
                                                         C_GtGetText::h_GetText("Path of database file."));
   this->mpc_Ui->pc_LabelBus->setText(C_GtGetText::h_GetText("Bus:"));
   this->mpc_Ui->pc_LabelBus->SetToolTipInformation(C_GtGetText::h_GetText("Bus"),
                                                    C_GtGetText::h_GetText(
                                                       "Bus information from openSYDE system definition."));
   this->mpc_Ui->pc_LabelBusId->setText(C_GtGetText::h_GetText("Bus ID:"));
   this->mpc_Ui->pc_LabelBitrate->setText(C_GtGetText::h_GetText("Bitrate:"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment:"));

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize colors, fonts, ...
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_InitStaticGuiElements() const
{
   // label "Select Bus"
   this->mpc_Ui->pc_LabelSelectBus->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelSelectBus->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSelectBus->SetFontPixel(14);

   // label "Database"
   this->mpc_Ui->pc_LabelDatabase->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelDatabase->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelDatabase->SetFontPixel(12);

   // label database value
   this->mpc_Ui->pc_LabelDatabaseValue->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelDatabaseValue->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelDatabaseValue->SetFontPixel(12);

   // label "Bus"
   this->mpc_Ui->pc_LabelBus->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelBus->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBus->SetFontPixel(12);

   // label "Bus ID:"
   this->mpc_Ui->pc_LabelBusId->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelBusId->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBusId->SetFontPixel(12);

   // label bus ID value
   this->mpc_Ui->pc_LabelBusIdValue->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelBusIdValue->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBusIdValue->SetFontPixel(12);

   // label "Bitrate:"
   this->mpc_Ui->pc_LabelBitrate->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelBitrate->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBitrate->SetFontPixel(12);

   // label bitrate value
   this->mpc_Ui->pc_LabelBitrateValue->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelBitrateValue->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelBitrateValue->SetFontPixel(12);

   // label "Comment:"
   this->mpc_Ui->pc_LabelComment->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelComment->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelComment->SetFontPixel(12);

   // label comment value
   this->mpc_Ui->pc_LabelCommentValue->SetForegroundColor(6);
   this->mpc_Ui->pc_LabelCommentValue->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelCommentValue->SetFontPixel(12);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize GUI from bus data.

   \param[in]   orc_Bus    Bus data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_InitComboBox(void) const
{
   // clean up
   this->mpc_Ui->pc_CbxBus->clear();

   // fill up
   for (std::vector<C_OscSystemBus>::const_iterator c_Iter = mc_Busses.begin(); c_Iter != mc_Busses.end(); ++c_Iter)
   {
      // only show can buses
      if (c_Iter->e_Type == C_OscSystemBus::eCAN)
      {
         this->mpc_Ui->pc_CbxBus->addItem(static_cast<QString>(c_Iter->c_Name.c_str()));
      }
   }

   connect(this->mpc_Ui->pc_CbxBus, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_CamMosDatabaseBusSelectionPopup::m_OnComboBoxChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of combo box index changed.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_OnComboBoxChanged(const int32_t os32_Index) const
{
   const int32_t s32_BusIndex = this->m_ComboboxIndexToBusIndex(os32_Index);

   if ((s32_BusIndex >= 0) && (s32_BusIndex < static_cast<int32_t>(this->mc_Busses.size())))
   {
      const C_OscSystemBus & rc_Bus = this->mc_Busses[s32_BusIndex];
      this->mpc_Ui->pc_LabelBusIdValue->setText(QString::number(rc_Bus.u8_BusId));
      this->mpc_Ui->pc_LabelBitrateValue->setText(QString::number(rc_Bus.u64_BitRate / 1000) + " kBit/s");
      this->mpc_Ui->pc_LabelCommentValue->setText(rc_Bus.c_Comment.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of cancel button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseBusSelectionPopup::m_OkClicked(void)
{
   QStringList c_Messages;

   C_CamProHandler::h_GetInstance()->GetAllMessagesFromDatabase(C_CamUti::h_GetAbsPathFromProj(
                                                                   this->mc_Database.c_Name),
                                                                &c_Messages, NULL);
   if ((c_Messages.size() > 0) && (this->mpc_Ui->pc_CbxBus->currentIndex() != this->mc_Database.s32_BusIndex))
   {
      QString c_Details;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("openSYDE database re-import"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really apply the changes? All message generator messages from this database will be deleted."));
      //Add details
      c_Details.append(C_GtGetText::h_GetText(
                          "Following messages will be deleted:\n"));
      //Add all messages
      for (QStringList::const_iterator c_It = c_Messages.begin(); c_It != c_Messages.end(); ++c_It)
      {
         c_Details.append(*c_It);
         c_Details.append("\n");
      }
      c_Message.SetDetails(c_Details);
      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES) // return to dialog on cancel
      {
         this->mrc_ParentDialog.accept();
      }
   }
   else
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus index from combo box index.

   Combo box only shows CAN buses; but for bus index we must also consider Ethernet buses.

   \param[in] Combo box Index

   \return
   bus index (-1 if something went wrong)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMosDatabaseBusSelectionPopup::m_ComboboxIndexToBusIndex(const int32_t os32_ComboboxIndex) const
{
   int32_t s32_Return = -1;
   int32_t s32_PosCanOnly = 0;

   // iterate over all buses and count CAN buses
   for (int32_t s32_PosAll = 0; s32_PosAll < static_cast<int32_t>(this->mc_Busses.size()); s32_PosAll++)
   {
      if (this->mc_Busses[s32_PosAll].e_Type == C_OscSystemBus::eCAN)
      {
         if (s32_PosCanOnly == os32_ComboboxIndex)
         {
            s32_Return = s32_PosAll;
            break;
         }
         s32_PosCanOnly += 1;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get combo box index from bus index.

   Combo box only shows CAN buses; but for bus index we must also consider Ethernet buses.
   Assumes that bus of given index is of type CAN!

   \param[in] Bus Index

   \return
   Combo box index (-1 if something went wrong)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamMosDatabaseBusSelectionPopup::m_BusIndexToComboboxIndex(const int32_t os32_BusIndex) const
{
   int32_t s32_Return = -1;

   if ((os32_BusIndex > -1) && (os32_BusIndex < static_cast<int32_t>(this->mc_Busses.size())))
   {
      int32_t s32_EthNumber = 0;
      // count Ethernet buses with index less than given index
      for (int32_t s32_PosAll = 0; s32_PosAll < os32_BusIndex; s32_PosAll++)
      {
         if (this->mc_Busses[s32_PosAll].e_Type == C_OscSystemBus::eETHERNET)
         {
            s32_EthNumber += 1;
         }
      }

      // subtract number of Ethernet buses with index less than given index
      s32_Return = os32_BusIndex - s32_EthNumber;
   }

   return s32_Return;
}

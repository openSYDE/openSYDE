//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for progress log entry (implementation)

   Widget for progress log entry

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpProgressLogEntry.hpp"
#include "ui_C_SyvUpProgressLogEntry.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_NodeIndex Node index
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpProgressLogEntry::C_SyvUpProgressLogEntry(const uint32_t ou32_NodeIndex, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpProgressLogEntry),
   mu32_NodeIndex(ou32_NodeIndex)
{
   mpc_Ui->setupUi(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpProgressLogEntry::~C_SyvUpProgressLogEntry(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set description

   \param[in] orc_Value New description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLogEntry::SetEntryDescription(const QString & orc_Value) const
{
   this->mpc_Ui->pc_LabelDescription->setText(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set status

   \param[in] orc_Value New status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpProgressLogEntry::SetEntryStatus(const QString & orc_Value) const
{
   this->mpc_Ui->pc_LabelStatus->setText(orc_Value);
   //Remove ...
   if (orc_Value.compare("") != 0)
   {
      QString c_Text = this->mpc_Ui->pc_LabelDescription->text();
      c_Text.replace("...", "");
      this->mpc_Ui->pc_LabelDescription->setText(c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if index matches

   \param[in] ou32_NodeIndex Node index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpProgressLogEntry::DoesIndexMatch(const uint32_t ou32_NodeIndex) const
{
   return this->mu32_NodeIndex == ou32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node index

   \return
   Current node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpProgressLogEntry::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

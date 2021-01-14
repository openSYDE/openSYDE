//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for list header (implementation)

   Line edit for list header.
   Sends signal on focus in.
   Intelligent handling of list count in edit vs. display mode.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeLeListHeader.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;

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
C_OgeLeListHeader::C_OgeLeListHeader(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   mq_JustGotFocus(false),
   mu32_Counter(0),
   mc_Name("")
{
   connect(this, &C_OgeLeListHeader::editingFinished, this, &C_OgeLeListHeader::m_OnEditingFinished);
   connect(this, &C_OgeLeListHeader::textEdited, this, &C_OgeLeListHeader::m_OnTextEdited);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set counter to display after text when focus is out. Example: "MyList (7)".

   \param[in]  ou32_Counter   Counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::SetCounter(const stw_types::uint32 ou32_Counter)
{
   this->mu32_Counter = ou32_Counter;
   this->m_UpdateText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set name i.e. text of line edit without the counter.

   \param[in]  orc_Name    Name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::SetName(const QString & orc_Name)
{
   this->mc_Name = orc_Name;
   this->m_UpdateText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get name, i.e. text without counter

   \return
   current name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeListHeader::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Signal and remember; remove counter from text

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::focusInEvent(QFocusEvent * const opc_Event)
{
   QLineEdit::focusInEvent(opc_Event);

   // remove counter while editing
   this->m_UpdateText();

   // handle signal
   Q_EMIT (this->SigFocus());

   this->mq_JustGotFocus = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Select all if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::mousePressEvent(QMouseEvent * const opc_Event)
{
   QLineEdit::mousePressEvent(opc_Event);

   // fix issue: when coming from spinbox in header widget, the mouse press event did not set focus to this line edit
   if (this->hasFocus() == false)
   {
      this->setFocus();
   }

   if (this->mq_JustGotFocus == true)
   {
      this->mq_JustGotFocus = false;
      this->selectAll(); // calling this in focusInEvent is "too early"
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set text depending on current focus state.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::m_UpdateText(void)
{
   if (this->hasFocus() == false)
   {
      this->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT + 10); // " (1234567)" should definitely be enough
      this->setText(static_cast<QString>("%1 (%2)").arg(this->mc_Name).arg(this->mu32_Counter));
   }
   else
   {
      this->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);
      this->setText(this->mc_Name);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of editing finished signal: set new name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::m_OnEditingFinished(void)
{
   this->mc_Name = this->text();
   this->m_UpdateText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of text edited signal: set new name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeListHeader::m_OnTextEdited()
{
   this->mc_Name = this->text();
}

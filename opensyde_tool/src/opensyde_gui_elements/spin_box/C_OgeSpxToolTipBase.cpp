//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Default spin box with tool tip and custom context menu (implementation)

   Default spin box with tool tip and custom context menu

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QLineEdit>
#include "C_GtGetText.h"
#include "C_OgeSpxToolTipBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxToolTipBase::C_OgeSpxToolTipBase(QWidget * const opc_Parent) :
   QSpinBox(opc_Parent),
   C_OgeSpxAllBase(),
   mpc_ContextMenu(NULL),
   mq_ShowSpecialMin(false),
   msn_SpecialMinValue(0),
   mq_ShowSpecialMax(false),
   msn_SpecialMaxValue(0)
{
   //This function does indeed call virtual functions so do not call this one in the base class
   ActivateDefaultToolTip();
   m_InitContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::event(QEvent * const opc_Event)
{
   return this->m_HandleEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get enabled status

   \return
   Enabled status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::m_IsEnabled(void) const
{
   return this->isEnabled();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimum as string

   \return
   Minimum as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMinimumRawString(void) const
{
   return this->m_ConvertNumToString(this->minimum());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get maximum as string

   \return
   Maximum as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMaximumRawString(void) const
{
   return this->m_ConvertNumToString(this->maximum());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set mouse tracking status

   \param[in]  oq_Active   New mouse tracking status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_SetMouseTracking(const bool oq_Active)
{
   this->setMouseTracking(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call base event slot

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::m_CallBaseEvent(QEvent * const opc_Event)
{
   return QSpinBox::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten text from value slot

             Here: Convert hex number to string without "0x"

   Here: Handle specific text from value of spin box
         Use that method when a prefix e. g. "0x" is already set
         and you only need the hex number in string format

   \param[in,out]  osn_Value  Value identification and information

   \return
   A string of numbers and letters
   Here: Converted hex number as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::textFromValue(const sintn osn_Value) const
{
   QString c_Retval;

   if (this->displayIntegerBase() == 16)
   {
      c_Retval = QString::number(osn_Value, 16).toUpper();
   }
   else
   {
      c_Retval = QSpinBox::textFromValue(osn_Value);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set minimum value (simple wrapper with tool tip update)

   \param[in]  osn_Value         New minimum value
   \param[in]  oq_ShowSpecial    Show special
   \param[in]  osn_SpecialValue  Special value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::SetMinimumCustom(const stw_types::sintn osn_Value, const bool oq_ShowSpecial,
                                           const sintn osn_SpecialValue)
{
   this->setMinimum(osn_Value);
   this->mq_ShowSpecialMin = oq_ShowSpecial;
   this->msn_SpecialMinValue = osn_SpecialValue;
   this->ActivateDefaultToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set maximum value (simple wrapper with tool tip update)

   \param[in]  osn_Value         New maximum value
   \param[in]  oq_ShowSpecial    Show special
   \param[in]  osn_SpecialValue  Special value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::SetMaximumCustom(const stw_types::sintn osn_Value, const bool oq_ShowSpecial,
                                           const sintn osn_SpecialValue)
{
   this->setMaximum(osn_Value);
   this->mq_ShowSpecialMax = oq_ShowSpecial;
   this->msn_SpecialMaxValue = osn_SpecialValue;
   this->ActivateDefaultToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeSpxToolTipBase::customContextMenuRequested, this,
           &C_OgeSpxToolTipBase::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions (differentiate between normal and read-only text edits)
   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Undo"),
                                       this,
                                       &C_OgeSpxToolTipBase::m_Undo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Z));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Redo"),
                                       this,
                                       &C_OgeSpxToolTipBase::m_Redo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Y));

      this->mpc_ContextMenu->addSeparator();

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                       this,
                                       &C_OgeSpxToolTipBase::m_Cut,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   }

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeSpxToolTipBase::m_Copy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                       this,
                                       &C_OgeSpxToolTipBase::m_Paste,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                       this,
                                       &C_OgeSpxToolTipBase::m_Delete,
                                       static_cast<sintn>(Qt::Key_Delete));
   }

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeSpxToolTipBase::selectAll,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Up"),
                                    this,
                                    &C_OgeSpxToolTipBase::stepUp);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Down"),
                                    this,
                                    &C_OgeSpxToolTipBase::stepDown);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(); // setup the custom menu here to have real "is-read-only" information
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit undo.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Undo() const
{
   this->lineEdit()->undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit redo.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Redo() const
{
   this->lineEdit()->redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit cut.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Cut() const
{
   this->lineEdit()->cut();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit copy.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Copy() const
{
   this->lineEdit()->copy();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit paste.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Paste() const
{
   this->lineEdit()->paste();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit delete.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Delete() const
{
   this->lineEdit()->del();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert num to string

   \param[in]  osn_Value   Value

   \return
   Converted num as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_ConvertNumToString(const sintn osn_Value) const
{
   QString c_Retval;

   if (this->displayIntegerBase() == 16)
   {
      c_Retval = "0x" + QString::number(osn_Value, 16).toUpper();
   }
   else
   {
      c_Retval = QString::number(osn_Value);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get minimum string

   \return
   Minimum string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMinimumString() const
{
   QString c_Retval;

   if (this->mq_ShowSpecialMin)
   {
      c_Retval = this->m_ConvertNumToString(this->msn_SpecialMinValue);
   }
   else
   {
      c_Retval = this->m_ConvertNumToString(this->minimum());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get maximum string

   \return
   Maximum string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMaximumString() const
{
   QString c_Retval;

   if (this->mq_ShowSpecialMax)
   {
      c_Retval = this->m_ConvertNumToString(this->msn_SpecialMaxValue);
   }
   else
   {
      c_Retval = this->m_ConvertNumToString(this->maximum());
   }
   return c_Retval;
}

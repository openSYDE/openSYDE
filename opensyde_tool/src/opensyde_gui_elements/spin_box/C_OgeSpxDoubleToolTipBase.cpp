//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Double spin box with tool tip and custom context menu (implementation)

   Double spin box with tool tip and custom context menu

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEvent>
#include <QHelpEvent>
#include <QLineEdit>
#include "C_GtGetText.hpp"
#include "C_OgeSpxDoubleToolTipBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxDoubleToolTipBase::C_OgeSpxDoubleToolTipBase(QWidget * const opc_Parent) :
   C_OgeSpxDoubleDynamicDecimalsBase(opc_Parent),
   C_OgeSpxAllBase(),
   mpc_ContextMenu(NULL)
{
   //This function does indeed call virtual functions so do not call this one in the base class
   ActivateDefaultToolTip();
   m_InitContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxDoubleToolTipBase::event(QEvent * const opc_Event)
{
   return this->m_HandleEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set minimum value (simple wrapper with tool tip update)

   \param[in] of64_Value  New minimum value
   \param[in] orc_Display String display value for minimum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::SetMinimumCustom(const float64_t of64_Value, const QString & orc_Display)
{
   this->setMinimum(of64_Value);
   this->mc_UserMinValString = orc_Display;
   this->ActivateDefaultToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set maximum value (simple wrapper with tool tip update)

   \param[in] of64_Value  New maximum value
   \param[in] orc_Display String display value for maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::SetMaximumCustom(const float64_t of64_Value, const QString & orc_Display)
{
   this->setMaximum(of64_Value);
   this->mc_UserMaxValString = orc_Display;
   this->ActivateDefaultToolTip();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get enabled status

   \return
   Enabled status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxDoubleToolTipBase::m_IsEnabled(void) const
{
   return this->isEnabled();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimum as string

   \return
   Minimum as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxDoubleToolTipBase::m_GetMinimumRawString(void) const
{
   return QString::number(this->minimum());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get maximum as string

   \return
   Maximum as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxDoubleToolTipBase::m_GetMaximumRawString(void) const
{
   return QString::number(this->maximum());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set mouse tracking status

   \param[in] oq_Active New mouse tracking status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_SetMouseTracking(const bool oq_Active)
{
   this->setMouseTracking(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call base event slot

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeSpxDoubleToolTipBase::m_CallBaseEvent(QEvent * const opc_Event)
{
   return C_OgeSpxDoubleDynamicDecimalsBase::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeSpxDoubleToolTipBase::customContextMenuRequested, this,
           &C_OgeSpxDoubleToolTipBase::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions (differentiate between normal and read-only text edits)
   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Undo"),
                                       this,
                                       &C_OgeSpxDoubleToolTipBase::m_Undo,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_Z));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Redo"),
                                       this,
                                       &C_OgeSpxDoubleToolTipBase::m_Redo,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_Y));

      this->mpc_ContextMenu->addSeparator();

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                       this,
                                       &C_OgeSpxDoubleToolTipBase::m_Cut,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_X));
   }

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeSpxDoubleToolTipBase::m_Copy,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_C));

   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                       this,
                                       &C_OgeSpxDoubleToolTipBase::m_Paste,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_V));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                       this,
                                       &C_OgeSpxDoubleToolTipBase::m_Delete,
                                       static_cast<int32_t>(Qt::Key_Delete));
   }

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeSpxDoubleToolTipBase::selectAll,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Up"),
                                    this,
                                    &C_OgeSpxDoubleToolTipBase::stepUp);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Down"),
                                    this,
                                    &C_OgeSpxDoubleToolTipBase::stepDown);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
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
void C_OgeSpxDoubleToolTipBase::m_Undo() const
{
   this->lineEdit()->undo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit redo.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_Redo() const
{
   this->lineEdit()->redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit cut.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_Cut() const
{
   this->lineEdit()->cut();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit copy.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_Copy() const
{
   this->lineEdit()->copy();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit paste.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_Paste() const
{
   this->lineEdit()->paste();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call line edit delete.

   For custom context menu we need access to the line edit
   functions of the spin box.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxDoubleToolTipBase::m_Delete() const
{
   this->lineEdit()->del();
}

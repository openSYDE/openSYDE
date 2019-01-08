//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Default spin box with tool tip and custom context menu (implementation)

   Default spin box with tool tip and custom context menu

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QLineEdit>
#include "C_GtGetText.h"
#include "C_OgeSpxToolTipBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     26.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxToolTipBase::C_OgeSpxToolTipBase(QWidget * const opc_Parent) :
   QSpinBox(opc_Parent),
   C_OgeSpxAllBase(),
   mpc_ContextMenu(NULL)
{
   //This function does indeed call virtual functions so do not call this one in the base class
   ActivateDefaultToolTip();
   m_InitContextMenu();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     26.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::event(QEvent * const opc_Event)
{
   return this->m_HandleEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get enabled status

   \return
   Enabled status

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::m_IsEnabled(void) const
{
   return this->isEnabled();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get minimum as string

   \return
   Minimum as string

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMinimum(void) const
{
   QString c_Retval;

   if (this->displayIntegerBase() == 16)
   {
      c_Retval = "0x" + QString::number(this->minimum(), 16);
   }
   else
   {
      c_Retval = QString::number(this->minimum());
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get maximum as string

   \return
   Maximum as string

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_OgeSpxToolTipBase::m_GetMaximum(void) const
{
   QString c_Retval;

   if (this->displayIntegerBase() == 16)
   {
      c_Retval = "0x" + QString::number(this->maximum(), 16);
   }
   else
   {
      c_Retval = QString::number(this->maximum());
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set mouse tracking status

   \param[in] oq_Active New mouse tracking status

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_SetMouseTracking(const bool oq_Active)
{
   this->setMouseTracking(oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call base event slot

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeSpxToolTipBase::m_CallBaseEvent(QEvent * const opc_Event)
{
   return QSpinBox::event(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set minimum value (simple wrapper with tool tip update)

   \param[in] osn_Value New minimum value

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::SetMinimumCustom(const stw_types::sintn osn_Value)
{
   this->setMinimum(osn_Value);
   this->ActivateDefaultToolTip();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set maximum value (simple wrapper with tool tip update)

   \param[in] osn_Value New maximum value

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::SetMaximumCustom(const stw_types::sintn osn_Value)
{
   this->setMaximum(osn_Value);
   this->ActivateDefaultToolTip();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize custom context menu functionality

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeSpxToolTipBase::customContextMenuRequested, this,
           &C_OgeSpxToolTipBase::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setup context menu entries

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(); // setup the custom menu here to have real "is-read-only" information
   QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit undo.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Undo() const
{
   this->lineEdit()->undo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit redo.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Redo() const
{
   this->lineEdit()->redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit cut.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Cut() const
{
   this->lineEdit()->cut();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit copy.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Copy() const
{
   this->lineEdit()->copy();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit paste.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Paste() const
{
   this->lineEdit()->paste();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call line edit delete.

   For custom context menu we need access to the line edit
   functions of the spin box.

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxToolTipBase::m_Delete() const
{
   this->lineEdit()->del();
}

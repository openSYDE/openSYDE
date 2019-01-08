//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Int 64 spin box with tool tip and custom context menu (implementation)

   Int 64 spin box with tool tip and custom context menu

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
#include "C_OgeSpxInt64ToolTipBase.h"

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

   \param[in,out] opc_Parent     Optional pointer to parent
   \param[in]     orq_IsUnsigned Indicator if this spin box should handle unsigned 64 bit or signed 64 bit

   \created     26.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxInt64ToolTipBase::C_OgeSpxInt64ToolTipBase(QWidget * const opc_Parent, const bool & orq_IsUnsigned) :
   C_OgeSpxInt64(opc_Parent, orq_IsUnsigned),
   C_OgeSpxAllBase(),
   mpc_ContextMenu(NULL)
{
   //This function does indeed call virtual functions so do not call this one in the base class
   ActivateDefaultToolTip();
   connect(this, &C_OgeSpxInt64ToolTipBase::SigMinMaxChanged, this,
           &C_OgeSpxInt64ToolTipBase::m_ActivateDefaultToolTip);
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
bool C_OgeSpxInt64ToolTipBase::event(QEvent * const opc_Event)
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
bool C_OgeSpxInt64ToolTipBase::m_IsEnabled(void) const
{
   return this->isEnabled();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get minimum as string

   \return
   Minimum as string

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_OgeSpxInt64ToolTipBase::m_GetMinimum(void) const
{
   const QVariant c_Tmp = this->GetMinimum();

   return c_Tmp.toString();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get maximum as string

   \return
   Maximum as string

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_OgeSpxInt64ToolTipBase::m_GetMaximum(void) const
{
   const QVariant c_Tmp = this->GetMaximum();

   return c_Tmp.toString();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set mouse tracking status

   \param[in] oq_Active New mouse tracking status

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64ToolTipBase::m_SetMouseTracking(const bool oq_Active)
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

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeSpxInt64ToolTipBase::m_CallBaseEvent(QEvent * const opc_Event)
{
   return C_OgeSpxInt64::event(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize custom context menu functionality

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64ToolTipBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeSpxInt64ToolTipBase::customContextMenuRequested, this,
           &C_OgeSpxInt64ToolTipBase::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setup context menu entries

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64ToolTipBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions (differentiate between normal and read-only text edits)
   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Undo"),
                                       this,
                                       &C_OgeSpxInt64ToolTipBase::m_Undo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Z));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Redo"),
                                       this,
                                       &C_OgeSpxInt64ToolTipBase::m_Redo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Y));

      this->mpc_ContextMenu->addSeparator();

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                       this,
                                       &C_OgeSpxInt64ToolTipBase::m_Cut,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   }

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeSpxInt64ToolTipBase::m_Copy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                       this,
                                       &C_OgeSpxInt64ToolTipBase::m_Paste,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                       this,
                                       &C_OgeSpxInt64ToolTipBase::m_Delete,
                                       static_cast<sintn>(Qt::Key_Delete));
   }

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeSpxInt64ToolTipBase::selectAll,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Up"),
                                    this,
                                    &C_OgeSpxInt64ToolTipBase::stepUp);

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Step Down"),
                                    this,
                                    &C_OgeSpxInt64ToolTipBase::stepDown);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     18.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64ToolTipBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
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
void C_OgeSpxInt64ToolTipBase::m_Undo() const
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
void C_OgeSpxInt64ToolTipBase::m_Redo() const
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
void C_OgeSpxInt64ToolTipBase::m_Cut() const
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
void C_OgeSpxInt64ToolTipBase::m_Copy() const
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
void C_OgeSpxInt64ToolTipBase::m_Paste() const
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
void C_OgeSpxInt64ToolTipBase::m_Delete() const
{
   this->lineEdit()->del();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger activate default tooltip

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxInt64ToolTipBase::m_ActivateDefaultToolTip()
{
   this->ActivateDefaultToolTip();
}

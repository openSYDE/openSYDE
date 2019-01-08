//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label with custom context menu (implementation)

   Label with custom context menu in openSYDE style.
   This is only needed for labels with text interaction flag
   Qt::TextSelectableByMouse.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.06.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>
#include <QClipboard>

#include "C_OgeLabContextMenuBase.h"
#include "stwtypes.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;

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

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeLabContextMenuBase::C_OgeLabContextMenuBase(QWidget * const opc_Parent) :
   QLabel(opc_Parent),
   mpc_ContextMenu(NULL)
{
   m_InitContextMenu();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize custom context menu functionality

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeLabContextMenuBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeLabContextMenuBase::customContextMenuRequested, this,
           &C_OgeLabContextMenuBase::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setup context menu entries

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeLabContextMenuBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeLabContextMenuBase::m_Copy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeLabContextMenuBase::m_SelectAll,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeLabContextMenuBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   // show context menu only if selectable
   if (this->textInteractionFlags().testFlag(Qt::TextSelectableByMouse) == true)
   {
      m_SetupContextMenu(); // setup the custom menu here to have real is-selectable information

      QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
      this->mpc_ContextMenu->popup(c_PosGlobal);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy the selected text to clipboard.

   QLabel has no copy action.

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeLabContextMenuBase::m_Copy() const
{
   QClipboard * const pc_Clipboard = QApplication::clipboard();

   pc_Clipboard->setText(this->selectedText());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select the hole text of the label.

   QLabel has no selectAll action.

   \created     29.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeLabContextMenuBase::m_SelectAll()
{
   this->setSelection(0, this->text().length());
}

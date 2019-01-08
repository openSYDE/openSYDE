//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Text edit with custom context menu (implementation)

   Text edit with custom context menu in openSYDE style.
   Shows no options for text editing (e.g. undo, paste, delete) when the
   text edit is in read only state.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.05.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QScrollBar>

#include "stwtypes.h"
#include "C_OgeTedContextMenuBase.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor.

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     17.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeTedContextMenuBase::C_OgeTedContextMenuBase(QWidget * const opc_Parent) :
   QTextEdit(opc_Parent),
   mpc_ContextMenu(NULL)
{
   m_InitContextMenu();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize custom context menu functionality

   \created     17.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeTedContextMenuBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeTedContextMenuBase::customContextMenuRequested, this,
           &C_OgeTedContextMenuBase::m_OnCustomContextMenuRequested);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Setup context menu entries

   \created     17.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeTedContextMenuBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions (differentiate between normal and read-only text edits)
   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Undo"),
                                       this,
                                       &C_OgeTedContextMenuBase::undo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Z));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Redo"),
                                       this,
                                       &C_OgeTedContextMenuBase::redo,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_Y));

      this->mpc_ContextMenu->addSeparator();

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                       this,
                                       &C_OgeTedContextMenuBase::cut,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   }

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeTedContextMenuBase::copy,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                       this,
                                       &C_OgeTedContextMenuBase::paste,
                                       static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                       this,
                                       &C_OgeTedContextMenuBase::m_Delete,
                                       static_cast<sintn>(Qt::Key_Delete));
   }

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeTedContextMenuBase::selectAll,
                                    static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_A));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     17.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeTedContextMenuBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(); // setup the custom menu here to have real "is-read-only" information
   QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
   c_PosGlobal = c_PosGlobal + QPoint(8, 5); // little coordination correction of popup needed

   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete selected text.

   If there is a selection, delete its content; otherwise do nothing.

   \created     17.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeTedContextMenuBase::m_Delete() const
{
   QTextCursor c_TextCursor;

   c_TextCursor = this->textCursor();
   c_TextCursor.removeSelectedText();
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit with custom context menu (implementation)

   Line edit with custom context menu in openSYDE style.
   Shows no options for text editing (e.g. undo, paste, delete) when the
   text edit is in read only state.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OgeLeContextMenuBase.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeContextMenuBase::C_OgeLeContextMenuBase(QWidget * const opc_Parent) :
   QLineEdit(opc_Parent),
   mpc_ContextMenu(NULL)
{
   m_InitContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeContextMenuBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeLeContextMenuBase::customContextMenuRequested, this,
           &C_OgeLeContextMenuBase::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeContextMenuBase::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions (differentiate between normal and read-only text edits)
   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Undo"),
                                       this,
                                       &C_OgeLeContextMenuBase::undo,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_Z));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Redo"),
                                       this,
                                       &C_OgeLeContextMenuBase::redo,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_Y));

      this->mpc_ContextMenu->addSeparator();

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                       this,
                                       &C_OgeLeContextMenuBase::cut,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_X));
   }

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeLeContextMenuBase::copy,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_C));

   if (!this->isReadOnly())
   {
      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                       this,
                                       &C_OgeLeContextMenuBase::paste,
                                       static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_V));

      this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                       this,
                                       &C_OgeLeContextMenuBase::del,
                                       static_cast<int32_t>(Qt::Key_Delete));
   }

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeLeContextMenuBase::selectAll,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeContextMenuBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(); // setup the custom menu here to have real "is-read-only" information
   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);

   this->mpc_ContextMenu->popup(c_PosGlobal);
}

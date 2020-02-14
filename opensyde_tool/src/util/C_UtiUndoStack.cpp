//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common base class for undo stacks  (implementation)

   Common base class for undo stacks

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include "C_UtiUndoStack.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_UtiUndoStack::C_UtiUndoStack(QObject * const opc_Parent) :
   QUndoStack(opc_Parent),
   mpc_UndoView(new QUndoView(this))
{
   //View
   mpc_UndoView->setWindowTitle("Command List");
   mpc_UndoView->setAttribute(Qt::WA_QuitOnClose, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_UtiUndoStack::~C_UtiUndoStack(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do command push

   \param[in,out]  opc_UndoCommand  New undo command to add to the stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::DoPush(QUndoCommand * const opc_UndoCommand)
{
   if (opc_UndoCommand != NULL)
   {
      this->m_PrepareAction();
      this->push(opc_UndoCommand);
      this->m_CleanupAction();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do redo step
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::DoRedo(void)
{
   if (this->count() > 0)
   {
      this->m_PrepareAction();
      this->redo();
      this->m_CleanupAction();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do undo step
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::DoUndo(void)
{
   if (this->count() > 0)
   {
      this->m_PrepareAction();
      this->undo();
      this->m_CleanupAction();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Toggle command display for undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::ToggleCommandDisplay(void)
{
   if (this->mpc_UndoView->isHidden())
   {
      this->mpc_UndoView->show();
   }
   else
   {
      this->mpc_UndoView->hide();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::m_PrepareAction(void)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
}

//----------------------------------------------------------------------------------------------------------------------
void C_UtiUndoStack::m_CleanupAction(void)
{
   QApplication::restoreOverrideCursor();
}

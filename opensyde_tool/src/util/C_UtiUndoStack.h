//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Common base class for undo stacks (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_UTIUNDOSTACK_H
#define C_UTIUNDOSTACK_H

/* -- Includes ------------------------------------------------------------- */
#include <QUndoStack>
#include <QUndoView>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UtiUndoStack :
   public QUndoStack
{
public:
   C_UtiUndoStack(QObject * const opc_Parent);
   ~C_UtiUndoStack(void);

   void DoPush(QUndoCommand * const opc_UndoCommand);
   void DoRedo(void);
   void DoUndo(void);
   void ToggleCommandDisplay(void);

protected:
   virtual void m_PrepareAction(void);
   virtual void m_CleanupAction(void);

private:
   QUndoView * const mpc_UndoView;
   //Avoid call
   C_UtiUndoStack(const C_UtiUndoStack &);
   C_UtiUndoStack & operator =(const C_UtiUndoStack &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

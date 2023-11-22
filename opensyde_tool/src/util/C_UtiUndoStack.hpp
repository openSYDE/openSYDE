//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common base class for undo stacks (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_UTIUNDOSTACK_HPP
#define C_UTIUNDOSTACK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QUndoStack>
#include <QUndoView>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UtiUndoStack :
   public QUndoStack
{
public:
   C_UtiUndoStack(QObject * const opc_Parent = NULL);
   ~C_UtiUndoStack(void) override;

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
   C_UtiUndoStack & operator =(const C_UtiUndoStack &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all topology undo commands (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYBASECOMMAND_H
#define C_SDMANUNOTOPOLOGYBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoBaseCommand :
   public QUndoCommand
{
public:
   C_SebUnoBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                       const QString & orc_Text, QUndoCommand * const opc_Parent = NULL);
   C_SebUnoBaseCommand(const C_SebUnoBaseCommand * const opc_Prev, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoBaseCommand(void);
   virtual void undo(void) override = 0;
   virtual void redo(void) override = 0;

protected:
   std::vector<QGraphicsItem *> m_GetSceneItems(void);
   QGraphicsItem * m_GetSceneItem(const stw_types::uint64 & oru64_ID);
   bool m_MergePossible(const C_SebUnoBaseCommand * const opc_Command) const;

   QGraphicsScene * const mpc_Scene;

   const std::vector<stw_types::uint64> GetIDs(void) const;

private:
   const std::vector<stw_types::uint64> mc_IDs;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all topology undo commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYBASECOMMAND_HPP
#define C_SDMANUNOTOPOLOGYBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoBaseCommand :
   public QUndoCommand
{
public:
   C_SebUnoBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                       const QString & orc_Text, QUndoCommand * const opc_Parent = NULL);
   C_SebUnoBaseCommand(const C_SebUnoBaseCommand * const opc_Prev, QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoBaseCommand(void) override;
   void undo(void) override = 0; //lint !e9170 //we do want to force children to implement this functions
   void redo(void) override = 0; //lint !e9170 //we do want to force children to implement this functions

protected:
   std::vector<QGraphicsItem *> m_GetSceneItems(void) const;
   QGraphicsItem * m_GetSceneItem(const uint64_t & oru64_Id) const;
   bool m_MergePossible(const C_SebUnoBaseCommand * const opc_Command) const;

   QGraphicsScene * const mpc_Scene;

   const std::vector<uint64_t> m_GetIds(void) const;

private:
   const std::vector<uint64_t> mc_Ids;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

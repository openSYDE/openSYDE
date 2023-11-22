//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for add and remove bend point actions (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBENDPOINTBASECOMMAND_HPP
#define C_SEBUNOTOPBENDPOINTBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoBendPointBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoBendPointBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                const QString & orc_Text, const QPointF & orc_ScenePosition,
                                QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoBendPointBaseCommand(void) override;
   void undo(void) override = 0;
   void redo(void) override = 0;

protected:
   void m_DeleteBend(void);
   void m_AddBend(void);

private:
   const QPointF mc_ScenePosition;
   int32_t ms32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

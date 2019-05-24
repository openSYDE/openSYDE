//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for add and remove bend point actions (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBENDPOINTBASECOMMAND_H
#define C_SEBUNOTOPBENDPOINTBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoBendPointBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoBendPointBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const QString & orc_Text, const QPointF & orc_ScenePosition,
                                QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoBendPointBaseCommand(void);
   virtual void undo(void) override = 0;
   virtual void redo(void) override = 0;

protected:
   void m_DeleteBend(void);
   void m_AddBend(void);

private:
   const QPointF mc_ScenePosition;
   stw_types::sint32 ms32_Index;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

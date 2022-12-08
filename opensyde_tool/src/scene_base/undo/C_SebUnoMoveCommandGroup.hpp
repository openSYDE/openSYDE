//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Command for grouping move commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOMOVECOMMANDGROUP_H
#define C_SEBUNOMOVECOMMANDGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoCommand>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoMoveCommandGroup :
   public QUndoCommand
{
public:
   C_SebUnoMoveCommandGroup(QUndoCommand * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list base command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTBASECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>
#include <QUndoCommand>
#include "C_SdNdeDpListsTreeWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListBaseCommand :
   public QUndoCommand
{
public:
   C_SdNdeUnoDataPoolListBaseCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                     const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_DataPoolIndex;
   stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const mpc_DataPoolListsTreeWidget;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

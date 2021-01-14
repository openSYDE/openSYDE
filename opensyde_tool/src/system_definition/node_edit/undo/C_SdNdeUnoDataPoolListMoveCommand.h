//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTMOVECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListMoveCommand :
   public C_SdNdeUnoDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDataPoolListMoveCommand(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                     const std::vector<stw_types::uint32> & oru32_SourceRows,
                                     const std::vector<stw_types::uint32> & oru32_TargetRows,
                                     QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   std::vector<stw_types::uint32> mc_SourceRows;
   std::vector<stw_types::uint32> mc_TargetRows;

   void m_DoMoveRows(const std::vector<stw_types::uint32> & orc_SelectedIndices,
                     const std::vector<stw_types::uint32> & orc_TargetIndices);
   void m_MoveItems(const std::vector<stw_types::uint32> & orc_ContiguousIndices,
                    const stw_types::uint32 ou32_TargetIndex) const;
   void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

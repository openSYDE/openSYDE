//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Align undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOALIGNCOMMAND_HPP
#define C_SEBUNOALIGNCOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.hpp"
#include "C_SebUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoAlignCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoAlignCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                        const uint64_t & oru64_GuidelineItemId, const E_Alignment & ore_Alignment,
                        QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoAlignCommand(void) override;
   void undo(void) override;
   void redo(void) override;

private:
   void m_Align(const uint64_t & oru64_GuidelineItemId, const E_Alignment & ore_Alignment);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

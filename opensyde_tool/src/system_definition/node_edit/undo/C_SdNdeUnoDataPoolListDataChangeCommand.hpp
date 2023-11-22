//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data change undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTDATACHANGECOMMAND_HPP
#define C_SDNDEUNODATAPOOLLISTDATACHANGECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoDataPoolListBaseCommand.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListDataChangeCommand :
   public C_SdNdeUnoDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDataPoolListDataChangeCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                           stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget, const uint32_t & oru32_DataPoolListIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ListDataChangeType & ore_DataChangeType,
                                           QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   const uint32_t mu32_DataPoolListIndex;
   QVariant mc_NewData;
   QVariant mc_PreviousData;
   const C_SdNdeDpUtil::E_ListDataChangeType me_DataChangeType;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
   static void mh_ConvertListTypeToGeneric(const stw::opensyde_core::C_OscNodeDataPoolList & orc_OscElement,
                                           const C_SdNdeDpUtil::E_ListDataChangeType & ore_Type,
                                           QVariant & orc_Generic);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

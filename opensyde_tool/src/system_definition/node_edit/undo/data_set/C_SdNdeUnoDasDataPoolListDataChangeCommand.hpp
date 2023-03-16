//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set data change undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTDATACHANGECOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTDATACHANGECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QVariant>
#include "C_SdNdeUnoDasDataPoolListBaseCommand.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListDataChangeCommand :
   public C_SdNdeUnoDasDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListDataChangeCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              const uint32_t & oru32_DataPoolListIndex,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                              const uint32_t & oru32_DataPoolListDataSetIndex,
                                              const QVariant & orc_NewData,
                                              const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType,
                                              QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   const uint32_t mu32_DataPoolListDataSetIndex;
   const C_SdNdeDpUtil::E_DataSetDataChangeType me_DataChangeType;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

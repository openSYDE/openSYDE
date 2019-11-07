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
#include "C_SdNdeUnoDasDataPoolListBaseCommand.h"
#include "C_SdNdeDpUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListDataChangeCommand :
   public C_SdNdeUnoDasDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListDataChangeCommand(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_types::uint32 & oru32_DataPoolIndex,
                                              const stw_types::uint32 & oru32_DataPoolListIndex,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType,
                                              QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   const stw_types::uint32 mu32_DataPoolListDataSetIndex;
   const C_SdNdeDpUtil::E_DataSetDataChangeType me_DataChangeType;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

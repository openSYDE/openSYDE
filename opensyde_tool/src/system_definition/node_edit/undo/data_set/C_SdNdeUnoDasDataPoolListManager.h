//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTMANAGER_H
#define C_SDNDEUNODASDATAPOOLLISTMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoCommand>
#include "stwtypes.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListManager
{
public:
   C_SdNdeUnoDasDataPoolListManager(void);

   void DoMoveElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                       const stw_types::uint32 & oru32_DataPoolListIndex,
                       C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                       const std::vector<stw_types::uint32> & orc_StartIndex,
                       const std::vector<stw_types::uint32> & orc_TargetIndex, const bool & orq_AdaptIndices);
   void DoDeleteElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                         const stw_types::uint32 & oru32_DataPoolListIndex,
                         C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                         const std::vector<stw_types::uint32> & orc_Indices);
   void DoPaste(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_DataPoolListIndex,
                C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                const stw_types::uint32 & oru32_InsertListIndex);
   void DoAddElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                      const stw_types::uint32 & oru32_DataPoolListIndex,
                      C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                      const std::vector<stw_types::uint32> & orc_Indices);
   void DoAddSpecificElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_DataPoolListIndex,
                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                              const std::vector<stw_types::uint32> & orc_Indices,
                              const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                              const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues);
   void DoDataChangeElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                             const stw_types::uint32 & oru32_DataPoolListIndex,
                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                             const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                             const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   QUndoCommand * TakeUndoCommand(void);

private:
   QUndoCommand * mpc_UndoCommand;

   void m_InitUndoCommand(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

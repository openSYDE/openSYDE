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
#include "stwtypes.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListManager
{
public:
   C_SdNdeUnoDasDataPoolListManager(void);

   void DoMoveElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                       const uint32_t & oru32_DataPoolListIndex,
                       C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                       const std::vector<uint32_t> & orc_StartIndex, const std::vector<uint32_t> & orc_TargetIndex,
                       const bool & orq_AdaptIndices);
   void DoDeleteElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                         const uint32_t & oru32_DataPoolListIndex,
                         C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                         const std::vector<uint32_t> & orc_Indices);
   void DoPaste(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                const uint32_t & oru32_DataPoolListIndex,
                C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                const uint32_t & oru32_InsertListIndex);
   void DoAddElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                      const uint32_t & oru32_DataPoolListIndex,
                      C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                      const std::vector<uint32_t> & orc_Indices);
   void DoAddSpecificElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex,
                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                              const std::vector<uint32_t> & orc_Indices,
                              const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
                              const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues);
   void DoDataChangeElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                             const uint32_t & oru32_DataPoolListIndex,
                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                             const uint32_t & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                             const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   QUndoCommand * TakeUndoCommand(void);

private:
   QUndoCommand * mpc_UndoCommand;

   void m_InitUndoCommand(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

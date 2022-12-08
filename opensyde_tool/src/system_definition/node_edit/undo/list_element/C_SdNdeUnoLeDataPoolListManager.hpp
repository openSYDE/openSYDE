//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLELEMENTMANAGER_H
#define C_SDNDEUNOLEDATAPOOLELEMENTMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoStack>
#include "stwtypes.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_UtiUndoStack.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListManager
{
public:
   C_SdNdeUnoLeDataPoolListManager(void);

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
                              const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscData, const std::vector<stw::opensyde_gui_logic::
                                                                                                                                   C_PuiSdNodeDataPoolListElement> & orc_UiData);
   void DoDataChangeElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                             const uint32_t & oru32_DataPoolListIndex,
                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                             const uint32_t & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                             const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                             const uint32_t & oru32_ArrayIndex = 0, const int32_t & ors32_DataSetIndex = -1);

   void SetUndoStack(C_UtiUndoStack * const opc_Value);
   void DoPush(QUndoCommand * const opc_Command);

private:
   C_UtiUndoStack * mpc_UndoStack;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

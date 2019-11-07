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
#include "stwtypes.h"
#include "C_SdNdeDpUtil.h"
#include "C_UtiUndoStack.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListManager
{
public:
   C_SdNdeUnoLeDataPoolListManager(void);

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
                              const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCData, const std::vector<stw_opensyde_gui_logic::
                                                                                                                                  C_PuiSdNodeDataPoolListElement> & orc_UIData);
   void DoDataChangeElements(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                             const stw_types::uint32 & oru32_DataPoolListIndex,
                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                             const stw_types::uint32 & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                             const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                             const stw_types::uint32 & oru32_ArrayIndex = 0,
                             const stw_types::sint32 & ors32_DataSetIndex = -1);

   void SetUndoStack(C_UtiUndoStack * const opc_Value);
   void DoPush(QUndoCommand * const opc_Command);

private:
   C_UtiUndoStack * mpc_UndoStack;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

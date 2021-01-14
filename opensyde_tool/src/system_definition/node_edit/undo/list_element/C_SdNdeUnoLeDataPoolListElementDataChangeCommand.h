//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element data change undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTDATACHANGECOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTDATACHANGECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QVariant>
#include "C_SdNdeUnoLeDataPoolListElementBaseCommand.h"
#include "C_SdNdeDpUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementDataChangeCommand :
   public C_SdNdeUnoLeDataPoolListElementBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementDataChangeCommand(const stw_types::uint32 & oru32_NodeIndex,
                                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListIndex,
                                                    C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const stw_types::uint32 & oru32_DataPoolListElementIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex,
                                                    QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   const stw_types::uint32 mu32_DataPoolListElementIndex;
   const C_SdNdeDpUtil::E_ElementDataChangeType me_DataChangeType;
   const stw_types::uint32 mu32_ArrayIndex;
   const stw_types::sint32 ms32_DataSetIndex;
   bool mq_Initial;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
   void m_ApplyAutoMin(void);
   void m_ApplyAutoMax(void);
   stw_opensyde_core::C_OSCNodeDataPoolContent m_GetCurrentTypeMinGeneric(void) const;
   stw_opensyde_core::C_OSCNodeDataPoolContent m_GetCurrentTypeMaxGeneric(void) const;
   void m_CopySingleValueToArrayIndex(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Single,
                                      const stw_types::uint32 & oru32_ArrayIndex,
                                      stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

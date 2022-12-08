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
#include "C_SdNdeUnoLeDataPoolListElementBaseCommand.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementDataChangeCommand :
   public C_SdNdeUnoLeDataPoolListElementBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementDataChangeCommand(const uint32_t & oru32_NodeIndex,
                                                    const uint32_t & oru32_DataPoolIndex,
                                                    const uint32_t & oru32_DataPoolListIndex,
                                                    C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const uint32_t & oru32_DataPoolListElementIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex,
                                                    QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   const uint32_t mu32_DataPoolListElementIndex;
   const C_SdNdeDpUtil::E_ElementDataChangeType me_DataChangeType;
   const uint32_t mu32_ArrayIndex;
   const int32_t ms32_DataSetIndex;
   bool mq_Initial;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
   void m_ApplyAutoMin(void);
   void m_ApplyAutoMax(void);
   stw::opensyde_core::C_OscNodeDataPoolContent m_GetCurrentTypeMinGeneric(void) const;
   stw::opensyde_core::C_OscNodeDataPoolContent m_GetCurrentTypeMaxGeneric(void) const;
   void m_CopySingleValueToArrayIndex(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Single,
                                      const uint32_t & oru32_ArrayIndex,
                                      stw::opensyde_core::C_OscNodeDataPoolContent & orc_Output) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

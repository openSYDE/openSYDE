//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit data change undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOAEDDATAPOOLLISTDATACHANGECOMMAND_HPP
#define C_SDNDEUNOAEDDATAPOOLLISTDATACHANGECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QVariant>
#include <QUndoCommand>
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoAedDataPoolListDataChangeCommand :
   public QUndoCommand
{
public:
   C_SdNdeUnoAedDataPoolListDataChangeCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                              const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                              const uint32_t & oru32_DataSetIndex,
                                              const uint32_t & oru32_ArrayElementIndex, const QVariant & orc_NewData,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                              QUndoCommand * const opc_Parent = NULL);
   void redo(void) override;
   void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   bool mq_Initial;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_DataPoolIndex;
   const uint32_t mu32_ListIndex;
   const uint32_t mu32_ElementIndex;
   const C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   const uint32_t mu32_DataSetIndex;
   const uint32_t mu32_ItemIndex;
   C_SdNdeDpListModelViewManager * const mpc_DataPoolListModelViewManager;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit data change undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOAEDDATAPOOLLISTDATACHANGECOMMAND_H
#define C_SDNDEUNOAEDDATAPOOLLISTDATACHANGECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QVariant>
#include <QUndoCommand>
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoAedDataPoolListDataChangeCommand :
   public QUndoCommand
{
public:
   C_SdNdeUnoAedDataPoolListDataChangeCommand(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_types::uint32 & oru32_DataPoolIndex,
                                              const stw_types::uint32 & oru32_ListIndex,
                                              const stw_types::uint32 & oru32_ElementIndex,
                                              const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                              const stw_types::uint32 & oru32_DataSetIndex,
                                              const stw_types::uint32 & oru32_ArrayElementIndex,
                                              const QVariant & orc_NewData,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                              QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   QVariant mc_PreviousData;
   QVariant mc_NewData;
   bool mq_Initial;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;
   const stw_types::uint32 mu32_ElementIndex;
   const C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   const stw_types::uint32 mu32_DataSetIndex;
   const stw_types::uint32 mu32_ItemIndex;
   C_SdNdeDpListModelViewManager * const mpc_DataPoolListModelViewManager;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
   C_SdNdeDpUtil::E_ElementDataChangeType m_ConvertToChangeType(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

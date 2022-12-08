//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOAEDDATAPOOLLISTMANAGER_H
#define C_SDNDEUNOAEDDATAPOOLLISTMANAGER_H

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

class C_SdNdeUnoAedDataPoolListManager
{
public:
   C_SdNdeUnoAedDataPoolListManager(void);

   void DoDataChangeElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                             const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                             const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                             const uint32_t & oru32_DataSetIndex, const uint32_t & oru32_ArrayElementIndex,
                             const QVariant & orc_NewData,
                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager);
   QUndoCommand * TakeUndoCommand(void);

private:
   QUndoCommand * mpc_UndoCommand;

   void m_InitUndoCommand(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

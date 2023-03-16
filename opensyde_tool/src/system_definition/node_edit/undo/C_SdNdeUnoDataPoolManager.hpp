//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLMANAGER_H
#define C_SDNDEUNODATAPOOLMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_UtiUndoStack.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui
{
class C_SdNdeDpListsTreeWidget;
}

namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolManager :
   public C_UtiUndoStack
{
   Q_OBJECT

public:
   C_SdNdeUnoDataPoolManager(QObject * const opc_Parent = NULL);
   ~C_SdNdeUnoDataPoolManager(void) override;

   void DoMoveList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                   stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                   const std::vector<uint32_t> & orc_StartIndices, const std::vector<uint32_t> & orc_TargetIndices);
   void DoDeleteList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                     stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                     const std::vector<uint32_t> & orc_Indices);
   void DoPaste(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                const uint32_t & oru32_InsertListIndex);
   void DoAddList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                  stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                  const std::vector<uint32_t> & orc_Indices);
   void DoChangeListData(const uint32_t & oru32_NodeIndex,     const uint32_t & oru32_DataPoolIndex,
                         stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                         const uint32_t & oru32_DataPoolListIndex, const QVariant & orc_NewData,
                         const C_SdNdeDpUtil::E_ListDataChangeType & ore_DataChangeType);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);

protected:
   void m_CleanupAction(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

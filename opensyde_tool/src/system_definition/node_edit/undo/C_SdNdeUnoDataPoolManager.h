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

#include "stwtypes.h"
#include "C_SdNdeDpUtil.h"
#include "C_UtiUndoStack.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui
{
class C_SdNdeDpListsTreeWidget;
}

namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolManager :
   public C_UtiUndoStack
{
   Q_OBJECT

public:
   C_SdNdeUnoDataPoolManager(QObject * const opc_Parent = NULL);
   virtual ~C_SdNdeUnoDataPoolManager(void);

   void DoMoveList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                   stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                   const std::vector<stw_types::uint32> & oru32_StartIndices,
                   const std::vector<stw_types::uint32> & oru32_TargetIndices);
   void DoDeleteList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                     stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                     const std::vector<stw_types::uint32> & orc_Indices);
   void DoPaste(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                const stw_types::uint32 & oru32_InsertListIndex);
   void DoAddList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                  stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                  const std::vector<stw_types::uint32> & orc_Indices);
   void DoChangeListData(const stw_types::uint32 & oru32_NodeIndex,     const stw_types::uint32 & oru32_DataPoolIndex,
                         stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                         const stw_types::uint32 & oru32_DataPoolListIndex, const QVariant & orc_NewData,
                         const C_SdNdeDpUtil::E_ListDataChangeType & ore_DataChangeType);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);

protected:
   virtual void m_CleanupAction(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

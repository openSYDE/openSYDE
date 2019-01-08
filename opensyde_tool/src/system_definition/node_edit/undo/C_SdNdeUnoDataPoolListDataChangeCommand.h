//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data change undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTDATACHANGECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTDATACHANGECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SdNdeUnoDataPoolListBaseCommand.h"
#include "C_SdNdeDataPoolUtil.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListDataChangeCommand :
   public C_SdNdeUnoDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDataPoolListDataChangeCommand(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                           stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget, const stw_types::uint32 & oru32_DataPoolListIndex, const QVariant & orc_NewData, const C_SdNdeDataPoolUtil::E_ListDataChangeType & ore_DataChangeType,
                                           QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   const stw_types::uint32 mu32_DataPoolListIndex;
   QVariant mc_NewData;
   QVariant mc_PreviousData;
   const C_SdNdeDataPoolUtil::E_ListDataChangeType me_DataChangeType;

   void m_Change(QVariant & orc_PreviousData, const QVariant & orc_NewData);
   static void mh_ConvertListTypeToGeneric(const stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCElement,
                                           const C_PuiSdNodeDataPoolList & orc_UIElement,
                                           const C_SdNdeDataPoolUtil::E_ListDataChangeType & ore_Type,
                                           QVariant & orc_Generic);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

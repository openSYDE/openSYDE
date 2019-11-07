//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list base class for add and delete commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTADDDELETEBASECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "stwtypes.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoDataPoolListBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListAddDeleteBaseCommand :
   public C_SdNdeUnoDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_types::uint32 & oru32_DataPoolIndex,
                                              stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                              const std::vector<stw_types::uint32> & orc_Indices, const QString & orc_Text = "",
                                              QUndoCommand * const opc_Parent = NULL);

protected:
   void Add(void);
   void Delete(void);
   void SetIndices(const std::vector<stw_types::uint32> & orc_Value);
   void SetInitialData(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolList> & orc_OSCContent,
                       const std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent);

private:
   std::vector<stw_types::uint32> mc_Indices;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolList> mc_OSCContent;
   std::vector<C_PuiSdNodeDataPoolList> mc_UIContent;

   void m_SortDescending(void);
   void m_SortAscending(void);
   void m_UpdateModels(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

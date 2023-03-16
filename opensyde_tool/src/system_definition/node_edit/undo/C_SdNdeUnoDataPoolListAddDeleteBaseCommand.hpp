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
#include "stwtypes.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_SdNdeUnoDataPoolListBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListAddDeleteBaseCommand :
   public C_SdNdeUnoDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                              const std::vector<uint32_t> & orc_Indices, const QString & orc_Text = "",
                                              QUndoCommand * const opc_Parent = NULL);

protected:
   void m_Add(void);
   void m_Delete(void);
   void m_SetIndices(const std::vector<uint32_t> & orc_Value);
   void m_SetInitialData(const std::vector<stw::opensyde_core::C_OscNodeDataPoolList> & orc_OscContent,
                         const std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent);

private:
   std::vector<uint32_t> mc_Indices;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolList> mc_OscContent;
   std::vector<C_PuiSdNodeDataPoolList> mc_UiContent;

   void m_SortDescending(void);
   void m_SortAscending(void);
   void m_UpdateModels(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element base class for add and delete commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTADDDELETEBASECOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "stwtypes.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_SdNdeUnoLeDataPoolListElementBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand :
   public C_SdNdeUnoLeDataPoolListElementBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(const uint32_t & oru32_NodeIndex,
                                                       const uint32_t & oru32_DataPoolIndex,
                                                       const uint32_t & oru32_DataPoolListIndex,
                                                       C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                       const std::vector<uint32_t> & orc_Indices, const QString & orc_Text = "",
                                                       QUndoCommand * const opc_Parent = NULL);

protected:
   void m_Add(void);
   void m_Delete(void);
   void m_SetIndices(const std::vector<uint32_t> & orc_Value);
   void m_SetInitialData(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscContent,
                         const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent);
   void m_ReSelect(const std::vector<std::vector<uint32_t> > & orc_Items, const bool oq_ScrollToLast);

private:
   std::vector<uint32_t> mc_Indices;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> mc_OscContent;
   std::vector<C_PuiSdNodeDataPoolListElement> mc_UiContent;

   void m_SortDescending(void);
   void m_SortAscending(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

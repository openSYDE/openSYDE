//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set base class for add and delete commands (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTADDDELETEBASECOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "stwtypes.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_SdNdeUnoDasDataPoolListBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand :
   public C_SdNdeUnoDasDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 const uint32_t & oru32_DataPoolListIndex,
                                                 C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                 const std::vector<uint32_t> & orc_Indices,
                                                 const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   void m_Add(void);
   void m_Delete(void);
   void m_SetIndices(const std::vector<uint32_t> & orc_Value);
   void m_SetInitialData(const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
                         const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues);

   bool mq_Initial;

private:
   std::vector<uint32_t> mc_Indices;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> mc_OscNames;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > mc_OscDataSetValues;

   void m_SortDescending(void);
   void m_SortAscending(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

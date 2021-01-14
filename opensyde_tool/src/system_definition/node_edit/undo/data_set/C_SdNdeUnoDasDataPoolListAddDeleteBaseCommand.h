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
#include "stwtypes.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_SdNdeUnoDasDataPoolListBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand :
   public C_SdNdeUnoDasDataPoolListBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(const stw_types::uint32 & oru32_NodeIndex,
                                                 const stw_types::uint32 & oru32_DataPoolIndex,
                                                 const stw_types::uint32 & oru32_DataPoolListIndex,
                                                 C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                 const std::vector<stw_types::uint32> & orc_Indices,
                                                 const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   void m_Add(void);
   void m_Delete(void);
   void m_SetIndices(const std::vector<stw_types::uint32> & orc_Value);
   void m_SetInitialData(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                         const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues);

   bool mq_Initial;

private:
   std::vector<stw_types::uint32> mc_Indices;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> mc_OSCNames;
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > mc_OSCDataSetValues;

   void m_SortDescending(void);
   void m_SortAscending(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element base class for add and delete commands (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTADDDELETEBASECOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include "stwtypes.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_SdNdeUnoLeDataPoolListElementBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand :
   public C_SdNdeUnoLeDataPoolListElementBaseCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(const stw_types::uint32 & oru32_NodeIndex,
                                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                                       const stw_types::uint32 & oru32_DataPoolListIndex,
                                                       C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                       const std::vector<stw_types::uint32> & orc_Indices, const QString & orc_Text = "",
                                                       QUndoCommand * const opc_Parent = NULL);

protected:
   void Add(void);
   void Delete(void);
   void SetIndices(const std::vector<stw_types::uint32> & orc_Value);
   void SetInitialData(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCContent,
                       const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent);

private:
   std::vector<stw_types::uint32> mc_Indices;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> mc_OSCContent;
   std::vector<C_PuiSdNodeDataPoolListElement> mc_UIContent;

   void m_SortDescending(void);
   void m_SortAscending(void);
   bool m_CheckContinuous(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list data set add undo command (implementation)

   Data pool list data set add undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoDasDataPoolListAddCommand.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in,out] opc_Parent                       Optional pointer to parent

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListAddCommand::C_SdNdeUnoDasDataPoolListAddCommand(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         const uint32 & oru32_DataPoolListIndex,
                                                                         C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                         const std::vector<uint32> & orc_Indices,
                                                                         QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                 opc_DataPoolListModelViewManager,
                                                 orc_Indices,
                                                 "Add List Dataset", opc_Parent)
{
   const C_OSCNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                          oru32_DataPoolListIndex);

   std::vector<C_OSCNodeDataPoolDataSet> c_OSCNames;
   std::vector<std::vector<C_OSCNodeDataPoolContent> > c_OSCDataSetValues;
   c_OSCNames.resize(orc_Indices.size());
   c_OSCDataSetValues.resize(orc_Indices.size());
   //Init data set values
   if (pc_List != NULL)
   {
      for (uint32 u32_ItDataSet = 0; u32_ItDataSet < c_OSCDataSetValues.size(); ++u32_ItDataSet)
      {
         c_OSCDataSetValues[u32_ItDataSet].resize(pc_List->c_Elements.size());
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_List->c_Elements.size(); ++u32_ItElement)
         {
            const C_OSCNodeDataPoolListElement & rc_DataElement = pc_List->c_Elements[u32_ItElement];
            c_OSCDataSetValues[u32_ItDataSet][u32_ItElement] = rc_DataElement.c_MinValue;
         }
      }
   }
   this->SetInitialData(c_OSCNames, c_OSCDataSetValues);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo add

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddCommand::redo(void)
{
   this->Add();
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo add

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddCommand::undo(void)
{
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::undo();
   this->Delete();
   this->mq_Initial = false;
}

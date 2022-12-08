//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (implementation)

   Setup style undo command

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdManUnoTopologySetupStyleCommand.hpp"
#include "C_GiLiBus.hpp"
#include "C_SdTopologyDataSnapshot.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Scene   Pointer to currently active scene
   \param[in]     orc_Ids     Affected unique IDs
   \param[in]     oq_DarkMode Optional flag if dark mode is active
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologySetupStyleCommand::C_SdManUnoTopologySetupStyleCommand(QGraphicsScene * const opc_Scene,
                                                                         const std::vector<uint64_t> & orc_Ids,
                                                                         const bool oq_DarkMode,
                                                                         QUndoCommand * const opc_Parent) :
   C_SebUnoSetupStyleCommand(opc_Scene, orc_Ids, oq_DarkMode, opc_Parent)
{
   delete (this->mpc_PreviousState);
   delete (this->mpc_NextState);
   this->mpc_PreviousState = new C_SdTopologyDataSnapshot();
   this->mpc_NextState = new C_SdTopologyDataSnapshot();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdManUnoTopologySetupStyleCommand::~C_SdManUnoTopologySetupStyleCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore all style features for all internally stored items from the snapshot using the preserved mapping

   \param[out] orc_MapIdToTypeAndIndex Map for ID to state data entry
   \param[out] opc_Snapshot            Preserved state data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologySetupStyleCommand::m_Restore(const QMap<uint64_t,
                                                               C_PuiBsTemporaryDataId> & orc_MapIdToTypeAndIndex,
                                                    const C_PuiBsElements * const opc_Snapshot)
{
   const C_SdTopologyDataSnapshot * const pc_Snapshot =
      dynamic_cast<const C_SdTopologyDataSnapshot * const>(opc_Snapshot);

   C_SebUnoSetupStyleCommand::m_Restore(orc_MapIdToTypeAndIndex, opc_Snapshot);

   if (pc_Snapshot != NULL)
   {
      const vector<QGraphicsItem *> c_Items = m_GetSceneItems();
      QMap<uint64_t, C_PuiBsTemporaryDataId>::const_iterator c_Entry;

      for (vector<QGraphicsItem *>::const_iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
      {
         //Bus

         C_GiLiBus * const pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItItem);
         if (pc_Bus != NULL)
         {
            c_Entry = orc_MapIdToTypeAndIndex.find(pc_Bus->GetId());
            if (c_Entry != orc_MapIdToTypeAndIndex.end())
            {
               if (c_Entry.value().u32_Index < pc_Snapshot->c_UiBuses.size())
               //Content
               {
                  const C_PuiSdBus & rc_UiBus = pc_Snapshot->c_UiBuses[c_Entry.value().u32_Index];
                  pc_Bus->SetColor(rc_UiBus.c_UiColor);
                  pc_Bus->SetMiddleLineColor(rc_UiBus.c_UiColorMiddleLine);
                  pc_Bus->SetWidth(rc_UiBus.s32_UiWidthPixels);
                  pc_Bus->TriggerSigChangedGraphic();
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all style features of the specified items in the snapshot and preserve a mapping for later access

   \param[in]  orc_Items    Items to save style features for
   \param[out] orc_Map      Map for ID to state data entry
   \param[out] opc_Snapshot Preserved state data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdManUnoTopologySetupStyleCommand::m_CreateMapAndSaveState(const vector<QGraphicsItem *> & orc_Items,
                                                                  QMap<uint64_t, C_PuiBsTemporaryDataId> & orc_Map,
                                                                  C_PuiBsElements * const opc_Snapshot) const
{
   C_SdTopologyDataSnapshot * const pc_Snapshot = dynamic_cast<C_SdTopologyDataSnapshot * const>(opc_Snapshot);

   C_SebUnoSetupStyleCommand::m_CreateMapAndSaveState(orc_Items, orc_Map, opc_Snapshot);

   if (pc_Snapshot != NULL)
   {
      C_GiLiBus * pc_Bus;
      uint32_t u32_Index;

      for (vector<QGraphicsItem *>::const_iterator c_ItItem = orc_Items.begin(); c_ItItem != orc_Items.end();
           ++c_ItItem)
      {
         //Bus

         pc_Bus = dynamic_cast<C_GiLiBus *>(*c_ItItem);
         if (pc_Bus != NULL)
         {
            //Map
            u32_Index = pc_Snapshot->c_UiBuses.size();
            orc_Map.insert(pc_Bus->GetId(),
                           C_PuiBsTemporaryDataId(static_cast<int32_t>(C_PuiSdDataElement::eBUS), u32_Index));
            pc_Snapshot->c_UiBuses.resize(static_cast<std::vector<C_PuiSdBus>::size_type>(u32_Index + 1UL));
            //Content
            {
               C_PuiSdBus & rc_UiBus = pc_Snapshot->c_UiBuses[u32_Index];
               rc_UiBus.c_UiColor = pc_Bus->GetColor();
               rc_UiBus.c_UiColorMiddleLine = pc_Bus->GetMiddleLineColor();
               rc_UiBus.s32_UiWidthPixels = pc_Bus->GetWidth();
            }
         }
      }
   }
}

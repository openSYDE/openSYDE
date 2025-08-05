//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for sync operations for data element IDs
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEDATAPOOLLISTELEMENTIDSYNCUTIL_HPP
#define C_PUISDNODEDATAPOOLLISTELEMENTIDSYNCUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolContent.hpp"
#include "C_PuiSdLastKnownHalElementId.hpp"
#include "C_OscNodeDataPoolListElementOptArrayId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeDataPoolListElementIdSyncUtil
{
public:
   static void h_OnSyncNodeAdded(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                 const uint32_t ou32_Index);
   static bool h_OnSyncNodeAboutToBeDeleted(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                            const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid = false);
   static void h_OnSyncNodeAboutToBeDeleted(std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                     C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
                                            const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid = false);
   static void h_OnSyncNodeDataPoolAdded(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolAdded(std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                  C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolMoved(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                         const uint32_t ou32_DataPoolTargetIndex);
   static void h_OnSyncNodeDataPoolMoved(std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                  C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                         const uint32_t ou32_DataPoolTargetIndex);
   static bool h_OnSyncNodeDataPoolAboutToBeDeleted(
      stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
      const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolAboutToBeDeleted(std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                             C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
                                                    const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolListAdded(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListMoved(stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId,
                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex);
   static bool h_OnSyncNodeDataPoolListAboutToBeDeleted(
      stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
      const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListElementAdded(
      stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
      const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   static void h_OnSyncNodeDataPoolListElementRangeChanged(const uint32_t ou32_NodeIndex,
                                                           const uint32_t ou32_DataPoolIndex,
                                                           const uint32_t ou32_ListIndex,
                                                           const uint32_t ou32_ElementIndex,
                                                           const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement, const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement, const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);
   static void h_OnSyncNodeDataPoolListElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex,
                                                                 const uint32_t ou32_DataPoolIndex,
                                                                 const uint32_t ou32_ListIndex,
                                                                 const uint32_t ou32_ElementIndex,
                                                                 const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray, const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);
   static void h_OnSyncNodeDataPoolListElementMoved(
      stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
      const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex,
      const uint32_t ou32_ElementTargetIndex);
   static bool h_OnSyncNodeDataPoolListElementAboutToBeDeleted(
      stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
      const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   static bool h_OnSyncNodeHalc(stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_DataElementId,
                                const uint32_t ou32_Index,
                                const std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                               stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> & orc_MapCurToNew);

   static int32_t h_CheckAndHandleNewElement(
      const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_NewId,
      std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
               C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs);
   static void h_GetNewMapOnSyncHalc(const uint32_t ou32_Index,
                                     std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                              stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> & orc_MapCurToNew, std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                                                                                                                      C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs);

private:
   C_PuiSdNodeDataPoolListElementIdSyncUtil();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

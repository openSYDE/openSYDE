//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for sync operations for data element IDs

   Utility for sync operations for data element IDs

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "C_PuiSdNodeDataPoolListElementIdSyncUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_Index          Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAdded(C_OscNodeDataPoolListElementId & orc_DataElementId,
                                                                 const uint32_t ou32_Index)
{
   if (orc_DataElementId.u32_NodeIndex >= ou32_Index)
   {
      ++orc_DataElementId.u32_NodeIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_Index          Node index
   \param[in]      oq_OnlyMarkInvalid  Flag to only mark invalid (in case of replacement rather than delete)

   \return
   Flags

   \retval   True    Element removed
   \retval   False   Element still exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAboutToBeDeleted(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid)
{
   bool q_Remove = false;

   if (orc_DataElementId.u32_NodeIndex > ou32_Index)
   {
      if (!oq_OnlyMarkInvalid)
      {
         --orc_DataElementId.u32_NodeIndex;
      }
   }
   else if (orc_DataElementId.u32_NodeIndex == ou32_Index)
   {
      q_Remove = true;
   }
   else
   {
      //No adaptation necessary
   }
   return q_Remove;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to deleted node index

   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs
   \param[in]      ou32_Index             Deleted node index
   \param[in]      oq_OnlyMarkInvalid     Only mark invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAboutToBeDeleted(
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
   const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid)
{
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> c_Tmp;
   for (std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId>::const_iterator c_It =
           orc_LastKnownHalcCrcs.begin();
        c_It != orc_LastKnownHalcCrcs.end(); ++c_It)
   {
      C_OscNodeDataPoolListElementOptArrayId c_Id = c_It->first;
      if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAboutToBeDeleted(c_Id, ou32_Index,
                                                                                 oq_OnlyMarkInvalid) == false)
      {
         c_Tmp[c_Id] = c_It->second;
      }
   }
   orc_LastKnownHalcCrcs = c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAdded(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
   {
      if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolIndex)
      {
         ++orc_DataElementId.u32_DataPoolIndex;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs
   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataPoolIndex     Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAdded(
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> c_Tmp;
   for (std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId>::const_iterator c_It =
           orc_LastKnownHalcCrcs.begin();
        c_It != orc_LastKnownHalcCrcs.end(); ++c_It)
   {
      C_OscNodeDataPoolListElementOptArrayId c_Id = c_It->first;
      C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
      c_Tmp[c_Id] = c_It->second;
   }
   orc_LastKnownHalcCrcs = c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId         Data element ID
   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataPoolSourceIndex  Source data pool index
   \param[in]      ou32_DataPoolTargetIndex  Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolMoved(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex,
   const uint32_t ou32_DataPoolSourceIndex, const uint32_t ou32_DataPoolTargetIndex)
{
   if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
   {
      if (orc_DataElementId.u32_DataPoolIndex > ou32_DataPoolSourceIndex)
      {
         --orc_DataElementId.u32_DataPoolIndex;
         if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolTargetIndex)
         {
            ++orc_DataElementId.u32_DataPoolIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
      else if (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolSourceIndex)
      {
         orc_DataElementId.u32_DataPoolIndex = ou32_DataPoolTargetIndex;
      }
      else
      {
         if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolTargetIndex)
         {
            ++orc_DataElementId.u32_DataPoolIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_LastKnownHalcCrcs     Last known halc crcs
   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataPoolSourceIndex  Source data pool index
   \param[in]      ou32_DataPoolTargetIndex  Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolMoved(
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex, const uint32_t ou32_DataPoolTargetIndex)
{
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> c_Tmp;
   for (std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId>::const_iterator c_It =
           orc_LastKnownHalcCrcs.begin();
        c_It != orc_LastKnownHalcCrcs.end(); ++c_It)
   {
      C_OscNodeDataPoolListElementOptArrayId c_Id = c_It->first;
      C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolMoved(c_Id, ou32_NodeIndex,
                                                                          ou32_DataPoolSourceIndex,
                                                                          ou32_DataPoolTargetIndex);
      c_Tmp[c_Id] = c_It->second;
   }
   orc_LastKnownHalcCrcs = c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index

   \return
   Flags

   \retval   True    Element removed
   \retval   False   Element still exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAboutToBeDeleted(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   bool q_Remove = false;

   if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
   {
      if (orc_DataElementId.u32_DataPoolIndex > ou32_DataPoolIndex)
      {
         --orc_DataElementId.u32_DataPoolIndex;
      }
      else if (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)
      {
         q_Remove = true;
      }
      else
      {
         //No adaptation necessary
      }
   }
   return q_Remove;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs
   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataPoolIndex     Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAboutToBeDeleted(
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs,
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> c_Tmp;
   for (std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId>::const_iterator c_It =
           orc_LastKnownHalcCrcs.begin();
        c_It != orc_LastKnownHalcCrcs.end(); ++c_It)
   {
      C_OscNodeDataPoolListElementOptArrayId c_Id = c_It->first;
      if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAboutToBeDeleted(c_Id, ou32_NodeIndex,
                                                                                         ou32_DataPoolIndex) ==
          false)
      {
         c_Tmp[c_Id] = c_It->second;
      }
   }
   orc_LastKnownHalcCrcs = c_Tmp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListAdded(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListIndex)
{
   if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
       (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
   {
      if (orc_DataElementId.u32_ListIndex >= ou32_ListIndex)
      {
         ++orc_DataElementId.u32_ListIndex;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId      Data element ID
   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataPoolIndex     Data pool index
   \param[in]      ou32_ListSourceIndex   Source list index
   \param[in]      ou32_ListTargetIndex   Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListMoved(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex)
{
   if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
       (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
   {
      if (orc_DataElementId.u32_ListIndex > ou32_ListSourceIndex)
      {
         --orc_DataElementId.u32_ListIndex;
         if (orc_DataElementId.u32_ListIndex >= ou32_ListTargetIndex)
         {
            ++orc_DataElementId.u32_ListIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
      else if (orc_DataElementId.u32_ListIndex == ou32_ListSourceIndex)
      {
         orc_DataElementId.u32_ListIndex = ou32_ListTargetIndex;
      }
      else
      {
         if (orc_DataElementId.u32_ListIndex >= ou32_ListTargetIndex)
         {
            ++orc_DataElementId.u32_ListIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index

   \return
   Flags

   \retval   True    Element removed
   \retval   False   Element still exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListAboutToBeDeleted(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListIndex)
{
   bool q_Remove = false;

   if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
       (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
   {
      if (orc_DataElementId.u32_ListIndex > ou32_ListIndex)
      {
         --orc_DataElementId.u32_ListIndex;
      }
      else if (orc_DataElementId.u32_ListIndex == ou32_ListIndex)
      {
         q_Remove = true;
      }
      else
      {
         //No adaptation necessary
      }
   }
   return q_Remove;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementAdded(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex)
{
   if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
        (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
       (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
   {
      if (orc_DataElementId.u32_ElementIndex >= ou32_ElementIndex)
      {
         ++orc_DataElementId.u32_ElementIndex;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index
   \param[in]      orc_MinElement      Min element
   \param[in]      orc_MaxElement      Max element
   \param[in]      orc_DataElementId   Data element ID
   \param[in,out]  orc_Value           Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementRangeChanged(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
   const uint32_t ou32_ElementIndex, const C_OscNodeDataPoolContent & orc_MinElement,
   const C_OscNodeDataPoolContent & orc_MaxElement, const C_OscNodeDataPoolListElementId & orc_DataElementId,
   C_OscNodeDataPoolContent & orc_Value)
{
   if (orc_DataElementId ==
       C_OscNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex))
   {
      C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_Unused;
      tgl_assert(C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_MinElement,
                                                                       orc_MaxElement,
                                                                       orc_Value, e_Unused,
                                                                       C_OscNodeDataPoolContentUtil::eLEAVE_VALUE) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index
   \param[in]      oe_Type             Type
   \param[in]      oq_IsArray          Is array
   \param[in]      orc_DataElementId   Data element ID
   \param[in,out]  orc_Value           Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementTypeOrArrayChanged(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
   const uint32_t ou32_ElementIndex, const C_OscNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray,
   const C_OscNodeDataPoolListElementId & orc_DataElementId, C_OscNodeDataPoolContent & orc_Value)
{
   if (orc_DataElementId ==
       C_OscNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex))
   {
      orc_Value.SetType(oe_Type);
      orc_Value.SetArray(oq_IsArray);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId         Data element ID
   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataPoolIndex        Data pool index
   \param[in]      ou32_ListIndex            List index
   \param[in]      ou32_ElementSourceIndex   Source element index
   \param[in]      ou32_ElementTargetIndex   Target element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementMoved(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex, const uint32_t ou32_ElementTargetIndex)
{
   if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
        (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
       (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
   {
      if (orc_DataElementId.u32_ElementIndex > ou32_ElementSourceIndex)
      {
         --orc_DataElementId.u32_ElementIndex;
         if (orc_DataElementId.u32_ElementIndex >= ou32_ElementTargetIndex)
         {
            ++orc_DataElementId.u32_ElementIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
      else if (orc_DataElementId.u32_ElementIndex == ou32_ElementSourceIndex)
      {
         orc_DataElementId.u32_ElementIndex = ou32_ElementTargetIndex;
      }
      else
      {
         if (orc_DataElementId.u32_ElementIndex >= ou32_ElementTargetIndex)
         {
            ++orc_DataElementId.u32_ElementIndex;
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index

   \return
   Flags

   \retval   True    Element removed
   \retval   False   Element still exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementAboutToBeDeleted(
   C_OscNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
   const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex)
{
   bool q_Remove = false;

   if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
        (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
       (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
   {
      if (orc_DataElementId.u32_ElementIndex > ou32_ElementIndex)
      {
         --orc_DataElementId.u32_ElementIndex;
      }
      else if (orc_DataElementId.u32_ElementIndex == ou32_ElementIndex)
      {
         q_Remove = true;
      }
      else
      {
         //No adaptation necessary
      }
   }
   return q_Remove;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element id
   \param[in]      ou32_Index          Index
   \param[in]      orc_MapCurToNew     Map cur to new

   \return
   Flags

   \retval   True    Element removed
   \retval   False   Element still exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeHalc(
   C_OscNodeDataPoolListElementOptArrayId & orc_DataElementId, const uint32_t ou32_Index,
   const std::map<C_OscNodeDataPoolListElementOptArrayId, C_OscNodeDataPoolListElementOptArrayId> & orc_MapCurToNew)
{
   bool q_Remove = false;

   if (orc_DataElementId.u32_NodeIndex == ou32_Index)
   {
      const std::map<C_OscNodeDataPoolListElementOptArrayId,
                     C_OscNodeDataPoolListElementOptArrayId>::const_iterator c_It = orc_MapCurToNew.find(
         orc_DataElementId);
      if (c_It != orc_MapCurToNew.end())
      {
         //Replace
         orc_DataElementId = c_It->second;
      }
      else
      {
         if (C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_DataElementId.u32_NodeIndex,
                                                             orc_DataElementId.u32_DataPoolIndex) != NULL)
         {
            C_OscNodeDataPool::E_Type e_Type;
            if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(ou32_Index, orc_DataElementId.u32_DataPoolIndex,
                                                                 e_Type) == C_NO_ERR)
            {
               if ((e_Type == C_OscNodeDataPool::eHALC) || (e_Type == C_OscNodeDataPool::eHALC_NVM))
               {
                  //Delete
                  q_Remove = true;
               }
            }
         }
         else
         {
            //Delete
            q_Remove = true;
         }
      }
   }
   return q_Remove;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check and handle new element

   \param[in]      orc_NewId              New id
   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdNodeDataPoolListElementIdSyncUtil::h_CheckAndHandleNewElement(
   const C_OscNodeDataPoolListElementOptArrayId & orc_NewId, std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                      C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs)
{
   int32_t s32_Retval = C_NO_ERR;

   C_OscNodeDataPool::E_Type e_Type;
   if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(orc_NewId.u32_NodeIndex, orc_NewId.u32_DataPoolIndex,
                                                        e_Type) == C_NO_ERR)
   {
      if ((e_Type == C_OscNodeDataPool::eHALC) ||
          (e_Type == C_OscNodeDataPool::eHALC_NVM))
      {
         const C_OscNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_NewId.u32_NodeIndex);
         const C_OscNodeDataPool * const pc_Dp =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_NewId.u32_NodeIndex,
                                                            orc_NewId.u32_DataPoolIndex);
         const C_OscNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_NewId.u32_NodeIndex,
                                                                       orc_NewId.u32_DataPoolIndex,
                                                                       orc_NewId.u32_ListIndex,
                                                                       orc_NewId.u32_ElementIndex);
         if ((pc_Node != NULL) && ((pc_Element != NULL) && (pc_Dp != NULL)))
         {
            const std::string c_Tmp = orc_NewId.GetHalChannelName().c_str();
            uint32_t u32_Hash = 0UL;
            if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
            {
               pc_Element->CalcHashElement(u32_Hash, orc_NewId.GetArrayElementIndexOrZero());
            }
            else
            {
               pc_Element->CalcHashStructure(u32_Hash);
            }
            stw::scl::C_SclChecksums::CalcCRC32(
               c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
            orc_LastKnownHalcCrcs[orc_NewId] = C_PuiSdLastKnownHalElementId(
               u32_Hash, pc_Dp->c_Name.c_str());
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get new map on sync halc

   \param[in]      ou32_Index             Index
   \param[out]     orc_MapCurToNew        Map cur to new
   \param[in,out]  orc_LastKnownHalcCrcs  Last known halc crcs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeDataPoolListElementIdSyncUtil::h_GetNewMapOnSyncHalc(const uint32_t ou32_Index,
                                                                     std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                              C_OscNodeDataPoolListElementOptArrayId> & orc_MapCurToNew, std::map<C_OscNodeDataPoolListElementOptArrayId,
                                                                                                                                                  C_PuiSdLastKnownHalElementId> & orc_LastKnownHalcCrcs)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_Index);

   if (pc_Node != NULL)
   {
      std::map<C_OscNodeDataPoolListElementOptArrayId, C_PuiSdLastKnownHalElementId> c_NewMap;
      for (uint32_t u32_ItDp = 0UL; u32_ItDp < pc_Node->c_DataPools.size(); ++u32_ItDp)
      {
         const C_OscNodeDataPool & rc_Dp = pc_Node->c_DataPools[u32_ItDp];
         if ((rc_Dp.e_Type == C_OscNodeDataPool::eHALC) || (rc_Dp.e_Type == C_OscNodeDataPool::eHALC_NVM))
         {
            for (uint32_t u32_ItLi = 0UL; u32_ItLi < rc_Dp.c_Lists.size(); ++u32_ItLi)
            {
               const C_OscNodeDataPoolList & rc_Li = rc_Dp.c_Lists[u32_ItLi];
               for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_Li.c_Elements.size(); ++u32_ItEl)
               {
                  const C_OscNodeDataPoolListElement & rc_El = rc_Li.c_Elements[u32_ItEl];
                  for (uint32_t u32_ItAr = 0UL; u32_ItAr < rc_El.GetArraySize(); ++u32_ItAr)
                  {
                     C_OscNodeDataPoolListElementOptArrayId c_NewId(ou32_Index, u32_ItDp, u32_ItLi, u32_ItEl,
                                                                    rc_El.GetArraySize() != 1, u32_ItAr);
                     uint32_t u32_DomainIndex;
                     bool q_UseChannelIndex;
                     uint32_t u32_ChannelIndex;

                     C_OscHalcDefDomain::E_VariableSelector e_Selector;
                     uint32_t u32_ParameterIndex;
                     bool q_UseElementIndex;
                     uint32_t u32_ParameterElementIndex;
                     bool q_IsUseCaseIndex;
                     bool q_IsChanNumIndex;
                     bool q_IsSafetyFlagIndex;

                     if (C_PuiSdHandler::h_GetInstance()->TranslateToHalcIndex(c_NewId,
                                                                               c_NewId.GetArrayElementIndexOrZero(),
                                                                               u32_DomainIndex, q_UseChannelIndex,
                                                                               u32_ChannelIndex, e_Selector,
                                                                               u32_ParameterIndex,
                                                                               q_UseElementIndex,
                                                                               u32_ParameterElementIndex,
                                                                               q_IsUseCaseIndex,
                                                                               q_IsChanNumIndex,
                                                                               q_IsSafetyFlagIndex) == C_NO_ERR)
                     {
                        QString c_Name;
                        if (C_PuiSdHandler::h_GetInstance()->GetHalChannelOrDomainName(ou32_Index,
                                                                                       u32_DomainIndex,
                                                                                       u32_ChannelIndex,
                                                                                       c_Name) == C_NO_ERR)
                        {
                           const std::string c_Tmp = c_Name.toStdString();
                           uint32_t u32_Hash = 0UL;
                           if (pc_Node->c_HalcConfig.e_SafetyMode == C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
                           {
                              rc_El.CalcHashElement(u32_Hash, u32_ItAr);
                           }
                           else
                           {
                              rc_El.CalcHashStructure(u32_Hash);
                           }
                           stw::scl::C_SclChecksums::CalcCRC32(
                              c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
                           c_NewId.SetHalChannelName(c_Tmp);
                           for (std::map<C_OscNodeDataPoolListElementOptArrayId,
                                         C_PuiSdLastKnownHalElementId>::const_iterator c_ItCur =
                                   orc_LastKnownHalcCrcs.begin();
                                c_ItCur != orc_LastKnownHalcCrcs.end();
                                ++c_ItCur)
                           {
                              if (((c_ItCur->second.u32_Crc == u32_Hash) &&
                                   (c_ItCur->second.c_HalDpName.compare(rc_Dp.c_Name.c_str()) == 0)) &&
                                  ((pc_Node->c_HalcConfig.e_SafetyMode ==
                                    C_OscHalcDefBase::eTWO_LEVELS_WITH_DROPPING) ||
                                   (c_ItCur->first.GetArrayElementIndexOrZero() == u32_ParameterElementIndex)))
                              {
                                 c_NewMap[c_NewId] = C_PuiSdLastKnownHalElementId(u32_Hash, rc_Dp.c_Name.c_str());
                                 orc_MapCurToNew[c_ItCur->first] = c_NewId;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      //Store new map
      orc_LastKnownHalcCrcs = c_NewMap;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeDataPoolListElementIdSyncUtil::C_PuiSdNodeDataPoolListElementIdSyncUtil()
{
}

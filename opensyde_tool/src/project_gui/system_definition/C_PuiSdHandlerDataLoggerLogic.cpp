//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief        Data logger logic part of UI system definition

    Data logger logic part of UI system definition

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMap>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandlerDataLoggerLogic.hpp"
#include "C_PuiSdNodeDataPoolListElementIdSyncUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
/*! \brief  Get data logger job

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index

   \return
   Data logger job
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscDataLoggerJob * C_PuiSdHandlerDataLoggerLogic::GetDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                                           const uint32_t ou32_DataLoggerJobIndex) const
{
   const C_OscDataLoggerJob * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         pc_Retval = &rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data logger data element

   \param[in]  ou32_NodeIndex                   Node index
   \param[in]  ou32_DataLoggerJobIndex          Data logger job index
   \param[in]  ou32_DataLoggerDataElementIndex  Data logger data element index

   \return
   Data logger data element
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscDataLoggerDataElementReference * C_PuiSdHandlerDataLoggerLogic::GetDataLoggerDataElement(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
   const uint32_t ou32_DataLoggerDataElementIndex) const
{
   const C_OscDataLoggerDataElementReference * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         const C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         if (ou32_DataLoggerDataElementIndex < rc_DataLoggerJob.c_ConfiguredDataElements.size())
         {
            pc_Retval = &rc_DataLoggerJob.c_ConfiguredDataElements[ou32_DataLoggerDataElementIndex];
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data logger active

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index
   \param[in]  oq_Enabled                 Enabled

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::SetDataLoggerEnabled(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_DataLoggerJobIndex,
                                                            const bool oq_Enabled)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         rc_DataLoggerJob.q_IsEnabled = oq_Enabled;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data logger properties

   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataLoggerJobIndex   Data logger job index
   \param[in,out]  orc_Data                  Data

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::SetDataLoggerPropertiesWithoutInterfaceChanges(const uint32_t ou32_NodeIndex,
                                                                                      const uint32_t ou32_DataLoggerJobIndex,
                                                                                      const stw::opensyde_core::C_OscDataLoggerJobProperties & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         rc_DataLoggerJob.c_Properties = orc_Data;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data logger properties

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_DataLoggerJobIndex       Data logger job index
   \param[in]  oe_ConnectedInterfaceType     Connected interface type
   \param[in]  ou8_ConnectedInterfaceNumber  Connected interface number

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::SetDataLoggerInterface(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_DataLoggerJobIndex,
                                                              const C_OscSystemBus::E_Type oe_ConnectedInterfaceType,
                                                              const uint8_t ou8_ConnectedInterfaceNumber)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         rc_DataLoggerJob.c_Properties.e_ConnectedInterfaceType = oe_ConnectedInterfaceType;
         rc_DataLoggerJob.c_Properties.u8_ConnectedInterfaceNumber = ou8_ConnectedInterfaceNumber;
         this->m_HandlePossibleRouteChange();
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add data logger element

   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataLoggerJobIndex   Data logger job index
   \param[in,out]  orc_Data                  Data

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::AddDataLoggerElement(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_DataLoggerJobIndex,
                                                            const stw::opensyde_core::C_OscDataLoggerDataElementReference & orc_Data)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         rc_DataLoggerJob.c_ConfiguredDataElements.push_back(orc_Data);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data logger element

   \param[in]  ou32_NodeIndex                   Node index
   \param[in]  ou32_DataLoggerJobIndex          Data logger job index
   \param[in]  ou32_DataLoggerDataElementIndex  Data logger data element index

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::DeleteDataLoggerElement(const uint32_t ou32_NodeIndex,
                                                               const uint32_t ou32_DataLoggerJobIndex,
                                                               const uint32_t ou32_DataLoggerDataElementIndex)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         if (ou32_DataLoggerDataElementIndex < rc_DataLoggerJob.c_ConfiguredDataElements.size())
         {
            rc_DataLoggerJob.c_ConfiguredDataElements.erase(
               rc_DataLoggerJob.c_ConfiguredDataElements.begin() + ou32_DataLoggerDataElementIndex);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set data logger element properties

   \param[in]  ou32_NodeIndex                   Node index
   \param[in]  ou32_DataLoggerJobIndex          Data logger job index
   \param[in]  ou32_DataLoggerDataElementIndex  Data logger data element index
   \param[in]  oq_UseCustomName                 Use custom name
   \param[in]  orc_CustomName                   Custom name

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::SetDataLoggerElementProperties(const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_DataLoggerJobIndex,
                                                                      const uint32_t ou32_DataLoggerDataElementIndex,
                                                                      const bool oq_UseCustomName,
                                                                      const stw::scl::C_SclString & orc_CustomName)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      if (ou32_DataLoggerJobIndex < rc_Node.c_DataLoggerJobs.size())
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[ou32_DataLoggerJobIndex];
         if (ou32_DataLoggerDataElementIndex < rc_DataLoggerJob.c_ConfiguredDataElements.size())
         {
            C_OscDataLoggerDataElementReference & rc_ConfiguredDataElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[ou32_DataLoggerDataElementIndex];
            rc_ConfiguredDataElement.q_UseCustomName = oq_UseCustomName;
            rc_ConfiguredDataElement.c_CustomName = orc_CustomName;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC CRCs

   \return
   Last known HALC crcs
*/
//----------------------------------------------------------------------------------------------------------------------
const std::map<C_OscNodeDataPoolListElementOptArrayId,
               C_PuiSdLastKnownHalElementId> & C_PuiSdHandlerDataLoggerLogic::GetLastKnownHalcCrcs(void) const
{
   return this->mc_LastKnownHalcCrcs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check and handle new element

   \param[in]  orc_NewId   New id

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerDataLoggerLogic::CheckAndHandleNewElement(const C_OscNodeDataPoolListElementOptArrayId & orc_NewId)
{
   const int32_t s32_Retval = C_PuiSdNodeDataPoolListElementIdSyncUtil::h_CheckAndHandleNewElement(orc_NewId,
                                                                                                   this->mc_LastKnownHalcCrcs);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerDataLoggerLogic::C_PuiSdHandlerDataLoggerLogic(QObject * const opc_Parent) :
   C_PuiSdHandlerCanOpenLogic(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add last known HAL CRC

   \param[in]  orc_Id   Id
   \param[in]  orc_Crc  Crc
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_AddLastKnownHalcCrc(const C_OscNodeDataPoolListElementOptArrayId & orc_Id,
                                                          const C_PuiSdLastKnownHalElementId & orc_Crc)
{
   this->mc_LastKnownHalcCrcs[orc_Id] = orc_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node deleted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDeleted()
{
   m_HandlePossibleRouteChange();
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDeleted();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for bus deleted

   \param[in]  ou32_BusIndex  Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncBusDeleted(const uint32_t ou32_BusIndex)
{
   m_HandlePossibleRouteChange();
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncBusDeleted(ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node interface changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeInterfaceChanged()
{
   m_HandlePossibleRouteChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node interface deleted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeInterfaceDeleted()
{
   m_HandlePossibleRouteChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node routing settings changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeRoutingSettingsChanged()
{
   m_HandlePossibleRouteChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node added

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeAdded(const uint32_t ou32_Index)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAdded(ou32_Index);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAdded(rc_DataLoggerElement.c_ConfiguredElementId,
                                                                        ou32_Index);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync node halc

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeHalc(const uint32_t ou32_Index)
{
   std::map<C_OscNodeDataPoolListElementOptArrayId, C_OscNodeDataPoolListElementOptArrayId> c_MapCurToNew;
   C_PuiSdNodeDataPoolListElementIdSyncUtil::h_GetNewMapOnSyncHalc(ou32_Index, c_MapCurToNew,
                                                                   this->mc_LastKnownHalcCrcs);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeHalc(rc_DataLoggerElement.c_ConfiguredElementId,
                                                                           ou32_Index,
                                                                           c_MapCurToNew) == true)
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeHalc(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync node replace

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeReplace(const uint32_t ou32_Index)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeReplace(ou32_Index);
   m_HandleNodeAboutToBeDeleted(ou32_Index, true);
   m_HandlePossibleRouteChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node about to be deleted

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeAboutToBeDeleted(const uint32_t ou32_Index)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeleted(ou32_Index);
   m_HandleNodeAboutToBeDeleted(ou32_Index);
   // HALC
   C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAboutToBeDeleted(this->mc_LastKnownHalcCrcs, ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool added

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_DataPoolIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolAdded(ou32_NodeIndex, ou32_DataPoolIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAdded(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolIndex);
         }
      }
   }
   // HALC
   C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAdded(this->mc_LastKnownHalcCrcs, ou32_NodeIndex,
                                                                       ou32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool moved

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolSourceIndex   Data pool source index
   \param[in]  ou32_DataPoolTargetIndex   Data pool target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex,
                                                                  const uint32_t ou32_DataPoolSourceIndex,
                                                                  const uint32_t ou32_DataPoolTargetIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                                             ou32_DataPoolTargetIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolMoved(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolSourceIndex, ou32_DataPoolTargetIndex);
         }
      }
   }
   // HALC
   C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolMoved(this->mc_LastKnownHalcCrcs, ou32_NodeIndex,
                                                                       ou32_DataPoolSourceIndex,
                                                                       ou32_DataPoolTargetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool about to be deleted

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DataPoolIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAboutToBeDeleted(
                   rc_DataLoggerElement.c_ConfiguredElementId,
                   ou32_NodeIndex, ou32_DataPoolIndex))
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
   // HALC
   C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolAboutToBeDeleted(this->mc_LastKnownHalcCrcs,
                                                                                  ou32_NodeIndex, ou32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list added

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_DataPoolIndex,
                                                                      const uint32_t ou32_ListIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListAdded(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list moved

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  ou32_ListSourceIndex    List source index
   \param[in]  ou32_ListTargetIndex    List target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_DataPoolIndex,
                                                                      const uint32_t ou32_ListSourceIndex,
                                                                      const uint32_t ou32_ListTargetIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListMoved(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                 ou32_ListSourceIndex, ou32_ListTargetIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListMoved(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex, ou32_ListTargetIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list about to be deleted

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                                 const uint32_t ou32_DataPoolIndex,
                                                                                 const uint32_t ou32_ListIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                            ou32_ListIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListAboutToBeDeleted(
                   rc_DataLoggerElement.c_ConfiguredElementId,
                   ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex))
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list element added

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DataPoolIndex,
                                                                             const uint32_t ou32_ListIndex,
                                                                             const uint32_t ou32_ElementIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListElementAdded(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                        ou32_ListIndex, ou32_ElementIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementAdded(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list element moved

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_ElementSourceIndex    Element source index
   \param[in]  ou32_ElementTargetIndex    Element target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DataPoolIndex,
                                                                             const uint32_t ou32_ListIndex,
                                                                             const uint32_t ou32_ElementSourceIndex,
                                                                             const uint32_t ou32_ElementTargetIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListElementMoved(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                        ou32_ListIndex, ou32_ElementSourceIndex,
                                                                        ou32_ElementTargetIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size(); ++u32_ItDataLoggerElement)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListElementMoved(
               rc_DataLoggerElement.c_ConfiguredElementId,
               ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementSourceIndex, ou32_ElementTargetIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node data pool list element about to be deleted

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                                        const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                                                                        const uint32_t ou32_ElementIndex)
{
   C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                                   ou32_ListIndex, ou32_ElementIndex);
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeDataPoolListAboutToBeDeleted(
                   rc_DataLoggerElement.c_ConfiguredElementId,
                   ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex))
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle node about to be deleted

   \param[in]  ou32_Index           Index
   \param[in]  oq_OnlyMarkInvalid   Only mark invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandleNodeAboutToBeDeleted(const uint32_t ou32_Index,
                                                                 const bool oq_OnlyMarkInvalid)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (C_PuiSdNodeDataPoolListElementIdSyncUtil::h_OnSyncNodeAboutToBeDeleted(
                   rc_DataLoggerElement.c_ConfiguredElementId,
                   ou32_Index, oq_OnlyMarkInvalid))
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle possible route change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerDataLoggerLogic::m_HandlePossibleRouteChange()
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItDataLogger = 0UL; u32_ItDataLogger < rc_Node.c_DataLoggerJobs.size(); ++u32_ItDataLogger)
      {
         QMap<uint32_t, bool> c_MapNodeReachable;
         C_OscDataLoggerJob & rc_DataLoggerJob = rc_Node.c_DataLoggerJobs[u32_ItDataLogger];
         for (uint32_t u32_ItDataLoggerElement = 0UL;
              u32_ItDataLoggerElement < rc_DataLoggerJob.c_ConfiguredDataElements.size();)
         {
            C_OscDataLoggerDataElementReference & rc_DataLoggerElement =
               rc_DataLoggerJob.c_ConfiguredDataElements[u32_ItDataLoggerElement];
            if (!c_MapNodeReachable.contains(rc_DataLoggerElement.c_ConfiguredElementId.u32_NodeIndex))
            {
               c_MapNodeReachable[rc_DataLoggerElement.c_ConfiguredElementId.u32_NodeIndex] =
                  C_PuiSdUtil::h_CheckDataLoggerNodeReachable(u32_ItNode, u32_ItDataLogger,
                                                              rc_DataLoggerElement.c_ConfiguredElementId.u32_NodeIndex);
            }
            if (!c_MapNodeReachable[rc_DataLoggerElement.c_ConfiguredElementId.u32_NodeIndex])
            {
               rc_DataLoggerJob.c_ConfiguredDataElements.erase(
                  rc_DataLoggerJob.c_ConfiguredDataElements.begin() + u32_ItDataLoggerElement);
            }
            else
            {
               ++u32_ItDataLoggerElement;
            }
         }
      }
   }
}

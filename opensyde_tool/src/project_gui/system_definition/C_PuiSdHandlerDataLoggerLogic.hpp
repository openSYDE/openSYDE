//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger logic part of UI system definition
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERDATALOGGERLOGIC_HPP
#define C_PUISDHANDLERDATALOGGERLOGIC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerCanOpenLogic.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerDataLoggerLogic :
   public C_PuiSdHandlerCanOpenLogic
{
public:
   const stw::opensyde_core::C_OscDataLoggerJob * GetDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                                   const uint32_t ou32_DataLoggerJobIndex) const;
   const stw::opensyde_core::C_OscDataLoggerDataElementReference * GetDataLoggerDataElement(
      const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
      const uint32_t ou32_DataLoggerDataElementIndex) const;

   int32_t AddDataLogger(const uint32_t ou32_NodeIndex, const stw::opensyde_core::C_OscDataLoggerJob & orc_Data,
                         const QString * const opc_Name = NULL);
   int32_t DeleteDataLogger(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex);
   int32_t SetDataLoggerEnabled(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                const bool oq_Enabled);
   int32_t SetDataLoggerProperties(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                   const stw::scl::C_SclString & orc_Name, const stw::scl::C_SclString & orc_Comment,
                                   const stw::opensyde_core::C_OscDataLoggerJobProperties::E_UseCase oe_UseCase,
                                   const stw::opensyde_core::C_OscDataLoggerJobProperties::E_LogFileFormat oe_LogFileFormat, const uint32_t ou32_MaxLogEntries, const uint32_t ou32_MaxLogDurationSec, const uint32_t ou32_LogIntervalMs, const stw::opensyde_core::C_OscDataLoggerJobProperties::E_LocalLogTrigger oe_LocalLogTrigger, const stw::scl::C_SclString & orc_LogDestinationDirectory);
   int32_t SetDataLoggerAdditionalTriggerProperties(const uint32_t ou32_NodeIndex,
                                                    const uint32_t ou32_DataLoggerJobIndex,
                                                    const stw::opensyde_core::C_OscDataLoggerJobAdditionalTriggerProperties & orc_Data);
   int32_t SetXappInterface(const uint32_t ou32_NodeIndex,
                            const opensyde_core::C_OscSystemBus::E_Type oe_ConnectedInterfaceType,
                            const uint8_t ou8_ConnectedInterfaceNumber);
   int32_t SetXappProperties(const uint32_t ou32_NodeIndex, const uint32_t ou32_PollingIntervalMs,
                             const uint32_t ou32_DataRequestIntervalMs);

   int32_t AddDataLoggerElement(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                const stw::opensyde_core::C_OscDataLoggerDataElementReference & orc_Data);
   int32_t DeleteDataLoggerElement(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                   const uint32_t ou32_DataLoggerDataElementIndex);
   int32_t SetDataLoggerElementProperties(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                          const uint32_t ou32_DataLoggerDataElementIndex, const bool oq_UseCustomName,
                                          const stw::scl::C_SclString & orc_CustomName);

   const std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                  C_PuiSdLastKnownHalElementId> & GetLastKnownHalcCrcs(void) const;
   int32_t CheckAndHandleNewElement(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_NewId);

protected:
   C_PuiSdHandlerDataLoggerLogic(QObject * const opc_Parent);

   void m_AddLastKnownHalcCrc(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id,
                              const C_PuiSdLastKnownHalElementId & orc_Crc);

   void m_HandleSyncNodeDeleted(void) override;
   void m_HandleSyncBusDeleted(const uint32_t ou32_BusIndex) override;
   void m_HandleSyncNodeInterfaceChanged(void) override;
   void m_HandleSyncNodeInterfaceDeleted(void) override;
   void m_HandleSyncNodeRoutingSettingsChanged(void) override;
   void m_HandleSyncNodeAdded(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeHalc(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeReplace(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeAboutToBeDeleted(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) override;
   void m_HandleSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                      const uint32_t ou32_DataPoolTargetIndex) override;
   void m_HandleSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                 const uint32_t ou32_DataPoolIndex) override;
   void m_HandleSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                          const uint32_t ou32_ListIndex) override;
   void m_HandleSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                          const uint32_t ou32_ListSourceIndex,
                                          const uint32_t ou32_ListTargetIndex) override;
   void m_HandleSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                     const uint32_t ou32_ListIndex) override;
   void m_HandleSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                 const uint32_t ou32_ListIndex,
                                                 const uint32_t ou32_ElementIndex) override;
   void m_HandleSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                 const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex,
                                                 const uint32_t ou32_ElementTargetIndex) override;
   void m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_DataPoolIndex,
                                                            const uint32_t ou32_ListIndex,
                                                            const uint32_t ou32_ElementIndex) override;
   void m_HandleSyncNodeDataPoolListElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_DataPoolIndex,
                                                              const uint32_t ou32_ListIndex,
                                                              const uint32_t ou32_ElementIndex,
                                                              const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type, const bool oq_IsArray, const uint32_t ou32_ArraySize,
                                                              const bool oq_IsString) override;
   void m_HandleSyncNodeDataPoolListElementRangeChanged(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_DataPoolIndex,
                                                        const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                        const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement, const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement)
   override;

private:
   void m_HandleNodeAboutToBeDeleted(const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid = false);
   void m_HandlePossibleRouteChange(void);
   void m_ValidateAllRoutesForOneNode(const uint32_t ou32_Index);
   static std::map<stw::scl::C_SclString, bool> mh_GetExistingDataLoggerNames(
      const stw::opensyde_core::C_OscNode & orc_Node);
   static void mh_HandleSyncDataLoggerElementAboutToBeDeleted(
      stw::opensyde_core::C_OscDataLoggerJobProperties & orc_Properties,
      const stw::opensyde_core::C_OscDataLoggerDataElementReference & orc_Element);
   static bool mh_CheckIdRelevantForSync(const stw::opensyde_core::C_OscDataLoggerJobProperties & orc_Properties);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

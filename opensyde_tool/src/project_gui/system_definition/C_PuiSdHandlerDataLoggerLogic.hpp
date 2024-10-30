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

   int32_t SetDataLoggerProperties(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex,
                                   const stw::opensyde_core::C_OscDataLoggerJobProperties & orc_Data);

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

private:
   void m_HandleNodeAboutToBeDeleted(const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

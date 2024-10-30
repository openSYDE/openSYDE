//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data part of UI system definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERDATA_HPP
#define C_PUISDHANDLERDATA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.hpp"

#include "C_PuiSdBus.hpp"
#include "C_PuiSdNode.hpp"
#include "C_PuiBsElements.hpp"
#include "C_PuiSdTextElementBus.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_PuiSdSharedDatapools.hpp"
#include "C_SdTopologyDataSnapshot.hpp"
#include "C_PuiSdLastKnownHalElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerData :
   public QObject
{
   Q_OBJECT

public:
   int32_t LoadFromFile(const stw::scl::C_SclString & orc_Path, uint16_t * const opu16_FileVersion);
   int32_t SaveToFile(const stw::scl::C_SclString & orc_Path, const bool oq_UseDeprecatedFileFormatV2,
                      const bool oq_UpdateInternalState = true);
   bool HasHashChanged(void) const;
   uint32_t CalcHashSystemDefinition(void) const;
   virtual void Clear(const bool oq_TriggerSyncSignals);
   C_SdTopologyDataSnapshot GetSnapshot(void) const;

   stw::opensyde_core::C_OscSystemDefinition & GetOscSystemDefinition(void);
   const stw::opensyde_core::C_OscSystemDefinition & GetOscSystemDefinitionConst(void) const;

   static QString h_AutomaticCeStringAdaptation(const QString & orc_Input);

   C_PuiBsElements c_Elements;
   std::vector<C_PuiSdTextElementBus> c_BusTextElements; ///< UI information for bus text elements

protected:
   stw::opensyde_core::C_OscSystemDefinition mc_CoreDefinition; ///< Core part of system definition data

   std::vector<C_PuiSdNode> mc_UiNodes; ///< UI information for nodes (should always stay synchronous with
   ///< core arrays)
   std::vector<C_PuiSdBus> mc_UiBuses; ///< UI information for buses (should always stay synchronous with
   ///< core arrays)

   C_PuiSdSharedDatapools mc_SharedDatapools; ///< UI information for shared datapools
   std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
            C_PuiSdLastKnownHalElementId> mc_LastKnownHalcCrcs; ///< HAL data element info
   uint32_t mu32_CalculatedHashSystemDefinition;

   C_PuiSdHandlerData(QObject * const opc_Parent = NULL);

   static int32_t mh_SortMessagesByName(stw::opensyde_core::C_OscNode & orc_OscNode, C_PuiSdNode & orc_UiNode);
   uint32_t m_GetHashNode(const uint32_t ou32_NodeIndex) const;
   uint32_t m_GetHashBus(const uint32_t ou32_BusIndex) const;
   virtual void m_HandleSyncNodeAdded(const uint32_t ou32_Index);
   virtual void m_HandleSyncNodeHalc(const uint32_t ou32_Index);
   virtual void m_HandleSyncNodeReplace(const uint32_t ou32_Index);
   virtual void m_HandleSyncNodeAboutToBeDeleted(const uint32_t ou32_Index);
   virtual void m_HandleSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   virtual void m_HandleSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                              const uint32_t ou32_DataPoolTargetIndex);
   virtual void m_HandleSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_DataPoolIndex);
   virtual void m_HandleSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                  const uint32_t ou32_ListIndex);
   virtual void m_HandleSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                  const uint32_t ou32_ListSourceIndex,
                                                  const uint32_t ou32_ListTargetIndex);
   virtual void m_HandleSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_DataPoolIndex,
                                                             const uint32_t ou32_ListIndex);
   virtual void m_HandleSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_DataPoolIndex,
                                                         const uint32_t ou32_ListIndex,
                                                         const uint32_t ou32_ElementIndex);
   virtual void m_HandleSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_DataPoolIndex,
                                                         const uint32_t ou32_ListIndex,
                                                         const uint32_t ou32_ElementSourceIndex,
                                                         const uint32_t ou32_ElementTargetIndex);
   virtual void m_HandleSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                                    const uint32_t ou32_DataPoolIndex,
                                                                    const uint32_t ou32_ListIndex,
                                                                    const uint32_t ou32_ElementIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodesChanged(void);
   void SigNodeChanged(const uint32_t ou32_Index);
   void SigBussesChanged(void);
   void SigBusChanged(const uint32_t ou32_Index);
   void SigSyncNodeAdded(const uint32_t ou32_Index);
   void SigSyncNodeHalc(const uint32_t ou32_Index);
   void SigSyncNodeReplace(const uint32_t ou32_Index);
   void SigSyncNodeAboutToBeDeleted(const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid);
   void SigSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                 const uint32_t ou32_DataPoolTargetIndex);
   void SigSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigSyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigSyncNodeApplicationMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                    const uint32_t ou32_DataPoolTargetIndex);
   void SigSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void SigSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_NodeIndex,
                                                               const uint32_t ou32_DataBlockIndex);
   void SigSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataBlockIndex);
   void SigSyncNodeApplicationResultPathSizeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataBlockIndex,
                                                    const uint32_t ou32_OldSize, const uint32_t ou32_NewSize);
   void SigSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                     const uint32_t ou32_ListIndex);
   void SigSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                     const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex);
   void SigSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                const uint32_t ou32_ListIndex);
   void SigSyncNodeDataPoolListDataSetAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                            const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetIndex);
   void SigSyncNodeDataPoolListDataSetMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                            const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetSourceIndex,
                                            const uint32_t ou32_DataSetTargetIndex);
   void SigSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                       const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetIndex);
   void SigSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                            const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   void SigSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                            const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex,
                                            const uint32_t ou32_ElementTargetIndex);
   void SigSyncNodeDataPoolListElementTypeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                  const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                  const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type,
                                                  const bool oq_IsArray, const uint32_t ou32_ArraySize,
                                                  const bool oq_IsString);
   void SigSyncNodeDataPoolListElementRangeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                   const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                   const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement,
                                                   const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement);
   void SigSyncNodeDataPoolListElementAccessChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                    const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                    const stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access oe_Access);
   void SigSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                       const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   void SigSyncBusAdded(const uint32_t ou32_Index);
   void SigSyncBusDeleted(const uint32_t ou32_Index);
   void SigSyncClear(void);

private:
   void m_FixNameIssues(void);
   void m_FixAddressIssues(void);
   void m_FixCommInconsistencyErrors(void);

   int32_t m_VerifyLoadedSystemDefintion(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

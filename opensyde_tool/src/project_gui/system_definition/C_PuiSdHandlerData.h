//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data part of UI system definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERDATA_H
#define C_PUISDHANDLERDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.h"

#include "C_PuiSdBus.h"
#include "C_PuiSdNode.h"
#include "C_PuiBsElements.h"
#include "C_PuiSdTextElementBus.h"
#include "C_OSCSystemDefinition.h"
#include "C_PuiSdSharedDatapools.h"
#include "C_SdTopologyDataSnapshot.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerData :
   public QObject
{
   Q_OBJECT

public:
   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_Path, stw_types::uint16 * const opu16_FileVersion);
   stw_types::sint32 SaveToFile(const stw_scl::C_SCLString & orc_Path, const bool oq_UseDeprecatedFileFormatV2);
   bool HasHashChanged(void) const;
   stw_types::uint32 CalcHashSystemDefinition(void) const;
   void Clear(void);
   C_SdTopologyDataSnapshot GetSnapshot(void) const;

   stw_opensyde_core::C_OSCSystemDefinition & GetOSCSystemDefinition(void);
   const stw_opensyde_core::C_OSCSystemDefinition & GetOSCSystemDefinitionConst(void) const;

   static QString h_AutomaticCStringAdaptation(const QString & orc_Input, const bool oq_ReplaceAllCharacters);

   C_PuiBsElements c_Elements;
   std::vector<C_PuiSdTextElementBus> c_BusTextElements; ///< UI information for bus text elements

protected:
   stw_opensyde_core::C_OSCSystemDefinition mc_CoreDefinition; ///< Core part of system definition data

   std::vector<C_PuiSdNode> mc_UINodes; ///< UI information for nodes (should always stay synchronous with
   ///< core arrays)
   std::vector<C_PuiSdBus> mc_UIBuses; ///< UI information for buses (should always stay synchronous with
   ///< core arrays)

   C_PuiSdSharedDatapools mc_SharedDatapools; ///< UI information for shared datapools
   stw_types::uint32 mu32_CalculatedHashSystemDefinition;

   C_PuiSdHandlerData(QObject * const opc_Parent = NULL);

   static stw_types::sint32 mh_SortMessagesByName(stw_opensyde_core::C_OSCNode & orc_OSCNode, C_PuiSdNode & orc_UiNode);
   stw_types::uint32 m_GetHashNode(const stw_types::uint32 ou32_NodeIndex) const;
   stw_types::uint32 m_GetHashBus(const stw_types::uint32 ou32_BusIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodesChanged(void);
   void SigBussesChanged(void);
   void SigSyncNodeAdded(const stw_types::uint32 ou32_Index);
   void SigSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index);
   void SigSyncNodeDataPoolAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeDataPoolMoved(const stw_types::uint32 ou32_NodeIndex,
                                 const stw_types::uint32 ou32_DataPoolSourceIndex,
                                 const stw_types::uint32 ou32_DataPoolTargetIndex);
   void SigSyncNodeDataPoolAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                    const stw_types::uint32 ou32_DataPoolSourceIndex,
                                    const stw_types::uint32 ou32_DataPoolTargetIndex);
   void SigSyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeDataPoolListAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                     const stw_types::uint32 ou32_ListIndex);
   void SigSyncNodeDataPoolListMoved(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                     const stw_types::uint32 ou32_ListSourceIndex,
                                     const stw_types::uint32 ou32_ListTargetIndex);
   void SigSyncNodeDataPoolListAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_DataPoolIndex,
                                                const stw_types::uint32 ou32_ListIndex);
   void SigSyncNodeDataPoolListDataSetAdded(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_DataSetIndex);
   void SigSyncNodeDataPoolListDataSetMoved(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_DataSetSourceIndex,
                                            const stw_types::uint32 ou32_DataSetTargetIndex);
   void SigSyncNodeDataPoolListDataSetAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                       const stw_types::uint32 ou32_ListIndex,
                                                       const stw_types::uint32 ou32_DataSetIndex);
   void SigSyncNodeDataPoolListElementAdded(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_ElementIndex);
   void SigSyncNodeDataPoolListElementMoved(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_ElementSourceIndex,
                                            const stw_types::uint32 ou32_ElementTargetIndex);
   void SigSyncNodeDataPoolListElementChanged(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_DataPoolIndex,
                                              const stw_types::uint32 ou32_ListIndex,
                                              const stw_types::uint32 ou32_ElementIndex,
                                              const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type,
                                              const bool oq_IsArray, const stw_types::uint32 ou32_ArraySize);
   void SigSyncNodeDataPoolListElementAccessChanged(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex,
                                                    const stw_types::uint32 ou32_ElementIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access oe_Access);
   void SigSyncNodeDataPoolListElementAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                       const stw_types::uint32 ou32_ListIndex,
                                                       const stw_types::uint32 ou32_ElementIndex);
   void SigSyncBusAdded(const stw_types::uint32 ou32_Index);
   void SigSyncBusDeleted(const stw_types::uint32 ou32_Index);
   void SigSyncClear(void);

private:
   void m_FixNameIssues(void);
   void m_FixAddressIssues(void);
   void m_FixCommInconsistencyErrors(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

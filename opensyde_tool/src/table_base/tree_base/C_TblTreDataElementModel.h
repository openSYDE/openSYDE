//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREEMODEL_H
#define C_SYVDAPEDATAELEMENTTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>

#include "C_OSCNode.h"
#include "C_TblTreItem.h"
#include "C_TblTreModel.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_TblTreDataElementModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   enum E_Mode
   {
      eNVM_LIST,
      eDATAPOOLS,
      eDATAPOOL_ELEMENT,
      eBUS_SIGNAL
   };

   explicit C_TblTreDataElementModel(QObject * const opc_Parent = NULL);
   ~C_TblTreDataElementModel(void);

   void InitSD(const stw_types::uint32 ou32_NodeIndex, const stw_types::sint32 os32_SkipApplicationIndex,
               const std::vector<stw_types::uint32> & orc_UsedDataPoolIndicesIndex);
   void InitSV(const stw_types::uint32 ou32_ViewIndex, const E_Mode oe_Mode, const bool oq_ShowOnlyWriteElements,
               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetDataElements(
      const QModelIndex & orc_Index)
   const;
   static void h_CleanUp(void);

   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //Generic interface
   QModelIndex GetIndexForItem(const std::vector<stw_types::uint32> & orc_ItemIndices) const;
   std::vector<stw_types::uint32> GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex) const;

private:
   class C_TblTreDataElementModelState
   {
   public:
      C_TblTreDataElementModelState(C_TblTreSimpleItem * const opc_Tree);
      C_TblTreSimpleItem * pc_Tree;

      void CleanUp(void);
   };

   E_Mode me_Mode;
   static QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> mhc_ViewSetupsNL;
   static QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> mhc_ViewSetupsDE;
   static QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> mhc_ViewSetupsBS;
   static const QString mhc_IconNode;
   static const QString mhc_IconDatapool;
   static const QString mhc_IconList;
   static const QString mhc_IconVariable;
   static const QString mhc_IconParameter;
   static const QString mhc_IconSignal;
   static const QString mhc_IconEthernet;
   static const QString mhc_IconCAN;
   static const QString mhc_IconMessage;
   static const QString mhc_IconHALCInput;
   static const QString mhc_IconHALCOutput;
   static const QString mhc_IconHALCMisc;
   static const QString mhc_AdditionalDataPoolInfo;

   void m_CleanUpLastModel(void);
   static C_PuiSvDbNodeDataPoolListElementId mh_Translate(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Indices,
      const stw_types::uint32 ou32_SignalIndex, const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType);
   void m_InitBusSignal(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                        const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                        const bool oq_Show64BitValues,
                        const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void m_InitDatapoolElements(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                               const bool oq_Show64BitValues,
                               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsHALC(C_TblTreItem * const opc_DpItem, bool & orq_HALCValid,
                                           const stw_opensyde_core::C_OSCNode & orc_Node,
                                           const stw_opensyde_core::C_OSCNodeDataPool & orc_Dp,
                                           const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DpIndex,
                                           const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                           const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsHALCConfig(C_TblTreItem * const opc_DpItem, bool & orq_HALCValid,
                                                 const stw_opensyde_core::C_OSCHalcDefDomain & orc_DomainDef,
                                                 const stw_opensyde_core::C_OSCHalcConfigChannel & orc_ChannelConfig,
                                                 const stw_opensyde_core::C_OSCHalcDefChannelValues & orc_Values,
                                                 const std::vector<stw_opensyde_core::C_OSCHalcDefChannelDef> & orc_Channels, const stw_opensyde_core::C_OSCNodeDataPool & orc_Dp, const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DpIndex, const stw_types::uint32 ou32_ChannelIndex, const stw_types::uint32 ou32_ChannelArrayIndex, stw_types::uint32 & oru32_CounterParam, stw_types::uint32 & oru32_CounterInput, stw_types::uint32 & oru32_CounterOutput, stw_types::uint32 & oru32_CounterStatus, const stw_types::uint32 ou32_RelevantChannelNumber, const bool oq_ChanNumVarNecessary, const bool oq_UseCaseVarNecessary, const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues, const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsHALCConfigList(C_TblTreItem * const opc_ChannelItem, bool & orq_ChannelValid,
                                                     const std::vector<stw_opensyde_core::C_OSCHalcDefStruct> & orc_Values, const stw_opensyde_core::C_OSCNodeDataPoolList & orc_List, const stw_types::uint32 ou32_ChannelArrayIndex, const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DpIndex, const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_UseCase, stw_types::uint32 & oru32_Counter, const stw_types::uint32 ou32_RelevantChannelNumber, const bool oq_ChanNumVarNecessary, const bool oq_UseCaseVarNecessary, const bool oq_IsParam, const stw_scl::C_SCLString & orc_DomainSingularName, const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues, const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, const QString & orc_HalChannelOrDomainName);
   static void mh_AddHALCItem(C_TblTreItem * const opc_BaseItem,
                              const stw_opensyde_core::C_OSCHalcDefElement & orc_HALCItem,
                              const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DpIndex,
                              const stw_types::uint32 ou32_ListIndex, stw_types::uint32 & oru32_Counter,
                              const stw_types::uint32 ou32_RelevantChannelNumber,
                              const stw_types::uint32 ou32_ChannelArrayIndex, const bool oq_ShowOnlyWriteElements,
                              const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                              const bool oq_Show64BitValues,
                              const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                              const QString & orc_HalChannelOrDomainName);
   static void mh_AddHALCTreeItem(C_TblTreItem * const opc_ListItem, const QString & orc_Name,
                                  const C_PuiSvDbNodeDataPoolListElementId & orc_Id, const bool oq_IsArray,
                                  const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                  const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                  const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsCOMM(C_TblTreItem * const opc_DpItem, bool & orq_COMValid,
                                           const stw_opensyde_core::C_OSCNode & orc_Node,
                                           const stw_opensyde_core::C_OSCNodeDataPool & orc_Dp,
                                           const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DpIndex,
                                           const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                           const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static bool mh_AddCOMMMessageItems(C_TblTreItem * const opc_BaseItem, const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> &
                                      orc_MessageIds, const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType,
                                      const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                      const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_UpdateDatapoolElement(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                        const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                        const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, C_TblTreSimpleItem * const opc_Tree);
   static void mh_CreateArrayElementNodes(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                          const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                          const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_Element,
                                          const bool oq_IsStringElement, C_TblTreItem * const opc_ElementItem,
                                          const C_PuiSvDbNodeDataPoolListElementId & orc_ParentId,
                                          const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void m_InitNvmList(const stw_types::uint32 ou32_ViewIndex);
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetDatapools(const QModelIndex & orc_Index)
   const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetAnyDatapoolElements(
      const QModelIndex & orc_Index) const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetNvmList(const QModelIndex & orc_Index)
   const;

   static void mh_CleanUp(QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> & orc_Map);
   static bool mh_Contains(const QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> & orc_Map,
                           const C_TblTreSimpleItem * const opc_Item);
   static std::vector<stw_types::uint32> mh_GetViewSdHash(const stw_types::uint32 ou32_ViewIndex);
   static bool mh_CheckNodeDiagnostic(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

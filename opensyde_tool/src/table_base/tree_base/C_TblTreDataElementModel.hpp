//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREEMODEL_HPP
#define C_SYVDAPEDATAELEMENTTREEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>

#include "C_OscNode.hpp"
#include "C_TblTreItem.hpp"
#include "C_TblTreModel.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   ~C_TblTreDataElementModel(void) override;

   void InitSd(const uint32_t ou32_NodeIndex, const int32_t os32_SkipApplicationIndex,
               const std::vector<uint32_t> & orc_UsedDataPoolIndicesIndex);
   void InitSv(const uint32_t ou32_ViewIndex, const E_Mode oe_Mode, const bool oq_ShowOnlyWriteElements,
               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void InitSdDatapoolElements(const uint32_t ou32_SdDataLoggerUseCaseNodeIndex,
                               const uint32_t ou32_SdDataLoggerUseCaseDataLoggerIndex,
                               const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                               const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);

   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetDataElements(
      const QModelIndex & orc_Index)
   const;
   static void h_CleanUp(void);

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //Generic interface
   QModelIndex GetIndexForItem(const std::vector<uint32_t> & orc_ItemIndices) const;
   std::vector<uint32_t> GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex) const;

   void CleanUpLastModel(void);

private:
   class C_TblTreDataElementModelState
   {
   public:
      C_TblTreDataElementModelState(C_TblTreSimpleItem * const opc_Tree);
      C_TblTreSimpleItem * pc_Tree;

      void CleanUp(void);
   };

   E_Mode me_Mode;
   static QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> mhc_ViewSetupsNl;
   static QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> mhc_ViewSetupsDe;
   static QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> mhc_ViewSetupsBs;
   static const QString mhc_ICON_NODE;
   static const QString mhc_ICON_DATAPOOL;
   static const QString mhc_ICON_LIST;
   static const QString mhc_ICON_VARIABLE;
   static const QString mhc_ICON_PARAMETER;
   static const QString mhc_ICON_SIGNAL;
   static const QString mhc_ICON_ETHERNET;
   static const QString mhc_ICON_CAN;
   static const QString mhc_ICON_MESSAGE;
   static const QString mhc_ICON_HALC_INPUT;
   static const QString mhc_ICON_HALC_OUTPUT;
   static const QString mhc_ICON_HALC_MISC;
   static const QString mhc_ADDITIONAL_DATA_POOL_INFO;

   static C_PuiSvDbNodeDataPoolListElementId mh_Translate(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Indices,
      const uint32_t ou32_SignalIndex, const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType);
   void m_InitBusSignal(const uint32_t ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                        const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                        const bool oq_Show64BitValues,
                        const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void m_InitDatapoolElements(const uint32_t ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                               const bool oq_Show64BitValues,
                               const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                               const bool oq_IsModelUsedInSysViews = true,
                               const uint32_t ou32_SdDataLoggerUseCaseNodeIndex = 0UL,
                               const uint32_t ou32_SdDataLoggerUseCaseDataLoggerIndex = 0UL);
   static void mh_InitDatapoolElementsHalc(C_TblTreItem * const opc_DpItem, bool & orq_HalcValid,
                                           const stw::opensyde_core::C_OscNode & orc_Node,
                                           const stw::opensyde_core::C_OscNodeDataPool & orc_Dp,
                                           const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex,
                                           const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                           const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsHalcConfig(C_TblTreItem * const opc_DpItem, bool & orq_HalcValid,
                                                 const stw::opensyde_core::C_OscHalcDefDomain & orc_DomainDef,
                                                 const stw::opensyde_core::C_OscHalcConfigDomain & orc_DomainConfig,
                                                 const stw::opensyde_core::C_OscHalcConfigChannel & orc_ChannelConfig,
                                                 const stw::opensyde_core::C_OscHalcDefChannelValues & orc_Values,
                                                 const stw::opensyde_core::C_OscNodeDataPool & orc_Dp,
                                                 const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex,
                                                 const uint32_t ou32_ChannelIndex,
                                                 const uint32_t ou32_ChannelArrayIndex, uint32_t & oru32_CounterParam,
                                                 uint32_t & oru32_CounterInput, uint32_t & oru32_CounterOutput,
                                                 uint32_t & oru32_CounterStatus,
                                                 const uint32_t ou32_RelevantChannelNumber,
                                                 const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                                 const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                                 const stw::opensyde_core::C_OscHalcMagicianDatapoolListHandler & orc_DpHandler, const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsHalcConfigList(C_TblTreItem * const opc_ChannelItem, bool & orq_ChannelValid,
                                                     const std::vector<stw::opensyde_core::C_OscHalcDefStruct> & orc_Values, const stw::opensyde_core::C_OscNodeDataPoolList & orc_List, const uint32_t ou32_ChannelArrayIndex, const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex, const uint32_t ou32_ListIndex, const uint32_t ou32_UseCase, uint32_t & oru32_Counter, const uint32_t ou32_RelevantChannelNumber, const bool oq_ChanNumVarNecessary, const bool oq_UseCaseVarNecessary, const bool oq_SafetyFlagVarNecessary, const bool oq_IsParam, const stw::scl::C_SclString & orc_DomainSingularName, const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues, const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, const QString & orc_HalChannelOrDomainName);
   static void mh_AddHalcItem(C_TblTreItem * const opc_BaseItem,
                              const stw::opensyde_core::C_OscHalcDefElement & orc_HalcItem,
                              const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex, const uint32_t ou32_ListIndex,
                              uint32_t & oru32_Counter, const uint32_t ou32_RelevantChannelNumber,
                              const uint32_t ou32_ChannelArrayIndex, const bool oq_ShowOnlyWriteElements,
                              const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements,
                              const bool oq_Show64BitValues,
                              const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements,
                              const QString & orc_HalChannelOrDomainName);
   static void mh_AddHalcTreeItem(C_TblTreItem * const opc_ListItem, const QString & orc_Name,
                                  const C_PuiSvDbNodeDataPoolListElementId & orc_Id, const bool oq_IsArray,
                                  const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                  const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                  const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_InitDatapoolElementsComm(C_TblTreItem * const opc_DpItem, bool & orq_ComValid,
                                           const stw::opensyde_core::C_OscNode & orc_Node,
                                           const stw::opensyde_core::C_OscNodeDataPool & orc_Dp,
                                           const uint32_t ou32_NodeIndex, const uint32_t ou32_DpIndex,
                                           const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                           const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                           const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static bool mh_AddCommMessageItems(C_TblTreItem * const opc_BaseItem, const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> &
                                      orc_MessageIds, const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_IdType,
                                      const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                      const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                      const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   static void mh_UpdateDatapoolElement(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                        const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                        const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, C_TblTreSimpleItem * const opc_Tree);
   static void mh_CreateArrayElementNodes(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                          const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                          const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_Element,
                                          const bool oq_IsStringElement, C_TblTreItem * const opc_ElementItem,
                                          const C_PuiSvDbNodeDataPoolListElementId & orc_ParentId,
                                          const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void m_InitNvmList(const uint32_t ou32_ViewIndex);
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetDatapools(
      const QModelIndex & orc_Index)
   const;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetAnyDatapoolElements(
      const QModelIndex & orc_Index) const;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetNvmList(const QModelIndex & orc_Index)
   const;

   static void mh_CleanUp(QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> & orc_Map);
   static bool mh_Contains(const QMap<std::vector<uint32_t>, C_TblTreDataElementModelState> & orc_Map,
                           const C_TblTreSimpleItem * const opc_Item);
   static std::vector<uint32_t> mh_GetViewSdHash(const uint32_t ou32_ViewIndex);
   static bool mh_SvCheckNodeDiagnostic(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

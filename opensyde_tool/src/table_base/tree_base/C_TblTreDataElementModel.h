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
               const bool oq_ShowArrayElements, const bool oq_Show64BitValues);
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetDataElements(
      const QModelIndex & orc_Index)
   const;
   static void h_CleanUp(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -restore

   //Generic interface
   QModelIndex GetIndexForItem(const std::vector<stw_types::uint32> & orc_ItemIndices) const;
   std::vector<stw_types::uint32> GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex) const;

private:
   class C_TblTreDataElementModelState
   {
   public:
      C_TblTreDataElementModelState(C_TblTreItem * const opc_Tree,
                                    const std::vector<C_PuiSdNodeCanMessageSyncManager *> & orc_SyncManagers);
      C_TblTreItem * pc_Tree;
      std::vector<C_PuiSdNodeCanMessageSyncManager *> c_SyncManagers;

      void CleanUp(void);
   };

   std::vector<C_PuiSdNodeCanMessageSyncManager *> mc_MessageSyncManagers;
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
   static const QString mhc_AdditionalDataPoolInfo;
   static const QString mhc_AdditionalWriteOnlyInfo;
   static const QString mhc_AdditionalArrayInfo;
   static const QString mhc_Additional64BitInfo;

   void m_ClearSyncManagers(void);
   void m_CleanUpLastModel(void);
   static C_PuiSvDbNodeDataPoolListElementId mh_Translate(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Indices,
      const stw_types::uint32 ou32_SignalIndex);
   void m_InitBusSignal(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                        const bool oq_ShowArrayElements, const bool oq_Show64BitValues);
   void m_InitDatapoolElement(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
                              const bool oq_ShowArrayElements, const bool oq_Show64BitValues);
   void m_UpdateDatapoolElement(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                const bool oq_Show64BitValues);
   static void mh_ConfigureDatapoolElement(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                           const bool oq_Show64BitValues,
                                           const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_Element,
                                           C_TblTreItem * const opc_ElementItem);
   void m_InitNvmList(const stw_types::uint32 ou32_ViewIndex);
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetBusSignals(
      const QModelIndex & orc_Index)
   const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetDatapools(const QModelIndex & orc_Index)
   const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetDatapoolElements(
      const QModelIndex & orc_Index) const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> m_GetNvmList(const QModelIndex & orc_Index)
   const;

   static void mh_CleanUp(QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> & orc_Map);
   static bool mh_Contains(const QMap<std::vector<stw_types::uint32>, C_TblTreDataElementModelState> & orc_Map,
                           const C_TblTreItem * const opc_Item);
   static std::vector<stw_types::uint32> mh_GetViewSdHash(const stw_types::uint32 ou32_ViewIndex);
   static bool mh_CheckNodeDiagnostic(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_NodeIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

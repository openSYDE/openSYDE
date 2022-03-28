//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter tree model (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPATREEMODEL_H
#define C_SYVDAITPATREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreItem.h"
#include "C_TblTreModel.h"
#include "C_PuiSvDbDataElementHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaTreeModel :
   public C_TblTreModel
{
public:
   enum E_Columns
   {
      eTREE = 0,
      eCOMMENT,
      eDEVICE_VALUE,
      eSET,
      eUNIT,
      eACTION_READ,
      eACTION_WRITE,
      eACTION_APPLY,
      eACTION_LOAD,
      eACTION_SAVE,
      eACTION_RECORD,
      eACTION_REMOVE
   };

   C_SyvDaItPaTreeModel(QObject * const opc_Parent = NULL);
   ~C_SyvDaItPaTreeModel(void);

   bool IsEmpty(void) const;
   void ReloadSetValues(void);
   void SetDark(const bool oq_Value);
   void SetEditMode(const bool oq_EditMode);
   void SetConnected(const bool oq_Connected);
   void SetLoadSaveActive(const bool oq_Active);
   void SetActionActive(const bool oq_Active);
   void DeleteSpecified(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void ClearECUValues(void);
   void ApplyEcuValues(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds,
                       std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds,
                       std::vector<QString> & orc_InvalidValues, std::vector<QString> & orc_NewValues);
   bool CheckRange(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds,
                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds2) const;
   bool CheckAllListsRead(void) const;
   void PrepareChangedValues(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds) const;
   void RemoveValuesChangedFlag(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds)
   const;

   QModelIndexList GetAllAvailableIndixesForOneColumn(void) const;
   QModelIndex GetIndexForItem(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                               const stw_types::uint32 & oru32_ValidLayers) const;
   void Init(stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget);
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetAllListIds(void) const;
   bool CheckListsRead(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds) const;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetChangedListElementIds(void) const;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> GetInvalidListIds(void) const;
   void UpdateECUValues(void);
   void SetCRCStatus(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_ListId, const bool oq_Status);
   void GetListSetValues(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ListId,
                         std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_ListValues) const;

   // Header:
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);
   static void h_DecodeIndex(const QModelIndex & orc_Index, stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                             stw_types::uint32 & oru32_ValidLayers);
   stw_types::uint32 GetParamIndexId(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   static void h_AppendOnlyUniqueListId(std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_Vec,
                                        const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ListId);
   static QString h_GetSelectedItemTypeTemplate(const QModelIndex & orc_Index);
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetListIdsForId(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id, const stw_types::uint32 ou32_ValidLayers)
   const;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetElementIdsForId(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id, const stw_types::uint32 ou32_ValidLayers)
   const;

   static void h_AdaptFloatRangeOfValueAndAppendResults(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                                        const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_DescriptionId, std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds, std::vector<QString> & orc_InvalidValues, std::vector<QString> & orc_NewValues);
   static void h_AdaptFloatRangeOfValue(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                        const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_Description);
   static void h_FixInvalidFloatValue(stw_types::float64 & orf64_Value);

private:
   C_PuiSvDbDataElementHandler * mpc_DataWidget;
   bool mq_DarkMode;
   bool mq_EditMode;
   bool mq_Connected;
   bool mq_SaveLoadActive;
   bool mq_ActionActive;
   std::vector<std::vector<QString> > mc_ECUValuesString;
   std::vector<bool> mc_ECUValuesReadStatus;
   std::vector<bool> mc_ECUCRCValidStatus;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> mc_ECUValues;

   static const QString mhc_ICON_ALL_NODE;
   static const QString mhc_ICON_NODE;
   static const QString mhc_ICON_DATAPOOL;
   static const QString mhc_ICON_LIST;
   static const QString mhc_ICON_PARAMETER;
   static const QString mhc_ECU_VALUE_INIT_STRING;

   static void mh_InitAllNode(C_TblTreItem * const opc_TreeNode, const stw_types::uint32 ou32_ViewIndex);
   static void mh_InitNode(C_TblTreItem * const opc_TreeNode, const stw_types::uint32 ou32_NodeIndex,
                           const stw_types::uint32 ou32_ViewIndex);
   static void mh_InitDataPool(C_TblTreItem * const opc_TreeNode, const stw_types::uint32 ou32_NodeIndex,
                               const stw_types::uint32 ou32_DataPoolIndex, const bool oq_Enabled);
   static void mh_InitList(C_TblTreItem * const opc_TreeNode, const stw_types::uint32 ou32_NodeIndex,
                           const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex,
                           const bool oq_Enabled);
   static void mh_InitElement(C_TblTreItem * const opc_TreeNode, const stw_types::uint32 ou32_NodeIndex,
                              const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex,
                              const stw_types::uint32 ou32_ElementIndex, const bool oq_Enabled);
   const stw_opensyde_core::C_OSCNodeDataPoolContent * m_GetSetValue(const stw_types::uint32 ou32_DataIndex) const;
   bool m_IsChanged(const QModelIndex & orc_Index) const;
   bool m_CheckListCRCIsChanged(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   bool m_CheckElementIsChanged(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   bool m_GetListIndex(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                       stw_types::uint32 & oru32_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

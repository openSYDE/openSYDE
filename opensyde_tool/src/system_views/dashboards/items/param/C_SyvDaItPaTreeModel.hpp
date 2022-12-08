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
#include "C_TblTreItem.hpp"
#include "C_TblTreModel.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   ~C_SyvDaItPaTreeModel(void) override;

   bool IsEmpty(void) const;
   void ReloadSetValues(void);
   void SetDark(const bool oq_Value);
   void SetEditMode(const bool oq_EditMode);
   void SetConnected(const bool oq_Connected);
   void SetLoadSaveActive(const bool oq_Active);
   void SetActionActive(const bool oq_Active);
   void DeleteSpecified(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);
   void ClearEcuValues(void);
   void ApplyEcuValues(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds,
                       std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_InvalidValueIds,
                       std::vector<QString> & orc_InvalidValues, std::vector<QString> & orc_NewValues);
   bool CheckRange(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds,
                   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds2) const;
   bool CheckAllListsRead(void) const;
   void PrepareChangedValues(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds) const;
   void RemoveValuesChangedFlag(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds)
   const;

   QModelIndexList GetAllAvailableIndixesForOneColumn(void) const;
   QModelIndex GetIndexForItem(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                               const uint32_t & oru32_ValidLayers) const;
   void Init(stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget);
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> GetAllListIds(void) const;
   bool CheckListsRead(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds) const;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> GetChangedListElementIds(void) const;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> GetInvalidListIds(void) const;
   void UpdateEcuValues(void);
   void SetCrcStatus(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_ListId, const bool oq_Status);
   void GetListSetValues(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ListId,
                         std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_ListValues) const;

   // Header:
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);
   static void h_DecodeIndex(const QModelIndex & orc_Index, stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                             uint32_t & oru32_ValidLayers);
   uint32_t GetParamIndexId(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   static void h_AppendOnlyUniqueListId(std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_Vec,
                                        const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ListId);
   static QString h_GetSelectedItemTypeTemplate(const QModelIndex & orc_Index);
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> GetListIdsForId(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id, const uint32_t ou32_ValidLayers)
   const;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> GetElementIdsForId(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id, const uint32_t ou32_ValidLayers)
   const;

   static void h_AdaptFloatRangeOfValueAndAppendResults(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                                        const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_DescriptionId, std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_InvalidValueIds, std::vector<QString> & orc_InvalidValues, std::vector<QString> & orc_NewValues);
   static void h_AdaptFloatRangeOfValue(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                        const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_Description);
   static void h_FixInvalidFloatValue(float64_t & orf64_Value);

private:
   C_PuiSvDbDataElementHandler * mpc_DataWidget;
   bool mq_DarkMode;
   bool mq_EditMode;
   bool mq_Connected;
   bool mq_SaveLoadActive;
   bool mq_ActionActive;
   std::vector<std::vector<QString> > mc_EcuValuesString;
   std::vector<bool> mc_EcuValuesReadStatus;
   std::vector<bool> mc_EcuCrcValidStatus;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> mc_EcuValues;

   static const QString mhc_ICON_ALL_NODE;
   static const QString mhc_ICON_NODE;
   static const QString mhc_ICON_DATAPOOL;
   static const QString mhc_ICON_LIST;
   static const QString mhc_ICON_PARAMETER;
   static const QString mhc_ECU_VALUE_INIT_STRING;

   static void mh_InitAllNode(C_TblTreItem * const opc_TreeNode, const uint32_t ou32_ViewIndex);
   static void mh_InitNode(C_TblTreItem * const opc_TreeNode, const uint32_t ou32_NodeIndex,
                           const uint32_t ou32_ViewIndex);
   static void mh_InitDataPool(C_TblTreItem * const opc_TreeNode, const uint32_t ou32_NodeIndex,
                               const uint32_t ou32_DataPoolIndex, const bool oq_Enabled);
   static void mh_InitList(C_TblTreItem * const opc_TreeNode, const uint32_t ou32_NodeIndex,
                           const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex, const bool oq_Enabled);
   static void mh_InitElement(C_TblTreItem * const opc_TreeNode, const uint32_t ou32_NodeIndex,
                              const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                              const uint32_t ou32_ElementIndex, const bool oq_Enabled);
   const stw::opensyde_core::C_OscNodeDataPoolContent * m_GetSetValue(const uint32_t ou32_DataIndex) const;
   bool m_IsChanged(const QModelIndex & orc_Index) const;
   bool m_CheckListCrcIsChanged(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   bool m_CheckElementIsChanged(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   bool m_GetListIndex(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id, uint32_t & oru32_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

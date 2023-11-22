//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTAMODEL_HPP
#define C_SYVDAITTAMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblModelAction.hpp"
#include <QIcon>
#include "stwtypes.hpp"
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItTaModel :
   public C_TblModelAction
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eICON = 0,
      eNAME,
      eVALUE,
      eBAR
   };

   explicit C_SyvDaItTaModel(stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_Data,
                             QObject * const opc_Parent = NULL);

   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);

   void InitMinMaxAndName(void);
   void UpdateValue(void);
   void UpdateError(void);
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value);
   void GetUniqueRows(const QModelIndexList & orc_Indices, std::vector<uint32_t> & orc_ItemIndices) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   //Common interface
   void CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const override;
   void MoveItems(const QModelIndexList & orc_Indices, const bool oq_Up);

   // Change data count:
   uint32_t AddItem(const QModelIndexList & orc_Indices,
                    const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   void RemoveItems(const QModelIndexList & orc_Indices,
                    std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements);

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

   const C_PuiSvDbNodeDataPoolListElementId * GetDataPoolElementIndex(const uint32_t ou32_Index) const;

   static const uint32_t hu32_MAX_ELEMENTS;

protected:
   uint32_t m_AddNewItem(const uint32_t ou32_SelectedIndex) override;
   std::vector<uint32_t> m_PasteItems(const uint32_t ou32_SelectedIndex) override;
   uint32_t m_GetSizeItems(void) const override;
   void m_DeleteItem(const uint32_t ou32_Index) override;
   void m_BeginRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex) override;
   void m_EndRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex) override;
   void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) override;

private:
   stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_AddDataPoolElementId;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> mc_RemovedDataPoolElementIds;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_Data;
   std::vector<std::vector<QString> > mc_ScaledDisplayDataValues;
   std::vector<std::vector<float64_t> > mc_UnscaledLastDataValues;
   std::vector<std::vector<float64_t> > mc_UnscaledMinValues;
   std::vector<std::vector<float64_t> > mc_UnscaledMaxValues;
   std::vector<uint32_t> mc_ArrayItemIndex;
   std::vector<QString> mc_Names;
   std::vector<QString> mc_Units;
   std::vector<int32_t> mc_Transparency;
   std::vector<bool> mc_InterpretAsStringFlags;
   std::vector<bool> mc_ShowPercentage;
   QString mc_IconParameter;
   QString mc_IconSignal;
   QString mc_IconVariable;
   QString mc_IconHalInput;
   QString mc_IconHalOutput;
   QString mc_IconHalOther;
   QString mc_IconParameterWarning;
   QString mc_IconSignalWarning;
   QString mc_IconVariableWarning;
   QString mc_IconHalInputWarning;
   QString mc_IconHalOutputWarning;
   QString mc_IconHalOtherWarning;
   QString mc_IconParameterError;
   QString mc_IconSignalError;
   QString mc_IconVariableError;
   QString mc_IconHalInputError;
   QString mc_IconHalOutputError;
   QString mc_IconHalOtherError;

   QColor mc_TextColor;

   static C_PuiSvDbNodeDataElementConfig mh_GetConfigForNewItem(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId m_RemoveItem(const uint32_t ou32_Index,
                                                                            std::vector<C_PuiSvDbNodeDataElementConfig> & orc_AdaptedItems);
   float32_t m_GetPercentage(const uint32_t ou32_Index) const;
   QString m_GetValue(const uint32_t ou32_Index) const;
   static std::vector<uint32_t> mh_GetSelectedRows(const QModelIndexList & orc_Indices);
   void m_InitMinMaxAndNameForOneRow(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                     const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig);
   void m_InitValuesForOneRow(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                              const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig,
                              const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscElement,
                              const stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement & orc_UiElement);
   void m_InitStartValueForOneRow(const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig,
                                  const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscElement,
                                  const stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement & orc_UiElement);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

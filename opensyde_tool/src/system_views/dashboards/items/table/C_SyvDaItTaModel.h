//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTAMODEL_H
#define C_SYVDAITTAMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblModelAction.h"
#include <QIcon>
#include "stwtypes.h"
#include "C_PuiSvDbWidgetBase.h"
#include "C_PuiSvDbDataElementHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   explicit C_SyvDaItTaModel(stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_Data,
                             QObject * const opc_Parent = NULL);

   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);

   void InitMinMaxAndName(void);
   void UpdateValue(void);
   void UpdateError(void);
   void UpdateTransparence(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value);
   void GetUniqueRows(const QModelIndexList & orc_Indices, std::vector<stw_types::uint32> & orc_ItemIndices) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   //lint -restore

   //Common interface
   virtual void CopySelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices) const override;
   void MoveItems(const QModelIndexList & orc_Indices, const bool oq_Up);

   // Change data count:
   stw_types::uint32 AddItem(const QModelIndexList & orc_Indices,
                             const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   void RemoveItems(const QModelIndexList & orc_Indices,
                    std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements);

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);

   const C_PuiSvDbNodeDataPoolListElementId * GetDataPoolElementIndex(const stw_types::uint32 ou32_Index) const;

   static const stw_types::uint32 hu32_MaxElements;

protected:
   virtual stw_types::uint32 m_AddNewItem(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual std::vector<stw_types::uint32> m_PasteItems(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual stw_types::uint32 m_GetSizeItems(void) const override;
   virtual void m_DeleteItem(const stw_types::uint32 ou32_Index) override;
   virtual void m_BeginRemoveRows(const stw_types::uint32 ou32_FirstIndex, const stw_types::uint32 ou32_LastIndex);
   virtual void m_EndRemoveRows(const stw_types::uint32 ou32_FirstIndex, const stw_types::uint32 ou32_LastIndex);
   virtual void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) override;

private:
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_AddDataPoolElementId;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> mc_RemovedDataPoolElementIds;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_Data;
   std::vector<std::vector<stw_types::float64> > mc_UnscaledLastDataValues;
   std::vector<std::vector<stw_types::float64> > mc_UnscaledMinValues;
   std::vector<std::vector<stw_types::float64> > mc_UnscaledMaxValues;
   std::vector<stw_types::uint32> mc_ArrayItemIndex;
   std::vector<QString> mc_Names;
   std::vector<QString> mc_Units;
   std::vector<stw_types::sintn> mc_Transparency;
   std::vector<bool> mc_InterpretAsStringFlags;
   std::vector<bool> mc_ShowPercentage;
   QString mc_IconParameter;
   QString mc_IconSignal;
   QString mc_IconVariable;
   QString mc_IconParameterWarning;
   QString mc_IconSignalWarning;
   QString mc_IconVariableWarning;
   QString mc_IconParameterError;
   QString mc_IconSignalError;
   QString mc_IconVariableError;

   QColor mc_TextColor;

   static C_PuiSvDbNodeDataElementConfig mh_GetConfigForNewItem(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId m_RemoveItem(const stw_types::uint32 ou32_Index,
                                                                           std::vector<C_PuiSvDbNodeDataElementConfig> & orc_AdaptedItems);
   stw_types::float32 GetPercentage(const stw_types::uint32 ou32_Index) const;
   QString GetValue(const stw_types::uint32 ou32_Index) const;
   static std::vector<stw_types::uint32> mh_GetSelectedRows(const QModelIndexList & orc_Indices);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

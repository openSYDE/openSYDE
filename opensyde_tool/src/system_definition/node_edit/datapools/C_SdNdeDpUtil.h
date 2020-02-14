//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool util class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPUTIL_H
#define C_SDNDEDPUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QItemSelection>
#include <QRect>
#include <QPainter>
#include <QWidget>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpUtil
{
public:
   enum E_ListDataChangeType
   {
      eLIST_NAME,
      eLIST_COMMENT,
      eLIST_SIZE,
      eLIST_CRC
   };

   enum E_DataSetDataChangeType
   {
      eDATA_SET_NAME,
      eDATA_SET_COMMENT
   };

   enum E_ElementDataChangeType
   {
      eELEMENT_NAME,
      eELEMENT_COMMENT,
      eELEMENT_VALUE_TYPE,
      eELEMENT_ARRAY,
      eELEMENT_AUTO_MIN_MAX,
      eELEMENT_MIN,
      eELEMENT_MAX,
      eELEMENT_FACTOR,
      eELEMENT_OFFSET,
      eELEMENT_UNIT,
      eELEMENT_DATA_SET,
      eELEMENT_ACCESS,
      eELEMENT_EVENT_CALL
   };

   enum E_ArrayEditType
   {
      eARRAY_EDIT_MIN,
      eARRAY_EDIT_MAX,
      eARRAY_EDIT_DATA_SET
   };

   static bool h_CheckConnected(const std::vector<stw_types::uint32> & orc_Items);
   static std::vector<stw_types::uint32> h_ConvertVector(const QModelIndexList & orc_Items,
                                                         const bool & orq_Rows = true);
   static void h_ConvertToElementGeneric(const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCElement,
                                         const C_PuiSdNodeDataPoolListElement & orc_UIElement,
                                         const E_ElementDataChangeType & ore_Type, QVariant & orc_Generic,
                                         const stw_types::uint32 & oru32_Index,
                                         const stw_types::sint32 & ors32_DataSetIndex);
   static stw_types::sint8 h_ConvertContentTypeToComboBox(
      const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type & ore_Type);
   static stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type h_ConvertContentTypeFromComboBox(
      const stw_types::sint8 & ors8_Type);
   static stw_types::sint8 h_ConvertElementAccessToComboBox(
      const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access & ore_Type);
   static stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access h_ConvertElementAccesFromComboBox(
      const stw_types::sint8 & ors8_Type);
   static QString h_ConvertContentTypeToString(const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type & ore_Type);
   static QString h_ConvertElementAccessToString(
      const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access & ore_Type);
   static QWidget * h_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                          const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                          const stw_types::uint32 & oru32_ArrayIndex, const bool oq_UseParamVariant);
   static void h_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index);
   static void h_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                             const QModelIndex & orc_Index);
   static QString h_ConvertToString(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Data);
   static void h_SetString(const QString & orc_String, stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Output);
   static QString h_ConvertToString(const std::vector<stw_types::sint8> & orc_Data);
   static void h_DrawTableBoolean(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index, const QPixmap & orc_CheckMark,
                                  const bool & orq_IsEditing);
   static void h_DrawTableBackground(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                     const bool & orq_UseSpecialBackgroundColor);
   static void h_DrawTableBackgroundGeneric(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                            const QColor & orc_Default, const QColor & orc_Alternate,
                                            const QColor & orc_Selected);
   static void h_DrawTableSelected(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                   const QModelIndex & orc_Index, const QAbstractTableModel * const opc_TableModel);
   static bool h_CompareSpecifiedItemSmaller(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content1,
                                             const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content2,
                                             const stw_types::uint32 & oru32_Index);
   static stw_types::sint32 h_GetTableSize(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex,
                                           const stw_types::uint32 ou32_ListIndex,
                                           const stw_types::sintn osn_MaximumHeight = 0);

   static stw_types::sint32 GetSharedDatapoolGroup(const stw_types::uint32 ou32_SharedDatapoolGroup,
                                                   const stw_opensyde_core::C_OSCNodeDataPoolId & orc_BaseDatapoolId,
                                                   const stw_types::uint32 ou32_NodeIndex,
                                                   std::vector<QString> & orc_SharedDatapoolNameGroup);

private:
   C_SdNdeDpUtil();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

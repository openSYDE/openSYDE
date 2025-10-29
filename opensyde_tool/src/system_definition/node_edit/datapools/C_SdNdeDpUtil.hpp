//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool util class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPUTIL_HPP
#define C_SDNDEDPUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QItemSelection>
#include <QPainter>
#include <QWidget>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscNodeDataPoolId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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

   static std::vector<uint32_t> h_ConvertVector(const QModelIndexList & orc_Items, const bool & orq_Rows = true);
   static void h_ConvertToElementGeneric(const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscElement,
                                         const C_PuiSdNodeDataPoolListElement & orc_UiElement,
                                         const E_ElementDataChangeType & ore_Type, QVariant & orc_Generic,
                                         const uint32_t & oru32_Index, const int32_t & ors32_DataSetIndex);
   static int8_t h_ConvertContentTypeToComboBox(const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type & ore_Type);
   static stw::opensyde_core::C_OscNodeDataPoolContent::E_Type h_ConvertContentTypeFromComboBox(
      const int8_t & ors8_Type);
   static int8_t h_ConvertElementAccessToComboBox(
      const stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access & ore_Type);
   static stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access h_ConvertElementAccesFromComboBox(
      const int8_t & ors8_Type);
   static QString h_ConvertContentTypeToString(const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type & ore_Type);
   static QString h_ConvertElementAccessToString(
      const stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access & ore_Type);
   static QWidget * h_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                          const float64_t of64_Factor, const float64_t of64_Offset,
                                          const uint32_t & oru32_ArrayIndex, const bool oq_UseParamVariant);
   static void h_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index);
   static void h_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                             const QModelIndex & orc_Index);
   static void h_DrawTableBoolean(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index, const QPixmap & orc_CheckMark,
                                  const bool & orq_IsEditing);
   static void h_DrawTableBackground(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                     const bool & orq_UseSpecialBackgroundColor);
   static void h_DrawTableBackgroundGeneric(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                            const QColor & orc_Default, const QColor & orc_Alternate,
                                            const QColor & orc_Selected);
   static bool h_CompareSpecifiedItemSmaller(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content1,
                                             const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content2,
                                             const uint32_t & oru32_Index);
   static int32_t h_GetTableSize(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                 const uint32_t ou32_ListIndex, const int32_t os32_MaximumHeight = 0);
   static int32_t h_GetSharedDatapoolGroup(const uint32_t ou32_SharedDatapoolGroup,
                                           const stw::opensyde_core::C_OscNodeDataPoolId & orc_BaseDatapoolId,
                                           const uint32_t ou32_NodeIndex,
                                           std::vector<QString> & orc_SharedDatapoolNameGroup);
   static int32_t h_GetNextDiagOrNvmDpIndex(const uint32_t ou32_NodeIndex, const uint32_t ou32_DatapoolIndex,
                                            const bool oq_Forwards);

private:
   C_SdNdeDpUtil();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

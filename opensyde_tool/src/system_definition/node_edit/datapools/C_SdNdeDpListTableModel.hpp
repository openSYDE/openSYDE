//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEMODEL_HPP
#define C_SDNDEDPLISTTABLEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpListTableErrorManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListTableModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eINVALID = 0,
      eINDEX,
      eICON,
      eNAME,
      eCOMMENT,
      eVALUE_TYPE,
      eARRAY_SIZE,
      eAUTO_MIN_MAX,
      eMIN,
      eMAX,
      eFACTOR,
      eOFFSET,
      eUNIT,
      eDATA_SET,
      eACCESS,
      eDATA_SIZE,
      eADDRESS,
      eEVENT_CALL,
      eUNKNOWN
   };

   explicit C_SdNdeDpListTableModel(QObject * const opc_Parent = NULL);

   void SetList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                const uint32_t & oru32_ListIndex);
   bool Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
               const uint32_t & oru32_ListIndex) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   Qt::DropActions supportedDropActions(void) const override;

   // Add data:
   std::vector<std::vector<uint32_t> > DoInsertRows(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscInsertedElements,
      const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiInsertedElements,
      const std::vector<uint32_t> & orc_Rows);
   bool insertColumns(const int32_t os32_Col, const int32_t os32_Count,
                      const QModelIndex & orc_Parent = QModelIndex()) override;
   // Remove data:
   void DoRemoveRows(const std::vector<uint32_t> & orc_Rows);
   bool removeColumns(const int32_t os32_Col, const int32_t os32_Count,
                      const QModelIndex & orc_Parent = QModelIndex()) override;

   //Move data:
   void DoMoveRows(const std::vector<uint32_t> & orc_SelectedIndices, const std::vector<uint32_t> & orc_TargetIndices);

   E_Columns ColumnToEnum(const int32_t & ors32_Column, int32_t * const ops32_DataSetIndex = NULL) const;
   int32_t EnumToColumn(const E_Columns & ore_Value) const;

   //Drag & drop
   QStringList mimeTypes() const override;
   QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;

   const stw::opensyde_core::C_OscNodeDataPoolListElement * GetOscElement(const QModelIndex & orc_Index) const;
   bool IsElementInterpretedAsString(const QModelIndex & orc_Index) const;
   void HandleDataChange(const uint32_t ou32_Row, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_ChangeType,
                         const int32_t os32_DataSetIndex);
   void DoInsertDataSet(const uint32_t ou32_DataSetIndex);
   void DoDeleteDataSet(const uint32_t ou32_DataSetIndex);
   void Reset(void);
   bool CheckName(const uint32_t ou32_Index, const QString & orc_Name) const;
   uint32_t GetArraySize(const uint32_t ou32_Index) const;
   bool IsString(const QModelIndex & orc_Index) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                               const uint32_t & oru32_DataPoolListIndex);
   void SigSizeChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex);
   void SigDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                      const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListElementIndex,
                      const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                      const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex);

private:
   class C_DpListTableDataSetData
   {
   public:
      QVariant c_DataSet;
      QVariant c_DataSetEdit;
      QString c_DataSetToolTipContent;
      QColor c_DataSetForeground;
   };

   class C_DpListTableData
   {
   public:
      QVariant c_Name;
      QColor c_NameForeground;
      QVariant c_Comment;

      QString c_ValueType;
      QVariant c_ValueTypeEdit;

      uint32_t u32_ArraySize;
      QVariant c_ArraySize;
      QVariant c_ArraySizeEdit;

      QVariant c_Min;
      QVariant c_MinEdit;
      QString c_MinToolTipContent;
      QVariant c_Max;
      QVariant c_MaxEdit;
      QString c_MaxToolTipContent;
      QFont c_MinMaxFont;        // Same font configuration for min and max col
      QColor c_MinMaxForeground; // Same color configuration for min and max col

      QVariant c_Factor;
      QVariant c_FactorEdit;
      QVariant c_Offset;
      QVariant c_OffsetEdit;
      QVariant c_Unit;

      QFont c_DataSetFont; // For all data sets
      std::vector<C_DpListTableDataSetData> c_DataSetData;

      QVariant c_Access;
      QVariant c_AccessEdit;
      QString c_DataSize;
      QString c_Address;

      QVariant c_InvalidToolTipContent;
      QVariant c_InvalidToolTipHeading;
      QStringList c_InvalidIconRole;

      bool q_AutoMinMaxActive;
      int32_t s32_AutoMinMaxCheckState;
      int32_t s32_EventCallCheckState;

      QStringList c_IconIconRole;
      bool q_InterpretAsString;
   };

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   std::vector<C_DpListTableData> mc_DpListInfoAll;
   bool mq_DpIsSafety;
   C_SdNdeDpListTableErrorManager mc_ErrorManager;

   void m_FillDpListInfo(void);
   void m_FillDpListElementInfo(const uint32_t ou32_ElementIndex);

   void m_DataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                     const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListElementIndex,
                     const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                     const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex);

   bool m_CheckLink(const opensyde_core::C_OscNodeDataPoolListElement * const opc_OscElement) const;
   void m_OnErrorChange(void);
   void m_MoveItems(const std::vector<uint32_t> & orc_ContiguousIndices, const uint32_t ou32_TargetIndex) const;
   void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) const;
   QString m_GetFloatAsString(const float64_t of64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

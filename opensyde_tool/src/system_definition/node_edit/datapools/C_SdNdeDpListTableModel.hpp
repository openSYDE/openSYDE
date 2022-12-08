//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEMODEL_H
#define C_SDNDEDPLISTTABLEMODEL_H

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
   const C_PuiSdNodeDataPoolListElement * GetUiElement(const QModelIndex & orc_Index) const;
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
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   C_SdNdeDpListTableErrorManager mc_ErrorManager;

   bool m_CheckLink(const QModelIndex & orc_Index) const;
   void m_OnErrorChange(void);
   void m_MoveItems(const std::vector<uint32_t> & orc_ContiguousIndices, const uint32_t ou32_TargetIndex) const;
   void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

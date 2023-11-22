//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list data set model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTDATASETMODEL_HPP
#define C_SDNDEDPLISTDATASETMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListDataSetModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   enum E_Rows
   {
      eNAME = 0,
      eCOMMENT
   };

   explicit C_SdNdeDpListDataSetModel(QObject * const opc_Parent = NULL);

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
   void DoInsertColumns(const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
                        const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues, const uint32_t & oru32_Row);
   bool insertColumns(const int32_t os32_Column, const int32_t os32_Count,
                      const QModelIndex & orc_Parent = QModelIndex()) override;

   // Remove data:
   bool removeColumns(const int32_t os32_Column, const int32_t os32_Count,
                      const QModelIndex & orc_Parent = QModelIndex()) override;

   // Move data
   bool moveColumns(const QModelIndex & orc_SourceParent, const int32_t os32_SourceColumn, const int32_t os32_Count,
                    const QModelIndex & orc_DestinationParent, const int32_t os32_DestinationChild) override;

   static E_Rows h_RowToEnum(const int32_t & ors32_Row);
   static int32_t h_EnumToRow(const E_Rows & ore_Value);

   //Drag & drop
   QStringList mimeTypes() const override;
   QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;

   void HandleDataChange(const uint32_t & oru32_Column, const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_ChangeType);
   void Reset(void);
   void HandleErrorChange(void);
   bool CheckName(const uint32_t & oru32_Index, const QString & orc_Name) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                               const uint32_t & oru32_DataPoolListIndex);
   void SigDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                      const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListDataSetIndex,
                      const QVariant & orc_NewData, const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void SigColumnCountChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                             const uint32_t & oru32_DataPoolListIndex, const int32_t & ors32_NewColumnCount);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> mc_OscNames;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > mc_OscDataSetValues;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

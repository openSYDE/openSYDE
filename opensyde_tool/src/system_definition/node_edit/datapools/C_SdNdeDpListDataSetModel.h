//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list data set model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTDATASETMODEL_H
#define C_SDNDEDPLISTDATASETMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_SdNdeDpUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   void SetList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_ListIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex) const;

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   virtual Qt::DropActions supportedDropActions(void) const override;

   // Add data:
   void DoInsertColumns(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                        const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues, const stw_types::uint32 & oru32_Row);
   virtual bool insertColumns(const stw_types::sintn osn_Column, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;

   // Remove data:
   virtual bool removeColumns(const stw_types::sintn osn_Column, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;

   // Move data
   virtual bool moveColumns(const QModelIndex & orc_SourceParent, const stw_types::sintn osn_SourceColumn,
                            const stw_types::sintn osn_Count, const QModelIndex & orc_DestinationParent,
                            const stw_types::sintn osn_DestinationChild) override;

   static E_Rows h_RowToEnum(const stw_types::sint32 & ors32_Row);
   static stw_types::sint32 h_EnumToRow(const E_Rows & ore_Value);

   //Drag & drop
   virtual QStringList mimeTypes() const override;
   virtual QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;

   void HandleDataChange(const stw_types::uint32 & oru32_Column,
                         const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_ChangeType);
   void Reset(void);
   void HandleErrorChange(void);
   bool CheckName(const stw_types::uint32 & oru32_Index, const QString & orc_Name) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                               const stw_types::uint32 & oru32_DataPoolListIndex);
   void SigDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                      const stw_types::uint32 & oru32_DataPoolListIndex,
                      const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                      const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void SigColumnCountChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                             const stw_types::uint32 & oru32_DataPoolListIndex,
                             const stw_types::sint32 & ors32_NewColumnCount);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> mc_OSCNames;
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > mc_OSCDataSetValues;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

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
#include "stwtypes.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListTableErrorManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   std::vector<std::vector<stw_types::uint32> > DoInsertRows(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCInsertedElements,
      const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UIInsertedElements,
      const std::vector<stw_types::uint32> & orc_Rows);
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   virtual bool insertColumns(const stw_types::sintn osn_Col, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;
   // Remove data:
   void DoRemoveRows(const std::vector<stw_types::uint32> & orc_Rows);
   virtual bool removeColumns(const stw_types::sintn osn_Col, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;

   //Move data:
   void DoMoveRows(const std::vector<stw_types::uint32> & orc_SelectedIndices,
                   const std::vector<stw_types::uint32> & orc_TargetIndices);

   E_Columns ColumnToEnum(const stw_types::sint32 & ors32_Column,
                          stw_types::sint32 * const ops32_DataSetIndex = NULL) const;
   stw_types::sint32 EnumToColumn(const E_Columns & ore_Value) const;

   //Drag & drop
   virtual QStringList mimeTypes() const override;
   virtual QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;

   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCElement(const QModelIndex & orc_Index) const;
   const C_PuiSdNodeDataPoolListElement * GetUIElement(const QModelIndex & orc_Index) const;
   void HandleDataChange(const stw_types::uint32 & oru32_Row,
                         const C_SdNdeDpUtil::E_ElementDataChangeType & ore_ChangeType,
                         const stw_types::sint32 & ors32_DataSetIndex);
   void DoInsertDataSet(const stw_types::uint32 & oru32_DataSetIndex);
   void DoDeleteDataSet(const stw_types::uint32 & oru32_DataSetIndex);
   void Reset(void);
   bool CheckName(const stw_types::uint32 & oru32_Index, const QString & orc_Name) const;
   stw_types::uint32 GetArraySize(const stw_types::uint32 & oru32_Index) const;
   bool IsString(const QModelIndex & orc_Index) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                               const stw_types::uint32 & oru32_DataPoolListIndex);
   void SigSizeChangePossible(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_DataPoolListIndex);
   void SigDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                      const stw_types::uint32 & oru32_DataPoolListIndex,
                      const stw_types::uint32 & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                      const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                      const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   C_SdNdeDpListTableErrorManager mc_ErrorManager;

   bool m_CheckLink(const QModelIndex & orc_Index) const;
   void m_OnErrorChange(void);
   void m_MoveItems(const std::vector<stw_types::uint32> & orc_ContiguousIndices,
                    const stw_types::uint32 ou32_TargetIndex) const;
   void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

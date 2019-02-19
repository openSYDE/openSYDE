//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table model (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEMODEL_H
#define C_SDNDEDATAPOOLLISTTABLEMODEL_H

/* -- Includes ------------------------------------------------------------- */

#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolListTableErrorManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListTableModel :
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

   explicit C_SdNdeDataPoolListTableModel(QObject * const opc_Parent = NULL);

   void SetList(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                const stw_types::uint32 & oru32_ListIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Header:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Editable:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   virtual Qt::DropActions supportedDropActions(void) const override;
   //lint -restore

   // Add data:
   std::vector<std::vector<stw_types::uint32> > DoInsertRows(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCInsertedElements,
      const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UIInsertedElements,
      const std::vector<stw_types::uint32> & orc_Rows);
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool insertColumns(const stw_types::sintn osn_Col, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;

   // Remove data:
   void DoRemoveRows(const std::vector<stw_types::uint32> & orc_Rows);
   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool removeColumns(const stw_types::sintn osn_Col, const stw_types::sintn osn_Count,
                              const QModelIndex & orc_Parent = QModelIndex()) override;

   //Move data:
   void DoMoveRows(const std::vector<stw_types::uint32> & orc_SelectedIndices,
                   const std::vector<stw_types::uint32> & orc_TargetIndices);

   //lint -restore

   E_Columns ColumnToEnum(const stw_types::sint32 & ors32_Column,
                          stw_types::sint32 * const opc_DataSetIndex = NULL) const;
   stw_types::sint32 EnumToColumn(const E_Columns & ore_Value) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   //Drag & drop
   virtual QStringList mimeTypes() const override;
   virtual QMimeData * mimeData(const QModelIndexList & orc_Indices) const override;
   //lint -restore
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCElement(const QModelIndex & orc_Index) const;
   const C_PuiSdNodeDataPoolListElement * GetUIElement(const QModelIndex & orc_Index) const;
   void HandleDataChange(const stw_types::uint32 & oru32_Row,
                         const C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_ChangeType,
                         const stw_types::sint32 & ors32_DataSetIndex);
   void DoInsertDataSet(const stw_types::uint32 & oru32_DataSetIndex);
   void DoDeleteDataSet(const stw_types::uint32 & oru32_DataSetIndex);
   void DoEditDataSetName(const stw_types::uint32 & oru32_DataSetIndex);
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
                      const C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_DataChangeType,
                      const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   C_SdNdeDataPoolListTableErrorManager mc_ErrorManager;

   bool m_CheckLink(const QModelIndex & orc_Index) const;
   void m_OnErrorChange(void);
   void m_MoveItems(const std::vector<stw_types::uint32> & orc_ContiguousIndices,
                    const stw_types::uint32 ou32_TargetIndex) const;
   void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model component for filter item table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERTABLEMODEL_H
#define C_CAMMOSFILTERTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblModelAction.h"
#include "C_CamProFilterData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosFilterTableModel :
   public C_TblModelAction
{
public:
   enum E_Columns
   {
      eENABLED = 0,
      eTYPE,
      eMODE,
      eDETAILS,
      eREMOVE
   };

   C_CamMosFilterTableModel(QObject * const opc_Parent = NULL);

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation,
                               const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::DisplayRole))
   const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual void CopySelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   // Converters
   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);

   // Utility for database
   static QString h_SearchMessageInDatabases(const stw_types::uint32 ou32_CanId, const bool oq_IsExtended);

   // Get and set data
   QList<stw_opensyde_gui_logic::C_CamProFilterItemData> GetFilterItemsData(void) const;
   void SetFilterItemsData(const QList<C_CamProFilterItemData> & orc_FilterItemsData);
   void SetFilterItemIDs(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_NewStartId,
                         const stw_types::uint32 ou32_NewEndId, const bool oq_IsExtended);
   void SetFilterItemExtended(const stw_types::uint32 ou32_Index, const bool oq_Extended);

protected:
   virtual stw_types::uint32 m_AddNewItem(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual std::vector<stw_types::uint32> m_PasteItems(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual stw_types::uint32 m_GetSizeItems(void) const override;
   virtual void m_DeleteItem(const stw_types::uint32 ou32_Index) override;
   virtual void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) override;

private:
   QList<stw_opensyde_gui_logic::C_CamProFilterItemData> mc_FilterItemsData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

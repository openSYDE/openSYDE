//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model component for filter item table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERTABLEMODEL_HPP
#define C_CAMMOSFILTERTABLEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblModelAction.hpp"
#include "C_CamProFilterData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                       const int32_t os32_Role = static_cast<int32_t>(Qt::DisplayRole))
   const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   void CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   // Converters
   static E_Columns h_ColumnToEnum(const int32_t os32_Column);
   static int32_t h_EnumToColumn(const E_Columns oe_Value);

   // Utility for database
   static QString h_SearchMessageInDatabases(const uint32_t ou32_CanId, const bool oq_IsExtended);

   // Get and set data
   QList<stw::opensyde_gui_logic::C_CamProFilterItemData> GetFilterItemsData(void) const;
   void SetFilterItemsData(const QList<C_CamProFilterItemData> & orc_FilterItemsData);
   void SetFilterItemIds(const uint32_t ou32_Index, const uint32_t ou32_NewStartId, const uint32_t ou32_NewEndId,
                         const bool oq_IsExtended);
   void SetFilterItemExtended(const uint32_t ou32_Index, const bool oq_Extended);

protected:
   uint32_t m_AddNewItem(const uint32_t ou32_SelectedIndex) override;
   std::vector<uint32_t> m_PasteItems(const uint32_t ou32_SelectedIndex) override;
   uint32_t m_GetSizeItems(void) const override;
   void m_DeleteItem(const uint32_t ou32_Index) override;
   void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) override;

private:
   QList<stw::opensyde_gui_logic::C_CamProFilterItemData> mc_FilterItemsData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

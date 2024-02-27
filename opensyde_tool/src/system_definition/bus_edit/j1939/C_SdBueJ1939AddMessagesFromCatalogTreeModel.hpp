//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939ADDMESSAGESFROMCATALOGTREEMODEL_HPP
#define C_SDBUEJ1939ADDMESSAGESFROMCATALOGTREEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>

#include "stwtypes.hpp"
#include "C_TblTreeModelCheckable.hpp"
#include "C_CieConverter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueJ1939AddMessagesFromCatalogTreeModel :
   public C_TblTreeModelCheckable
{
public:
   enum E_Columns
   {
      ePGN_SPN = 0,
      eCAN_ID, // future use
      eNAME,
      eCOMMENT
   };

   explicit C_SdBueJ1939AddMessagesFromCatalogTreeModel(QObject * const opc_Parent = NULL);
   ~C_SdBueJ1939AddMessagesFromCatalogTreeModel() override;

   void UpdateData(const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported);

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

   // Abstract functions overridden from C_TblTreeModelCheckable
   int32_t Init(const std::vector<uint32_t> & orc_ElementIndices) override;
   void GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const override;
   uint32_t GetCheckedItemCount(void) const override;
   void GetSelectedMessages(std::vector<C_CieConverter::C_CieNodeMessage> & orc_SelectedMessages) const;
   void SelectAllParentItems(void);
   void UnselectAllParentItems(void);
   void SelectFilteredParentItems(const QModelIndexList & orc_FilteredItems);

private:
   class C_SignalTableData
   {
   public:
      uint32_t u32_Spn;
      stw::scl::C_SclString c_Name;
      stw::scl::C_SclString c_Comment;
   };

   class C_MsgTableData
   {
   public:
      QVariant c_Icon;
      QVariant c_CanId;
      QVariant c_J1939Pgn;
      stw::scl::C_SclString c_Name;
      stw::scl::C_SclString c_Comment;
      std::vector< C_SignalTableData> c_Signals;
   };

   void m_FillMessageInfo(void);
   void m_DecodeIndex(const QModelIndex & orc_ModelIndex, uint32_t & oru32_MessageIndex, uint32_t & oru32_SignalIndex,
                      bool & orq_IsSignal) const;

   std::vector<C_MsgTableData> mc_MessageInfoList;
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> mc_MessagesImportedFromCatalog;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

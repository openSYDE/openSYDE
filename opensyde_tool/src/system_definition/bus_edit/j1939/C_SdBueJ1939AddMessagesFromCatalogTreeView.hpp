//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939ADDMESSAGESFROMCATALOGTREEVIEW_HPP
#define C_SDBUEJ1939ADDMESSAGESFROMCATALOGTREEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_TblTreMultiColumnLeafSortFilter.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogTreeModel.hpp"
#include "C_CieConverter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdBueJ1939AddMessagesFromCatalogTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdBueJ1939AddMessagesFromCatalogTreeDelegate(QObject * const opc_Parent = NULL);

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;
};

class C_SdBueJ1939AddMessagesFromCatalogTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SdBueJ1939AddMessagesFromCatalogTreeView(QWidget * const opc_Parent = NULL);
   bool IsEmpty(void) const;
   void Search(const QString & orc_Text);
   void UpdateData(const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported);
   void SelectAllMessages(const bool oq_IsFilterTextEmpty);
   void UnSelectAllMessages(void);
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> GetSelectedMessages(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const uint32_t ou32_Count) const;

private:
   void m_InitColumns(void);
   static std::map<opensyde_gui_logic::C_SdBueJ1939AddMessagesFromCatalogTreeModel::E_Columns,
                   uint32_t> mh_GetDefaultColumnWidths(void);
   void m_OnItemChecked();

   stw::opensyde_gui_logic::C_TblTreMultiColumnLeafSortFilter mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdBueJ1939AddMessagesFromCatalogTreeModel mc_Model;
   C_SdBueJ1939AddMessagesFromCatalogTreeDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

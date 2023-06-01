//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for adding messages from J1939 Catalog
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939ADDMESSAGESFROMCATALOGTABLEVIEW_H
#define C_SDBUEJ1939ADDMESSAGESFROMCATALOGTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>

#include "C_TblViewScroll.hpp"
#include "C_SdBueJ1939AddMessagesFromCatalogTableModel.hpp"
#include "C_CieConverter.hpp"
#include "C_TblTreMultiColumnLeafSortFilter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueJ1939AddMessagesFromCatalogTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdBueJ1939AddMessagesFromCatalogTableView(QWidget * const opc_Parent = NULL);

   void UpdateData(const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported);
   bool IsEmpty(void) const;
   void Search(const QString & orc_Text);
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> GetSelectedMessages(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const int32_t os32_Count);

protected:
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   //Avoid call
   C_SdBueJ1939AddMessagesFromCatalogTableView(const C_SdBueJ1939AddMessagesFromCatalogTableView &);
   C_SdBueJ1939AddMessagesFromCatalogTableView & operator =(const C_SdBueJ1939AddMessagesFromCatalogTableView &) &;

   void m_InitColumns(void);
   static std::map<opensyde_gui_logic::C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns,
                   uint32_t> mh_GetDefaultColumnWidths(void);
   QModelIndexList m_MapModelIndices(const QModelIndexList & orc_SortModelIndices) const;
   uint32_t m_GetSelectedIndexCount() const;

   stw::opensyde_gui_logic::C_TblTreMultiColumnLeafSortFilter mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdBueJ1939AddMessagesFromCatalogTableModel mc_Model;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

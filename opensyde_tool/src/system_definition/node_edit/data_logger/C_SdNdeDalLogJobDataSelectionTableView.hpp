//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for log job data selection
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBDATASELECTIONTABLEVIEW_H
#define C_SDNDEDALLOGJOBDATASELECTIONTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>

#include "C_TblViewScroll.hpp"
#include "C_SdNdeDalLogJobDataSelectionTableModel.hpp"
#include "C_OscDataLoggerDataElementReference.hpp"
#include "C_SdNdeDalLogJobDataSelectionTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobDataSelectionTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDalLogJobDataSelectionTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobDataSelectionTableView(void) override;

   void LoadUserSettings(const std::vector<int32_t> & orc_Values);
   void SaveUserSettings(std::vector<int32_t> & orc_Values) const;
   bool IsEmpty(void) const;
   void Search(const QString & orc_Text);
   void UpdateData(const std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> & orc_DataElements,
                   const uint32_t ou32_NodeIndex);
   void AddData(const std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> & orc_DataElements,
                const uint32_t ou32_NodeIndex);
   void GetSelectedElements(std::vector<uint32_t> & orc_SelectedIndices) const;
   void DeleteSelectedElements();
   void GetElementLocationCount(uint32_t & oru32_LocalElements, uint32_t & oru32_RemoteElements);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataChanged(const QModelIndex & orc_Index, const QString oc_Data);
   void SigDeleteSelectedElements();

protected:
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   void m_InitColumns(void);
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   static std::map<opensyde_gui_logic::C_SdNdeDalLogJobDataSelectionTableModel::E_Columns,
                   uint32_t> mh_GetDefaultColumnWidths(void);
   QModelIndexList m_MapModelIndices(const QModelIndexList & orc_SortModelIndices) const;

   stw::opensyde_gui_logic::C_SdNdeDalLogJobDataSelectionTableModel mc_Model;
   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdNdeDalLogJobDataSelectionTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGETABLEVIEW_H
#define C_SDBUEMESSAGETABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_TblViewScroll.hpp"
#include "C_SdBueMessageTableModel.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_SdBueMessageSignalTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdBueMessageTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageTableView(void);

   void LoadUserSettings(const std::vector<int32_t> & orc_Values);
   void SaveUserSettings(std::vector<int32_t> & orc_Values) const;
   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void UpdateData(void);
   int32_t GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMessageTableView(const C_SdBueMessageTableView &);
   C_SdBueMessageTableView & operator =(const C_SdBueMessageTableView &) &;

   const stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;

   void m_InitColumns(void);
   void m_HandleColumnVisibility(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdBueMessageTableModel mc_Model;
   stw::opensyde_gui_logic::C_SdBueMessageSignalTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

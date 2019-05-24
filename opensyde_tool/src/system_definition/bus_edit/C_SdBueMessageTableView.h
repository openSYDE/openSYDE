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
#include <QTableView>
#include <QSortFilterProxyModel>
#include "C_SdBueMessageTableModel.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_SdBueMessageSignalTableDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageTableView :
   public QTableView
{
   Q_OBJECT

public:
   C_SdBueMessageTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMessageTableView(void);

   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void UpdateData(void);
   stw_types::sintn GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdBueMessageTableView(const C_SdBueMessageTableView &);
   C_SdBueMessageTableView & operator =(const C_SdBueMessageTableView &);

   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_SdBueMessageTableModel mc_Model;
   stw_opensyde_gui_logic::C_SdBueMessageSignalTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

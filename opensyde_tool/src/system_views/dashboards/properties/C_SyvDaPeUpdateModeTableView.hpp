//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for system view dashboard data element update mode (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETABLEVIEW_HPP
#define C_SYVDAPEUPDATEMODETABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeWidget>
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_SyvDaPeUpdateModeTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaPeUpdateModeTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SyvDaPeUpdateModeTableView(const uint32_t ou32_ViewIndex = 0, QTreeWidget * const opc_TreeWidget = NULL,
                                QTreeWidgetItem * const opc_TreeWidgetItem = NULL);
   ~C_SyvDaPeUpdateModeTableView(void) override;
   void InitColumns(void);
   void AdjustToItems(const bool & orq_Initial = false, const bool oq_Last = false);

private:
   QTreeWidget * const mpc_TreeWidget;
   QTreeWidgetItem * const mpc_TreeWidgetItem;
   C_SyvDaPeUpdateModeTableDelegate mc_Delegate;

   //Avoid call
   C_SyvDaPeUpdateModeTableView(const C_SyvDaPeUpdateModeTableView &);
   C_SyvDaPeUpdateModeTableView & operator =(const C_SyvDaPeUpdateModeTableView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

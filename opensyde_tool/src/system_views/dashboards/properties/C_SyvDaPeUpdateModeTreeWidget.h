//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode tree (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETREEWIDGET_H
#define C_SYVDAPEUPDATEMODETREEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeWidget>
#include "stwtypes.h"
#include "C_SyvDaPeUpdateModeTableModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaPeUpdateModeTreeWidget :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_SyvDaPeUpdateModeTreeWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaPeUpdateModeTreeWidget(void);

   void Init(const stw_types::uint32 ou32_ViewIndex);
   void Select(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   void ApplyData(void) const;

private:
   std::vector<stw_opensyde_gui_logic::C_SyvDaPeUpdateModeTableModel *> mc_Models;

   void m_Clear(void);
   void m_OnExpandRequested(QTreeWidgetItem * const opc_Item, const bool oq_Expand);
   void m_OnCollapse(const QModelIndex & orc_Index) const;
   void m_OnExpand(const QModelIndex & orc_Index) const;
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

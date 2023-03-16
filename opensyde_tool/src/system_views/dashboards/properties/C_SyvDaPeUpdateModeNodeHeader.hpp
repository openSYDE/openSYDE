//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode node header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODENODEHEADER_H
#define C_SYVDAPEUPDATEMODENODEHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTreeWidgetItem>
#include "stwtypes.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeUpdateModeNodeHeader;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeUpdateModeNodeHeader :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeUpdateModeNodeHeader(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                          QTreeWidgetItem * const opc_Item, QWidget * const opc_Parent = NULL);
   ~C_SyvDaPeUpdateModeNodeHeader(void) override;

   void InitStaticNames(void) const;
   void UpdateCount(const int32_t os32_Count = -1) const;
   void RegisterExpandOrCollapse(const bool & orq_Expanded) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigExpand(QTreeWidgetItem * const opc_Item, const bool oq_Expand);

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   Ui::C_SyvDaPeUpdateModeNodeHeader * mpc_Ui;
   QTreeWidgetItem * const mpc_TreeWidgetItem;
   const uint32_t mu32_ViewIndex;
   const uint32_t mu32_NodeIndex;
   //View restrictions
   static const uint32_t mhu32_HEADER_HEIGHT;

   void m_OnPushButtonExpandClicked(const bool oq_Checked);

   //Avoid call
   C_SyvDaPeUpdateModeNodeHeader(const C_SyvDaPeUpdateModeNodeHeader &);
   C_SyvDaPeUpdateModeNodeHeader & operator =(const C_SyvDaPeUpdateModeNodeHeader &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

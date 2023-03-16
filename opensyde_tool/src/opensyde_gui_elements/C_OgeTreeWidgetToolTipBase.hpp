//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLTIPBASE_H
#define C_OGETREEWIDGETTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeWidget>
#include "stwtypes.hpp"
#include "C_OgeTreeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeWidgetToolTipBase :
   public QTreeWidget,
   public C_OgeTreeToolTipBase
{
public:
   C_OgeTreeWidgetToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;
   bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

   QPoint m_MapToGlobal(const QPoint & orc_LocalPos) const override;
   const QHeaderView * m_Header(void) const override;
   const QAbstractItemModel * m_Model(void) const override;
   bool m_BaseEvent(QEvent * const opc_Event) override;
   void m_SetMouseTracking(const bool oq_Active) override;
   QModelIndex m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEVIEWTOOLTIPBASE_H
#define C_OGETREEVIEWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeView>
#include <QStyledItemDelegate>
#include <QProxyStyle>
#include "stwtypes.hpp"
#include "C_OgeTreeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeTreeViewToolTipBaseDelegate :
   public QStyledItemDelegate
{
public:
   // This delegate class is optional and must be set in derived classes of C_OgeTreeViewToolTipBase
   // for using. It adapts the checkbox clickable areas in the tree.
   C_OgeTreeViewToolTipBaseDelegate(QObject * const opc_Parent = NULL);

protected:
   bool editorEvent(QEvent * const opc_Event, QAbstractItemModel * const opc_Model,
                    const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) override;
};

class C_OgeTreeViewToolTipBase :
   public QTreeView,
   public C_OgeTreeToolTipBase
{
public:
   C_OgeTreeViewToolTipBase(QWidget * const opc_Parent = NULL);

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

   std::vector<int32_t> m_GetColumnWidths(void) const;
   bool m_SetColumnWidths(const std::vector<int32_t> & orc_ColumnWidths);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

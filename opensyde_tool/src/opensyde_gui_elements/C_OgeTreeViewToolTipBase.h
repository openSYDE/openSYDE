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
#include "stwtypes.h"
#include "C_OgeTreeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
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
   virtual bool editorEvent(QEvent * const opc_Event, QAbstractItemModel * const opc_Model,
                            const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) override;
};

class C_OgeTreeViewToolTipBase :
   public QTreeView,
   public C_OgeTreeToolTipBase
{
public:
   C_OgeTreeViewToolTipBase(QWidget * const opc_Parent = NULL);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

   virtual QPoint m_MapToGlobal(const QPoint & orc_LocalPos) const override;
   virtual const QHeaderView * m_Header(void) const override;
   virtual const QAbstractItemModel * m_Model(void) const override;
   virtual bool m_BaseEvent(QEvent * const opc_Event) override;
   virtual void m_SetMouseTracking(const bool oq_Active) override;
   virtual QModelIndex m_IndexAtGlobalPos(const QPoint & orc_GlobalPos) const override;

   std::vector<stw_types::sint32> m_GetColumnWidths(void) const;
   bool m_SetColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

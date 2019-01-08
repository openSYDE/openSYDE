//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget element for the QTreeWidget based C_OgeTreeWidgetToolBarSearch painted in delegate

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLBARSEARCHITEMWIDGET_H
#define C_OGETREEWIDGETTOOLBARSEARCHITEMWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QString>

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_OgeTreeWidgetToolBarSearchItemWidget;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTreeWidgetToolBarSearchItemWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeTreeWidgetToolBarSearchItemWidget(QWidget * const opc_Parent = NULL);
   explicit C_OgeTreeWidgetToolBarSearchItemWidget(const QString & orc_Title, const QString & orc_Subtitle,
                                                   QWidget * const opc_Parent = NULL);
   virtual ~C_OgeTreeWidgetToolBarSearchItemWidget();

private:
   //Avoid call
   C_OgeTreeWidgetToolBarSearchItemWidget(const C_OgeTreeWidgetToolBarSearchItemWidget &);
   C_OgeTreeWidgetToolBarSearchItemWidget & operator =(const C_OgeTreeWidgetToolBarSearchItemWidget &);

   Ui::C_OgeTreeWidgetToolBarSearchItemWidget * mpc_Ui;
};
}

#endif

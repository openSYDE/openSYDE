//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget element for the QTreeWidget based C_OgeTreeWidgetToolBarSearch painted in delegate

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLBARSEARCHITEMWIDGET_H
#define C_OGETREEWIDGETTOOLBARSEARCHITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeTreeWidgetToolBarSearchItemWidget;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeWidgetToolBarSearchItemWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeTreeWidgetToolBarSearchItemWidget(QWidget * const opc_Parent = NULL);
   explicit C_OgeTreeWidgetToolBarSearchItemWidget(const QString & orc_Title, const QString & orc_Subtitle,
                                                   QWidget * const opc_Parent = NULL);
   ~C_OgeTreeWidgetToolBarSearchItemWidget() override;

private:
   //Avoid call
   C_OgeTreeWidgetToolBarSearchItemWidget(const C_OgeTreeWidgetToolBarSearchItemWidget &);
   C_OgeTreeWidgetToolBarSearchItemWidget & operator =(const C_OgeTreeWidgetToolBarSearchItemWidget &) &;

   Ui::C_OgeTreeWidgetToolBarSearchItemWidget * mpc_Ui;
};
}
}

#endif

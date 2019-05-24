//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table drawing delegate for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTADELEGATE_H
#define C_SYVDAITTADELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QStyledItemDelegate>
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTaDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvDaItTaDelegate(QObject * const opc_Parent = NULL);

   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   QColor mc_BarColor;
   QColor mc_BackgroundColorDefault;
   QColor mc_BackgroundColorAlternating;
   QColor mc_BackgroundColorSelection;
   QColor mc_BorderColorHover;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

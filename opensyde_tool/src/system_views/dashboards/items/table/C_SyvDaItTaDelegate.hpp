//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table drawing delegate for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTADELEGATE_HPP
#define C_SYVDAITTADELEGATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QStyledItemDelegate>
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTaDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvDaItTaDelegate(QObject * const opc_Parent = NULL);

   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;

private:
   QColor mc_BarColor;
   QColor mc_BackgroundColorDefault;
   QColor mc_BackgroundColorAlternating;
   QColor mc_BackgroundColorSelection;
   QColor mc_BorderColorHover;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Toolbar search item delegate (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QStyleOptionViewItem>

#include "stwtypes.hpp"
#include "constants.hpp"

#include "C_OgeTreeWidgetToolBarSearchItemDelegate.hpp"

#include "C_OgeTreeWidgetToolBarSearchItemWidget.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearchItemDelegate::C_OgeTreeWidgetToolBarSearchItemDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint method

   Custom way to paint tree widget item

   \param[in,out]  opc_Painter   Painter (default interface)
   \param[in]      orc_Option    Option (default interface)
   \param[in]      orc_Index     Index (default interface)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeWidgetToolBarSearchItemDelegate::paint(QPainter * const opc_Painter,
                                                     const QStyleOptionViewItem & orc_Option,
                                                     const QModelIndex & orc_Index) const
{
   if ((orc_Index.isValid() == true) &&
       (orc_Index.parent().isValid() == true)) // not on root
   {
      const QIcon c_Icon = qvariant_cast<QIcon>(orc_Index.data(static_cast<int32_t>(Qt::DecorationRole)));

      // creating local QWidget (that's why i think it should be fasted, cause we
      // don't touch the heap and don't deal with a QWidget except painting)
      C_OgeTreeWidgetToolBarSearchItemWidget c_ItemWidget(
         orc_Index.data(static_cast<int32_t>(Qt::DisplayRole)).toString(),
         orc_Index.data(static_cast<int32_t>(Qt::UserRole) + 1).toString()); //mhs32_DATAROLE_SUBTITLE

      // paint the background
      if ((C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_MouseOver) == true) ||
          (C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_Selected) == true))
      {
         QStyleOptionViewItem c_Option = orc_Option;
         QBrush c_Brush;
         QPen c_Pen;

         //Adapt rect (paint left transparent corner)
         c_Option.rect.setX(orc_Option.rect.x() - 10);

         c_Brush = opc_Painter->brush();
         c_Brush.setColor(stw::opensyde_gui::mc_STYLE_GUIDE_COLOR_11);
         c_Brush.setStyle(Qt::SolidPattern);
         c_Pen.setColor(Qt::transparent);
         opc_Painter->setBrush(c_Brush);
         opc_Painter->setPen(c_Pen);

         opc_Painter->drawRect(c_Option.rect);
      }

      if (c_Icon.isNull() == false)
      {
         // adapt the position of the icon
         // it will be drawn into the free space of the widget. The layout left margin of the widget must be big enough.
         const QRect c_Rect = orc_Option.rect.adjusted(0, 8, 0, 8);
         c_Icon.paint(opc_Painter, c_Rect, Qt::AlignLeft);
      }

      // showEvent will not be triggered. Separate initialization necessary
      // rendering of QWidget itself
      // store the configuration of the original painter before translation. The change of the coordinates
      // is only necessary here.
      opc_Painter->save();
      opc_Painter->translate(orc_Option.rect.topLeft());
      c_ItemWidget.render(opc_Painter,
                          QPoint(0, 0),
                          QRegion(0, 0, orc_Option.rect.width(), orc_Option.rect.height()),
                          QWidget::RenderFlag::DrawChildren);

      // restore the previous coordinates
      opc_Painter->restore();
   }
   else
   {
      // top level elements
      QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reimplement default size hint

   \param[in]  orc_Option  Option
   \param[in]  orc_Index   Index

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_OgeTreeWidgetToolBarSearchItemDelegate::sizeHint(const QStyleOptionViewItem & orc_Option,
                                                         const QModelIndex & orc_Index) const
{
   Q_UNUSED(orc_Index)
   return QSize(orc_Option.rect.width(), 40);
}

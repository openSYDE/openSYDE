//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       List item to display project (implementation)

   List item to display project

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFont>
#include <QFontMetrics>
#include "stwtypes.h"
#include "constants.h"
#include "C_PopListItem.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     18.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PopListItem::C_PopListItem(QObject * const opc_Parent) :
   QItemDelegate(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint method

   Custom way to paint list item

   \param[in,out] opc_Painter Painter (default interface)
   \param[in]     orc_Option  Option (default interface)
   \param[in]     orc_index   Index (default interface)

   \created     18.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PopListItem::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const
{
   if (orc_Option.state.testFlag(QStyle::State_MouseOver) == true)
   {
      const QFont c_FontHeading("Segoe UI", 12, static_cast<sintn>(QFont::Normal));
      const QFontMetrics c_FontHeadingInfo(c_FontHeading);
      const sintn sn_Height = orc_Option.rect.height();
      QRect c_Rect = orc_Option.rect;
      QColor c_ColorPen(42, 42, 91);
      QPen c_Pen(c_ColorPen, 1.0, Qt::SolidLine);
      //advisory
      opc_Painter->save();
      opc_Painter->setBrush(QBrush(QColor(245, 247, 248)));
      //Deactivate pen to disable border
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->drawRect(c_Rect);

      //foreground
      opc_Painter->setPen(c_Pen);

      //Display heading
      {
         QString c_Heading = orc_Index.data(static_cast<sintn>(Qt::DisplayRole)).toString();
         c_Rect = orc_Option.rect.adjusted(0, 0, 0, c_FontHeadingInfo.height() - sn_Height);
         opc_Painter->setFont(c_FontHeading);
         opc_Painter->drawText(c_Rect, static_cast<sintn>(Qt::AlignLeft), c_Heading);
      }
      //Display description
      {
         QString c_Description = orc_Index.data(msn_USER_ROLE_ADDITIONAL_INFORMATION).toString();
         c_Rect = orc_Option.rect.adjusted(0, c_FontHeadingInfo.height(), 0, 0);
         opc_Painter->setFont(QFont("Segoe UI", 10, static_cast<sintn>(QFont::Normal)));
         opc_Painter->drawText(c_Rect, static_cast<sintn>(Qt::AlignLeft), c_Description);
      }
      //advisory
      opc_Painter->restore();
   }
   else
   {
      QItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   }
}

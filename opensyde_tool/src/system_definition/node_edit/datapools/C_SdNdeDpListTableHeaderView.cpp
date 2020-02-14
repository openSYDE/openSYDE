//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom header view for table (implementation)

   Custom header view for table

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QSpinBox>
#include <QStyleOptionHeader>
#include <QPainter>
#include "C_SdNdeDpListTableHeaderView.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ore_Orientation Orientation
   \param[in,out] opc_Parent      Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableHeaderView::C_SdNdeDpListTableHeaderView(const Qt::Orientation & ore_Orientation,
                                                           QWidget * const opc_Parent) :
   QHeaderView(ore_Orientation, opc_Parent)
{
   //May be cyclic trigger directly in paintSection necessary
   QStyleOptionHeader * const pc_Option = new QStyleOptionHeader();

   pc_Option->iconAlignment = Qt::AlignRight;

   switch (ore_Orientation)
   {
   case Qt::Horizontal:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IsHorizontal", true);
      break;
   case Qt::Vertical:
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IsHorizontal", false);
      break;
   default:
      //Unknown
      break;
   }
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IsHovered", false);

   this->initStyleOption(pc_Option);
   delete (pc_Option);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableHeaderView::~C_SdNdeDpListTableHeaderView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten view options event slot

   \return
   Currently active view options
*/
//----------------------------------------------------------------------------------------------------------------------
QStyleOptionViewItem C_SdNdeDpListTableHeaderView::viewOptions(void) const
{
   //Seems to ignore drawing delegate which might be the only use for this virtual function
   QStyleOptionViewItem c_Retval = QHeaderView::viewOptions();

   c_Retval.decorationAlignment = Qt::AlignVCenter | Qt::AlignRight;
   c_Retval.decorationPosition = QStyleOptionViewItem::Right;

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint section event slot

   Here: Draw icon manually

   \param[in,out] opc_Painter      Painter
   \param[in]     orc_Rect         Rectangle
   \param[in]     osn_LogicalIndex Logical index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableHeaderView::paintSection(QPainter * const opc_Painter, const QRect & orc_Rect,
                                                const sintn osn_LogicalIndex) const
{
   QHeaderView::paintSection(opc_Painter, orc_Rect, osn_LogicalIndex);
   //You can draw anything here, it is not shown as long as you do call the original function
   if (this->model() != NULL)
   {
      //Check if icon set
      const QVariant c_Variant =
         this->model()->headerData(osn_LogicalIndex, this->orientation(), static_cast<sintn>(Qt::DecorationRole));
      const QPixmap c_Icon = qvariant_cast<QPixmap>(c_Variant);
      if (c_Icon.isNull() == false)
      {
         //Draw icon if any
         opc_Painter->drawPixmap(orc_Rect, c_Icon);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse enter event slot

   Here: apply custom style

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableHeaderView::enterEvent(QEvent * const opc_Event)
{
   QHeaderView::enterEvent(opc_Event);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IsHovered", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse leave event slot

   Here: revert custom style

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableHeaderView::leaveEvent(QEvent * const opc_Event)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "IsHovered", false);
   QHeaderView::leaveEvent(opc_Event);
}

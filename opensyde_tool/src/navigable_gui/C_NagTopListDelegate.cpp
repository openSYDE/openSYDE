//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for topology list (implementation)

   Delegate for topology list

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "C_Uti.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_NagTopListDelegate.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopListDelegate::C_NagTopListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   ms32_Selected(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected index

   \param[in] os32_Selected Selected index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListDelegate::SetSelectedIndex(const stw_types::sint32 os32_Selected)
{
   this->ms32_Selected = os32_Selected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint method

   Custom way to paint padding for items

   \param[in,out] opc_Painter Painter (default interface)
   \param[in]     orc_Option  Option (default interface)
   \param[in]     orc_Index   Index (default interface)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopListDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                 const QModelIndex & orc_Index) const
{
   const sint32 s32_PaddingLeft = 24;
   const QRect c_Rect(orc_Option.rect.topLeft(), QSize(s32_PaddingLeft, orc_Option.rect.size().height()));
   const bool q_IsHovered = C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_MouseOver);
   QStyleOptionViewItem c_Option = orc_Option;

   //Add padding
   c_Option.rect.adjust(s32_PaddingLeft, 0, 0, 0);

   //Never draw focus rectangle
   //lint -e{730, 746, 1013, 1055} Use clean Qt interface
   c_Option.state.setFlag(QStyle::State_HasFocus, false);

   if (orc_Index.row() == this->ms32_Selected)
   {
      //Don't allow drawing of hovered state for selected item
      //lint -e{730, 746, 1013, 1055} Use clean Qt interface
      c_Option.state.setFlag(QStyle::State_MouseOver, false);
      //Fill up padding for selection
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_21);
      opc_Painter->drawRect(c_Rect);
      opc_Painter->restore();
   }
   else if (q_IsHovered)
   {
      //Fill up padding for hover
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_3);
      opc_Painter->drawRect(c_Rect);
      opc_Painter->restore();
   }
   else
   {
      //Nothing to do
   }
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded size hint

   \param[in] orc_Option Option
   \param[in] orc_Index  Index

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_NagTopListDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      c_Retval.setHeight(30);
   }

   return c_Retval;
}

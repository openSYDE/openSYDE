//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for topology item navigation tree

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "stwtypes.h"
#include "C_Uti.h"
#include "C_NagTopTreeDelegate.h"

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
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagTopTreeDelegate::C_NagTopTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint method

   Here: paint hover and selected background on whole tree widget width

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagTopTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                 const QModelIndex & orc_Index) const
{
   const QRect c_Rect(orc_Option.rect.topLeft() - QPoint(40, 0), orc_Option.rect.size() + QSize(40, 0));
   const bool q_IsHovered = C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_MouseOver);
   const bool q_IsEnabled = C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_Enabled);
   const bool q_IsSelected = C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_Selected);
   QStyleOptionViewItem c_Option = orc_Option;

   //Never draw focus rectangle
   c_Option.state.setFlag(QStyle::State_HasFocus, false);

   if (q_IsSelected == true)
   {
      //Don't allow drawing of hovered state for selected item
      c_Option.state.setFlag(QStyle::State_MouseOver, false);
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_21);
      opc_Painter->drawRect(c_Rect);
      opc_Painter->restore();
   }
   else if ((q_IsHovered == true) && (q_IsEnabled == true))
   {
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_3);
      opc_Painter->drawRect(c_Rect);
      opc_Painter->restore();
   }
   else
   {
      //Nothing special to do
   }
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
}

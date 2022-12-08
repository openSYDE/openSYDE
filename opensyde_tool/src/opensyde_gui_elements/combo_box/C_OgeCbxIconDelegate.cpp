//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box item delagate for custom drawing (implementation)

   Combo box item delagate for custom drawing

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "constants.hpp"
#include "C_OgeCbxIconDelegate.hpp"
#include "stwtypes.hpp"
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxIconDelegate::C_OgeCbxIconDelegate(void) :
   QStyledItemDelegate(),
   ms16_PaddingLeft(5)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set padding

   \param[in] os16_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxIconDelegate::SetPaddingLeft(const int16_t os16_Value)
{
   this->ms16_PaddingLeft = os16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Custom paint to add a padding left

   - add padding (ms16_PaddingLeft)
   - call original paint
   - paint selection rectangle if required

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxIconDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                 const QModelIndex & orc_Index) const
{
   QStyleOptionViewItem c_Option = orc_Option;
   QRect c_Rect;

   //Prepare additional rectangle to draw selection for complete item
   c_Rect.setTopLeft(orc_Option.rect.topLeft());
   c_Rect.setHeight(orc_Option.rect.height());
   c_Rect.setWidth(this->ms16_PaddingLeft);

   //Adapt original item position
   c_Option.rect.setX(orc_Option.rect.x() + ms16_PaddingLeft);
   //Disable hover for disabled item
   c_Option.state.setFlag(QStyle::State_MouseOver,
                          ((C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_MouseOver)) &&
                           orc_Index.flags().testFlag(Qt::ItemIsEnabled)));

   //Draw original item & icon
   QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);

   //Draw selection rectangle

   if (orc_Index.flags().testFlag(Qt::ItemIsEnabled) &&
       (C_Uti::h_CheckStyleState(orc_Option.state, QStyle::State_Selected) == true))
   {
      opc_Painter->save();
      opc_Painter->setBrush(stw::opensyde_gui::mc_STYLE_GUIDE_COLOR_25); //selection color

      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->drawRect(c_Rect);
      opc_Painter->restore();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include <QPainter>
#include "constants.hpp"
#include "C_TblTreDelegateUtil.hpp"
#include "C_SdNdeCoPdoTableDelegate.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
C_SdNdeCoPdoTableDelegate::C_SdNdeCoPdoTableDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_Model(NULL),
   ms32_HoveredRow(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint item

   Here: special handling for boolean & hovered cells

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                      const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);

   // paint icon
   C_TblTreDelegateUtil::h_PaintIcon(opc_Painter, orc_Option, orc_Index);

   //Draw hover effect manually
   if (orc_Index.isValid() == true)
   {
      if (orc_Index.row() == this->ms32_HoveredRow)
      {
         const QPoint c_TopLeft = orc_Option.rect.topLeft();
         const QPoint c_TopRight = orc_Option.rect.topRight();
         const QPoint c_BottomLeft = orc_Option.rect.bottomLeft();
         const QPoint c_BottomRight = orc_Option.rect.bottomRight();
         opc_Painter->save();
         opc_Painter->setPen(mc_STYLE_GUIDE_COLOR_7);
         //Top
         opc_Painter->drawLine(c_TopLeft, c_TopRight);
         //Bottom
         opc_Painter->drawLine(c_BottomLeft, c_BottomRight);
         //Left
         if (orc_Index.column() == 0)
         {
            opc_Painter->drawLine(c_TopLeft, c_BottomLeft);
         }
         //Right
         if (this->mpc_Model != NULL)
         {
            if (orc_Index.column() == (this->mpc_Model->columnCount() - 1))
            {
               opc_Painter->drawLine(c_TopRight, c_BottomRight);
            }
         }
         opc_Painter->restore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set hovered row index

   \param[in]  ors32_Value    New hovered row index

   \return
   true  Change
   false No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoPdoTableDelegate::SetHoveredRow(const int32_t & ors32_Value)
{
   bool q_Retval;

   if (this->ms32_HoveredRow == ors32_Value)
   {
      q_Retval = false;
   }
   else
   {
      q_Retval = true;
   }
   this->ms32_HoveredRow = ors32_Value;
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model for column look up

   \param[in]  opc_Value   Model for column look up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoPdoTableDelegate::SetModel(const QAbstractTableModel * const opc_Value)
{
   this->mpc_Model = opc_Value;
}

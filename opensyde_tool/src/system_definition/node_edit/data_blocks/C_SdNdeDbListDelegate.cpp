//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Application list display delegate (implementation)

   Application list display delegate

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "stwtypes.h"

#include "C_SdNdeDbListDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbListDelegate::C_SdNdeDbListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_ApplicationWidget(NULL),
   ms32_IndexPaint(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw widget for drag event

   \param[in,out] opc_Painter Painter
   \param[in,out] orc_Option  Option
   \param[in,out] orc_Index   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (((ms32_IndexPaint == orc_Index.row()) &&
        (orc_Option.rect.x() == 0) &&
        (orc_Option.rect.y() == 0)) && (this->mpc_ApplicationWidget != NULL))
   {
      if ((orc_Index.isValid() == true) && (orc_Index.parent().isValid() == false))
      {
         // creating local QWidget (that's why i think it should be fasted, cause we
         // don't touch the heap and don't deal with a QWidget except painting)
         C_SdNdeDbWidget c_ItemWidget(
            this->mpc_ApplicationWidget->GetNodeIndex(), this->mpc_ApplicationWidget->GetApplicationIndex());

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
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update index to paint widget in drag

   \param[in] os32_Index          Update index for
   \param[in] opc_MovingWidget   Actual widget which will be moved
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListDelegate::StartPaint(const sint32 os32_Index, C_SdNdeDbWidget * const opc_MovingWidget)
{
   this->ms32_IndexPaint = os32_Index;
   this->mpc_ApplicationWidget = opc_MovingWidget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop paint widget in drag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbListDelegate::StopPaint(void)
{
   this->ms32_IndexPaint = -1;
   this->mpc_ApplicationWidget = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list drawing delegate (implementation)

   This delegate handles the drawing of an datapool item widget when dragging an item in the list.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "stwtypes.h"
#include "C_SdNdeDpSelectorListDelegate.h"

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
C_SdNdeDpSelectorListDelegate::C_SdNdeDpSelectorListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_ActualWidget(NULL),
   ms32_IndexPaint(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw widget for drag event

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                                const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if ((ms32_IndexPaint == orc_Index.row()) &&
       (orc_Option.rect.x() == 0) &&
       (orc_Option.rect.y() == 0) &&
       (this->mpc_ActualWidget != NULL))
   {
      if ((orc_Index.isValid() == true) && (orc_Index.parent().isValid() == false))
      {
         // creating local QWidget (that's why i think it should be fasted, cause we
         // don't touch the heap and don't deal with a QWidget except painting)
         C_SdNdeDpSelectorItemWidget c_ItemWidget(this->mpc_ActualWidget->GetUsageViewActive(), NULL);

         // showEvent will not be triggered. Seperate initialization necessary
         c_ItemWidget.InitWidget();
         // configure the widget
         c_ItemWidget.SetData(this->mpc_ActualWidget->GetDatapoolName(),
                              this->mpc_ActualWidget->GetDatapoolComment(),
                              this->mpc_ActualWidget->GetStateSafety(),
                              this->mpc_ActualWidget->GetDataPoolSize(),
                              this->mpc_ActualWidget->GetDataPoolUsedSize(),
                              this->mpc_ActualWidget->GetDataPoolReservedSize());
         c_ItemWidget.SetShareDatapool(this->mpc_ActualWidget->GetShareDatapool());
         c_ItemWidget.SetMaximized(this->mpc_ActualWidget->GetMaximized());
         c_ItemWidget.setStyleSheet(this->mpc_ActualWidget->styleSheet());

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

   \param[in] s32_Index          Update index for
   \param[in] opc_MovingWidget   Actual widget which will be moved
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListDelegate::StartPaint(const sint32 s32_Index,
                                                     C_SdNdeDpSelectorItemWidget * const opc_Widget)
{
   this->ms32_IndexPaint = s32_Index;
   this->mpc_ActualWidget = opc_Widget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop paint widget in drag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorListDelegate::StopPaint(void)
{
   this->ms32_IndexPaint = -1;
   this->mpc_ActualWidget = NULL;
}

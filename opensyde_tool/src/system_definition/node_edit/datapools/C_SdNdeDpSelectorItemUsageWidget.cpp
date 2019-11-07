//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar of an selector item (implementation)

   The widget draws three bars for used, reserved and free space in the complete area.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QEvent>
#include <QHelpEvent>

#include "stwtypes.h"
#include "constants.h"

#include "C_SdNdeDpSelectorItemUsageWidget.h"

#include "C_Uti.h"

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

   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorItemUsageWidget::C_SdNdeDpSelectorItemUsageWidget(QWidget * const opc_Parent) :
   stw_opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   mq_UsedToFull(false),
   mq_ReservedToFull(false),
   mq_UsedOverReserved(false),
   mu32_PercentageUsed(0U),
   mu32_PercentageReserved(0U),
   mu32_Size(0U),
   mu32_Used(0U),
   mu32_Reserved(0U)
{
   // set default inactive color
   this->mc_ColorFree = stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, 20);
   // set default reserved color
   this->mc_ColorReserved = stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, 50);

   this->mc_Brush.setStyle(Qt::SolidPattern);
   this->mc_Pen.setColor(Qt::transparent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the actual percentage of the usage

   \param[in] ou32_Size                   Maximum size
   \param[in] ou32_Used                   Absolute used space of the available space
   \param[in] ou32_Reserved               Absolute reserved space of the available space
   \param[out] oru32_PercentageUsed       Calculated usage percentage
   \param[out] oru32_PercentageReserved   Calculated reservation percentage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemUsageWidget::SetData(const uint32 ou32_Size, const uint32 ou32_Used,
                                                     const uint32 ou32_Reserved, uint32 & oru32_PercentageUsed,
                                                     uint32 & oru32_PercentageReserved)
{
   // Calculate the percentage
   if (ou32_Size != 0U)
   {
      // percentage of the used value
      if (ou32_Used == 0U)
      {
         this->mu32_PercentageUsed = 0U;
      }
      else
      {
         this->mu32_PercentageUsed = (ou32_Used * 100U) / ou32_Size;

         if (ou32_Used > ou32_Size)
         {
            this->mq_UsedToFull = true;
         }
         else
         {
            this->mq_UsedToFull = false;
         }
      }

      // percentage of the reserved value
      if (ou32_Reserved == 0U)
      {
         this->mu32_PercentageReserved = 0U;
      }
      else
      {
         this->mu32_PercentageReserved = (ou32_Reserved * 100U) / ou32_Size;

         if (ou32_Reserved > ou32_Size)
         {
            this->mq_ReservedToFull = true;
         }
         else
         {
            this->mq_ReservedToFull = false;
         }
      }

      this->mq_UsedOverReserved = (ou32_Used > ou32_Reserved);
   }
   else
   {
      this->mu32_PercentageUsed = 0U;
      this->mu32_PercentageReserved = 0;
   }

   // save the values for hint
   this->mu32_Size = ou32_Size;
   this->mu32_Used = ou32_Used;
   this->mu32_Reserved = ou32_Reserved;

   oru32_PercentageUsed = this->mu32_PercentageUsed;
   oru32_PercentageReserved = this->mu32_PercentageReserved;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the usage bars

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorItemUsageWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   QWidget::paintEvent(opc_Event);

   c_Painter.setPen(this->mc_Pen);

   if (((this->mu32_PercentageUsed == 0U) && (this->mu32_PercentageReserved == 0U)) ||
       (this->mq_UsedToFull == true) ||
       (this->mq_ReservedToFull == true) ||
       ((this->mu32_PercentageUsed == 100U) && (this->mu32_PercentageReserved == 100U)) ||
       ((this->mu32_PercentageUsed == 0U) && (this->mu32_PercentageReserved == 100U)))
   {
      // only one bar is visible

      if ((this->mu32_PercentageUsed == 0U) && (this->mu32_PercentageReserved == 0U))
      {
         // special case: nothing used or reserved
         this->mc_Brush.setColor(this->mc_ColorFree);
      }
      else if ((this->mq_UsedToFull == false) && (this->mq_ReservedToFull == false))
      {
         if ((this->mu32_PercentageUsed == 100U) && (this->mu32_PercentageReserved == 100U))
         {
            // special case: full usage and full reservation -> no error and 100%
            this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
         }
         else
         {
            // special case: zero usage and full reservation -> no error and 100%
            this->mc_Brush.setColor(this->mc_ColorReserved);
         }
      }
      else
      {
         // special case: to much reservation or usage -> error with 100% more or more
         this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_24);
      }

      // draw the area
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 0, this->width(), this->height());
   }
   else if ((this->mu32_PercentageUsed >= this->mu32_PercentageReserved) ||
            ((this->mu32_PercentageUsed == 0U) && (this->mu32_PercentageReserved > 0U)) ||
            ((this->mu32_PercentageUsed > 0U) && (this->mu32_PercentageReserved == 100U)))
   {
      // only two bars are visible
      uint32 u32_UsedWidth;
      QColor c_SecondBarColor = this->mc_ColorFree;

      if ((this->mu32_PercentageUsed >= this->mu32_PercentageReserved) &&
          (this->mu32_PercentageUsed != 0U))
      {
         u32_UsedWidth = (100U * static_cast<uint32>(this->width())) / (10000U / this->mu32_PercentageUsed);

         if (this->mq_UsedOverReserved == false)
         {
            // special case: used size is equal to reserved size -> show usage bar only
            this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
         }
         else
         {
            // special case: used size is bigger than reserved size -> error
            this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_24);
         }
      }
      else if ((this->mu32_PercentageUsed == 0U) && (this->mu32_PercentageReserved > 0U))
      {
         // special case: only one reservation bar and no usage bar
         u32_UsedWidth = (100U * static_cast<uint32>(this->width())) / (10000U / this->mu32_PercentageReserved);
         this->mc_Brush.setColor(this->mc_ColorReserved);
      }
      else
      {
         // special case: reservation is 100% and usage bar has minimum 1% -> No error and no free area
         if (this->mu32_PercentageUsed != 0)
         {
            u32_UsedWidth = (100U * static_cast<uint32>(this->width())) / (10000U / this->mu32_PercentageUsed);
         }
         else
         {
            // shall not happen
            u32_UsedWidth = 1;
         }
         this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
         c_SecondBarColor = this->mc_ColorReserved;
      }

      // draw the used area
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 0, u32_UsedWidth, this->height());

      // two pixel space between used and free space bars
      // is there space for the second bar?
      if ((this->width() - static_cast<sintn>(u32_UsedWidth)) > 2)
      {
         // draw the free area
         this->mc_Brush.setColor(c_SecondBarColor);
         c_Painter.setBrush(this->mc_Brush);
         c_Painter.drawRect(static_cast<sintn>(u32_UsedWidth + 2U), 0, this->width(), this->height());
      }
   }
   else if ((this->mu32_PercentageUsed != 0) &&
            (this->mu32_PercentageReserved != 0))
   {
      // all three bars are visible

      // the available size without the space between the bars
      const sintn sn_UsableWidth = this->width() - 2;
      const sintn sn_UsedWidthUsed = (100 * sn_UsableWidth) /
                                     (10000 / static_cast<sintn>(this->mu32_PercentageUsed));
      const sintn sn_UsedWidthReserved = (100 * sn_UsableWidth) /
                                         (10000 / static_cast<sintn>(this->mu32_PercentageReserved));

      // draw the usage
      this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 0, sn_UsedWidthUsed, this->height());

      // draw the reservation
      this->mc_Brush.setColor(this->mc_ColorReserved);
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(sn_UsedWidthUsed + 2, 0, sn_UsedWidthReserved - (sn_UsedWidthUsed + 2), this->height());

      // two pixel space between used and free space bars
      // is there space for the second bar?
      if ((this->width() - sn_UsedWidthReserved) > 2)
      {
         // draw the free area
         this->mc_Brush.setColor(this->mc_ColorFree);
         c_Painter.setBrush(this->mc_Brush);
         c_Painter.drawRect(sn_UsedWidthReserved + 2, 0, this->width(), this->height());
      }
   }
   else
   {
      // shall not happen
   }
}

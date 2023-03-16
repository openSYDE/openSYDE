//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with minimal progress bar. (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>
#include <QPainter>

#include "stwtypes.hpp"
#include "constants.hpp"

#include "C_OgeWiProgressBar.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

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
C_OgeWiProgressBar::C_OgeWiProgressBar(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   ms32_SpacingWidth(2U),
   mu32_Percentage(0U),
   mq_Full(false)
{
   this->mc_Brush.setStyle(Qt::SolidPattern);
   this->mc_Pen.setColor(Qt::transparent);

   // Default colors
   this->SetColorFree(mc_STYLE_GUIDE_COLOR_10);
   this->SetColorReserved(mc_STYLE_GUIDE_COLOR_21);
   this->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_21);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color for the free space

   \param[in] orc_Color       Color for free space bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::SetColorFree(const QColor & orc_Color)
{
   this->mc_ColorFree = orc_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color for the reserved space

   \param[in] orc_Color       Color for free space bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::SetColorReserved(const QColor & orc_Color)
{
   this->mc_ColorReserved = orc_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the color if percentage is greater than 100%

   \param[in] orc_Color       Color for free space bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::SetColorTooMuch(const QColor & orc_Color)
{
   this->mc_ColorTooMuch = orc_Color;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the spacing width.

   Set the space between free bar and reserved bar, which has background color.

   \param[in]     os32_SpacingWidth   spacing width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::SetSpacingWidth(const int32_t os32_SpacingWidth)
{
   this->ms32_SpacingWidth = os32_SpacingWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current progress for showing

   \param[in]     ou32_Percentage   Percentage of progress
   \param[in]     oq_Full           Flag for using the too much color if the percentage value is 100.
                                    For compensating rounding problems.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::SetProgress(const uint32_t ou32_Percentage, const bool oq_Full)
{
   this->mu32_Percentage = ou32_Percentage;
   this->mq_Full = oq_Full;
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current progress

   \return
   Current progress as percentage
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OgeWiProgressBar::GetProgress(void) const
{
   return this->mu32_Percentage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the usage bar

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiProgressBar::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   QWidget::paintEvent(opc_Event);

   c_Painter.setPen(this->mc_Pen);

   if ((this->mu32_Percentage == 0U) ||
       (this->mu32_Percentage >= 100U))
   {
      // only one bar is visible

      if (this->mu32_Percentage == 0U)
      {
         // special case: nothing used
         this->mc_Brush.setColor(this->mc_ColorFree);
      }
      else if ((this->mu32_Percentage >= 100U) && (this->mq_Full == false))
      {
         // special case: full
         this->mc_Brush.setColor(this->mc_ColorReserved);
      }
      else
      {
         // special case: to much
         this->mc_Brush.setColor(this->mc_ColorTooMuch);
      }

      // draw the area
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 0, this->width(), this->height());
   }
   else
   {
      const uint32_t u32_UsedWidth = (100U * static_cast<uint32_t>(this->width())) / (10000U / this->mu32_Percentage);

      // draw the used area
      this->mc_Brush.setColor(this->mc_ColorReserved);
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 0, u32_UsedWidth, this->height());

      // space between used and free space bars
      // is there space for the second bar?
      if ((this->width() - static_cast<int32_t>(u32_UsedWidth)) > this->ms32_SpacingWidth)
      {
         // draw the free area
         this->mc_Brush.setColor(this->mc_ColorFree);
         c_Painter.setBrush(this->mc_Brush);
         c_Painter.drawRect(static_cast<int32_t>(u32_UsedWidth) + static_cast<int32_t>(ms32_SpacingWidth), 0,
                            this->width(), this->height());
      }
   }
}

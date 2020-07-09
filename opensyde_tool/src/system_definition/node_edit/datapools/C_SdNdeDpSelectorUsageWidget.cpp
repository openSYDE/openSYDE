//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar with multiple elements (implementation)

   The widget draws multiple bars for used and free space in the complete area.

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

#include "C_SdNdeDpSelectorUsageWidget.h"

#include "C_Uti.h"
#include "C_GtGetText.h"

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
C_SdNdeDpSelectorUsageWidget::C_SdNdeDpSelectorUsageWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_ToolTip(NULL),
   mu32_TotalUsed(0U),
   mu32_TotalPercentage(0U),
   mu32_Size(0U)
{
   // set default inactive color
   this->mc_ColorFree = stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, 20);

   this->mc_Brush.setStyle(Qt::SolidPattern);
   this->mc_Pen.setColor(Qt::transparent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorUsageWidget::~C_SdNdeDpSelectorUsageWidget(void)
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the actual percentage of the usage

   \param[in] ou32_Size       Maximum size
   \param[in] orc32_VecUsed   List of used space of all relevant datapools
   \param[in] orc_VecNames    List of names of all relevant datapools

   \return
   Calculated percentage
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeDpSelectorUsageWidget::SetUsage(const uint32 ou32_Size, const std::vector<uint32> & orc32_VecUsed,
                                              std::vector<QString> & orc_VecNames)
{
   uint32 u32_SumUsed = 0U;

   std::vector<uint32>::const_iterator c_ItUsed;

   // preprare the own vectors
   this->mc_Used.clear();
   this->mc_Percentages.clear();
   this->mc_PercentagesFloat.clear();
   this->mc_VecNames.clear();

   for (c_ItUsed = orc32_VecUsed.begin(); c_ItUsed != orc32_VecUsed.end(); ++c_ItUsed)
   {
      // calculate the complete used storage
      u32_SumUsed += *c_ItUsed;
   }

   this->mu32_TotalUsed = u32_SumUsed;

   // Calculate the percentage
   if ((u32_SumUsed == 0U) || (ou32_Size == 0U))
   {
      this->mu32_TotalPercentage = 0U;
   }
   else
   {
      uint32 u32_Counter = 0;
      uint32 u32_DataPoolPercentage;
      float32 f32_DataPoolPercentage;

      // Calculate the percentage of the sum of all datapools
      this->mu32_TotalPercentage = (u32_SumUsed * 100U) / ou32_Size;

      // Calculate the percentage of each datapool
      for (c_ItUsed = orc32_VecUsed.begin(); c_ItUsed != orc32_VecUsed.end(); ++c_ItUsed)
      {
         if ((*c_ItUsed) > 0U)
         {
            u32_DataPoolPercentage = ((*c_ItUsed) * 100U) / ou32_Size;
            f32_DataPoolPercentage =
               static_cast<float32>(((static_cast<float32>(*c_ItUsed) * 100.0)) / static_cast<float32>(ou32_Size));
            if (u32_DataPoolPercentage > 1U)
            {
               this->mc_Percentages.push_back(u32_DataPoolPercentage);
               this->mc_PercentagesFloat.push_back(f32_DataPoolPercentage);
            }
            else
            {
               // special case zero percent because of rounding, but the datapool has data
               this->mc_Percentages.push_back(1U);
               this->mc_PercentagesFloat.push_back(f32_DataPoolPercentage);
            }
            this->mc_Used.push_back(*c_ItUsed);
            this->mc_VecNames.push_back(orc_VecNames[u32_Counter]);
         }
         ++u32_Counter;
      }

      // prepare the colors
      this->mc_ColorsUsed.clear();
      if (this->mc_Percentages.size() > 1)
      {
         // more than one color necessary
         uint32 u32_DeltaColorScale;
         uint32 u32_ActualColorScale;

         // The scaling must be between 100% and 50%, starting at 100% and ending at 50%
         u32_ActualColorScale = 100U;
         u32_DeltaColorScale = (50U / (static_cast<uint32>(this->mc_Percentages.size()) - 1U));
         for (c_ItUsed = this->mc_Percentages.begin(); c_ItUsed != this->mc_Percentages.end(); ++c_ItUsed)
         {
            // update the scaling
            QColor c_Color = stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, u32_ActualColorScale);
            // for the next scaling
            u32_ActualColorScale -= u32_DeltaColorScale;

            // add the color
            this->mc_ColorsUsed.push_back(c_Color);
         }
      }
      else
      {
         // only one color is necessary
         this->mc_ColorsUsed.push_back(mc_STYLE_GUIDE_COLOR_7);
      }
   }

   // save the values for hint
   this->mu32_Size = ou32_Size;

   return this->mu32_TotalPercentage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the usage bars

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorUsageWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   QWidget::paintEvent(opc_Event);

   c_Painter.setPen(this->mc_Pen);

   // paint the background first
   this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
   c_Painter.setBrush(this->mc_Brush);
   c_Painter.drawRect(0, 0, this->width(), this->height());

   this->mc_Distances.clear();

   if ((this->mc_Percentages.size() == 0U) ||
       ((this->mu32_TotalPercentage >= 100U) && (this->mc_Percentages.size() == 1)))
   {
      // only one bar is visible

      if (this->mu32_TotalPercentage == 0U)
      {
         // special case: nothing used
         this->mc_Brush.setColor(this->mc_ColorFree);
      }
      else
      {
         // special case: full
         this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_7);
         this->mc_Distances.push_back(0);
      }

      // draw the area
      c_Painter.setBrush(this->mc_Brush);
      c_Painter.drawRect(0, 2, this->width(), this->height());
   }
   else
   {
      sintn sn_UsedWidthBegin = 0U;
      sintn sn_UsedWidth;
      uint32 u32_Counter;
      // the available size without the space between the bars
      const sintn sn_UsableWidth = this->width() - static_cast<sintn>(this->mc_Percentages.size() * 2U);

      for (u32_Counter = 0; u32_Counter < this->mc_Percentages.size(); ++u32_Counter)
      {
         sn_UsedWidth = (100 * sn_UsableWidth) /
                        static_cast<sintn>(10000U / this->mc_Percentages[u32_Counter]);

         // draw the used area
         this->mc_Brush.setColor(this->mc_ColorsUsed[u32_Counter]);
         c_Painter.setBrush(this->mc_Brush);
         if ((this->mu32_TotalPercentage < 100U) ||
             (u32_Counter < (static_cast<uint32>(this->mc_Percentages.size()) - 1U)))
         {
            c_Painter.drawRect(sn_UsedWidthBegin, 2, sn_UsedWidth, this->height());

            // save distances for the tooltip
            this->mc_Distances.push_back(sn_UsedWidthBegin);

            if (u32_Counter == (static_cast<uint32>(this->mc_Percentages.size()) - 1U))
            {
               // last used bar
               this->mc_Distances.push_back(sn_UsedWidthBegin + sn_UsedWidth);
            }
         }
         else
         {
            // special case: 100% and last bar
            // draw it to the end
            c_Painter.drawRect(sn_UsedWidthBegin, 2, (this->width() - sn_UsedWidthBegin), this->height());

            // save distances for the tooltip
            this->mc_Distances.push_back(sn_UsedWidthBegin);
            this->mc_Distances.push_back(this->width());
         }

         // for the next bar
         sn_UsedWidthBegin += sn_UsedWidth + 2;
      }

      // two pixel space between used and free space bars
      // is there space for the last bar?
      if (((this->width() - static_cast<sintn>(sn_UsedWidthBegin)) > 0) &&
          (this->mu32_TotalPercentage < 100U))
      {
         // draw the free area
         this->mc_Brush.setColor(this->mc_ColorFree);
         c_Painter.setBrush(this->mc_Brush);
         c_Painter.drawRect(static_cast<sintn>(sn_UsedWidthBegin), 2, this->width(), this->height());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorUsageWidget::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (mpc_ToolTip == NULL)
      {
         mpc_ToolTip = new C_NagToolTip();
      }

      if (this->mpc_ToolTip->isVisible() == false)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

         if (pc_HelpEvent != NULL)
         {
            this->setMouseTracking(true);

            this->m_UpdateTooltip(pc_HelpEvent->pos().x());
            this->mpc_ToolTip->show();
            this->mpc_ToolTip->DoMove(this->mapToGlobal(pc_HelpEvent->pos()));
         }
      }

      Q_EMIT this->SigHideOtherToolTips();
      opc_Event->accept();
      return true;
   }
   else if (opc_Event->type() == QEvent::Leave)
   {
      //hide on leave
      if (this->mpc_ToolTip != NULL)
      {
         this->mpc_ToolTip->hide();
      }

      this->setMouseTracking(false);

      opc_Event->accept();
      return true;
   }
   else
   {
      return QWidget::event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Update tooltip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorUsageWidget::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QWidget::mouseMoveEvent(opc_Event);

   if ((this->mpc_ToolTip != NULL) &&
       (this->mpc_ToolTip->isVisible() == true))
   {
      const sintn sn_PosX = this->mapFromGlobal(QCursor::pos()).x();
      this->m_UpdateTooltip(sn_PosX);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorUsageWidget::m_UpdateTooltip(const sintn osn_MouseX)
{
   if (this->mpc_ToolTip != NULL)
   {
      QString c_Text = QString("%1% %2 (%3 / %4)");
      QString c_DatapoolName;
      uint32 u32_UsedSize = 0U;
      float32 f32_ShowPercentage = 0.0F;
      bool q_ShowFree = true;
      QString c_Percentage;

      if (this->mc_Distances.size() == 0)
      {
         // special case, only one bar and it is completely free
         u32_UsedSize = this->mu32_Size - this->mu32_TotalUsed;
         f32_ShowPercentage = 100.0F;
      }
      else if (this->mc_Distances.size() == 1)
      {
         // special case, only one bar
         q_ShowFree = false;
         u32_UsedSize = this->mc_Used[0];
         f32_ShowPercentage = this->mc_PercentagesFloat[0];
         c_DatapoolName = this->mc_VecNames[0];
      }
      else
      {
         uint32 u32_Counter;

         // multiple bars, search the correct one
         for (u32_Counter = 1; u32_Counter < this->mc_Distances.size(); ++u32_Counter)
         {
            if (osn_MouseX <= static_cast<sintn>(this->mc_Distances[u32_Counter]))
            {
               const uint32 u32_Index = u32_Counter - 1U;
               u32_UsedSize = this->mc_Used[u32_Index];
               f32_ShowPercentage = this->mc_PercentagesFloat[u32_Index];
               c_DatapoolName = this->mc_VecNames[u32_Index];
               q_ShowFree = false;
               break;
            }
         }

         if (q_ShowFree == true)
         {
            // no datapool specific bar
            u32_UsedSize = this->mu32_Size - this->mu32_TotalUsed;
            f32_ShowPercentage = 100.0F - static_cast<float32>(this->mu32_TotalPercentage);
         }
      }

      c_Percentage.sprintf("%.2f", f32_ShowPercentage);

      if (q_ShowFree == true)
      {
         this->mpc_ToolTip->SetHeading(C_GtGetText::h_GetText("Free space"));

         c_Text = c_Text.arg(c_Percentage,
                             C_GtGetText::h_GetText("free"),
                             C_Uti::h_GetByteCountAsString(u32_UsedSize),
                             C_Uti::h_GetByteCountAsString(this->mu32_Size));
      }
      else
      {
         this->mpc_ToolTip->SetHeading(c_DatapoolName + " " + C_GtGetText::h_GetText("size"));

         c_Text = c_Text.arg(c_Percentage,
                             C_GtGetText::h_GetText("reserved"),
                             C_Uti::h_GetByteCountAsString(u32_UsedSize),
                             C_Uti::h_GetByteCountAsString(this->mu32_Size));
      }
      this->mpc_ToolTip->SetContent(c_Text);
   }
}

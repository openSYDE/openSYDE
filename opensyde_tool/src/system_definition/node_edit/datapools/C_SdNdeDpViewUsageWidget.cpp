//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar with multiple elements (implementation)

   The widget draws multiple bars for used and free space in the complete area.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QEvent>
#include <QHelpEvent>

#include "stwtypes.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"

#include "C_SdNdeDpViewUsageWidget.hpp"

#include "C_Uti.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SdNdeDpViewUsageWidget::C_SdNdeDpViewUsageWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_ToolTip(NULL),
   mu32_TotalUsed(0U),
   mu32_TotalPercentage(0U),
   mu32_Size(0U),
   mq_DataPoolHovered(false),
   mu32_HoveredDataPoolIndex(0U)
{
   // set default inactive color
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mc_ColorFree = stw::opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, 20);

   this->mc_Brush.setStyle(Qt::SolidPattern);
   this->mc_Pen.setColor(Qt::transparent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpViewUsageWidget::~C_SdNdeDpViewUsageWidget(void)
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the actual percentage of the usage

   \param[in]  ou32_NodeIndex     Node index of current node
   \param[in]  ou32_MaximumSize   Maximum size
   \param[in]  orc_Areas          List of used space of all relevant datapools
   \param[out] oru32_Percentage   Calculated percentage
   \param[out] oru32_UsedNvmSize  Calculated used memory size of the NVM
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewUsageWidget::SetUsage(const uint32_t ou32_NodeIndex, const uint32_t ou32_MaximumSize,
                                        const std::vector<C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas,
                                        uint32_t & oru32_Percentage, uint32_t & oru32_UsedNvmSize)
{
   uint32_t u32_AreaCounter;

   oru32_UsedNvmSize = 0U;

   // preprare the own vectors
   this->mc_VecUsageAreas.clear();

   // calculate the complete used storage
   for (u32_AreaCounter = 0U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
   {
      const C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea & rc_Area = orc_Areas[u32_AreaCounter];
      if (rc_Area.c_DataPoolIndexes.size() > 0)
      {
         // Areas with zero Datapools are free and not used gaps
         oru32_UsedNvmSize += rc_Area.u32_Size;
      }
   }

   this->mu32_TotalUsed = oru32_UsedNvmSize;

   // Calculate the percentage
   if ((oru32_UsedNvmSize == 0U) || (ou32_MaximumSize == 0U))
   {
      if (oru32_UsedNvmSize > ou32_MaximumSize)
      {
         this->mu32_TotalPercentage = 100U;
      }
      else
      {
         this->mu32_TotalPercentage = 0U;
      }
   }
   else
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         uint32_t u32_CorrectDatapoolAreas = 0U;
         // more than one color necessary
         uint32_t u32_DeltaColorScale = 0U;
         uint32_t u32_ActualColorScale = 100U;

         // Calculate the percentage of the sum of all datapools
         this->mu32_TotalPercentage = (oru32_UsedNvmSize * 100U) / ou32_MaximumSize;

         // Count the areas with one Datapool for calculating the correct colors
         for (u32_AreaCounter = 0U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
         {
            if (orc_Areas[u32_AreaCounter].c_DataPoolIndexes.size() == 1)
            {
               ++u32_CorrectDatapoolAreas;
            }
         }

         // The scaling must be between 100% and 50%, starting at 100% and ending at 50%
         if (u32_CorrectDatapoolAreas > 1)
         {
            u32_DeltaColorScale = (50U / (u32_CorrectDatapoolAreas - 1U));
         }

         // Calculate the percentage of each datapool
         for (u32_AreaCounter = 0U; u32_AreaCounter < orc_Areas.size(); ++u32_AreaCounter)
         {
            const C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea & rc_Area = orc_Areas[u32_AreaCounter];
            C_SdNdeDpViewUsageArea c_UsageArea;

            if (rc_Area.u32_Size > 0U)
            {
               const uint32_t u32_AreaPercentage = (rc_Area.u32_Size * 100U) / ou32_MaximumSize;
               const float32_t f32_AreaPercentage =
                  static_cast<float32_t>(((static_cast<float32_t>(rc_Area.u32_Size) * 100.0)) /
                                         static_cast<float32_t>(ou32_MaximumSize));

               if (u32_AreaPercentage > 1U)
               {
                  c_UsageArea.u32_Percentage = u32_AreaPercentage;
               }
               else
               {
                  // special case zero percent because of rounding, but the Datapool has data
                  c_UsageArea.u32_Percentage = 1U;
               }
               c_UsageArea.f32_PercentageFloat = f32_AreaPercentage;

               if (rc_Area.c_DataPoolIndexes.size() == 1)
               {
                  // Unique Datapool. Set the values of the Datapool
                  // In case of overlapped areas, the values must not be recalculated for the tooltip
                  c_UsageArea.u32_StartAddressDatapool =
                     pc_Node->c_DataPools[rc_Area.c_DataPoolIndexes[0]].u32_NvmStartAddress;
                  c_UsageArea.u32_SizeDatapool = pc_Node->c_DataPools[rc_Area.c_DataPoolIndexes[0]].u32_NvmSize;
                  c_UsageArea.f32_PercentageFloatDatapool =
                     static_cast<float32_t>(((static_cast<float32_t>(c_UsageArea.u32_SizeDatapool) * 100.0)) /
                                            static_cast<float32_t>(ou32_MaximumSize));
               }
               else
               {
                  // No unique Datapool, values are set to values of the area
                  c_UsageArea.u32_StartAddressDatapool = rc_Area.u32_StartAddress;
                  c_UsageArea.u32_SizeDatapool = rc_Area.u32_Size;
                  c_UsageArea.f32_PercentageFloatDatapool = c_UsageArea.f32_PercentageFloat;
               }

               c_UsageArea.c_NvmArea = rc_Area;

               if (rc_Area.c_DataPoolIndexes.size() == 1)
               {
                  uint32_t u32_NextAreaCounter;
                  bool q_AdaptColor = true;

                  tgl_assert(rc_Area.c_DataPoolIndexes[0] < pc_Node->c_DataPools.size());
                  c_UsageArea.c_ShowedString = pc_Node->c_DataPools[rc_Area.c_DataPoolIndexes[0]].c_Name.c_str();
                  c_UsageArea.q_Used = true;
                  c_UsageArea.c_Color = stw::opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7,
                                                                                     u32_ActualColorScale);

                  // for the next scaling
                  // Check if the next one is an overlap and the next after next is the same Datapool to have the
                  // same color if the current Datapool will be separated in multiple areas by overlapped Datapools
                  // in between
                  for (u32_NextAreaCounter = u32_AreaCounter + 1U; u32_NextAreaCounter < orc_Areas.size();
                       ++u32_NextAreaCounter)
                  {
                     const C_PuiSdHandler::C_PuiSdHandlerNodeLogicNvmArea & rc_NextArea =
                        orc_Areas[u32_NextAreaCounter];

                     // Checking for areas with at least two Datapools is not necessary due to overlapping areas
                     // In case of a area with 0 or 1 Datapool no further areas has to be checked
                     if (rc_NextArea.c_DataPoolIndexes.size() <= 1)
                     {
                        if ((rc_NextArea.c_DataPoolIndexes.size() == 1) &&
                            (rc_NextArea.c_DataPoolIndexes[0] == rc_Area.c_DataPoolIndexes[0]))
                        {
                           // Same Datapool in following areas. Do not change the color
                           q_AdaptColor = false;
                        }

                        break;
                     }
                  }

                  if (q_AdaptColor == true)
                  {
                     u32_ActualColorScale -= u32_DeltaColorScale;
                  }
               }
               else if (rc_Area.c_DataPoolIndexes.size() == 0)
               {
                  c_UsageArea.c_ShowedString = "";
                  c_UsageArea.q_Used = false;
                  c_UsageArea.c_Color = this->mc_ColorFree;
               }
               else
               {
                  // Overlapping NVM Datapools
                  uint32_t u32_DatapoolCounter;
                  QString c_DatapoolNames;

                  for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < rc_Area.c_DataPoolIndexes.size();
                       ++u32_DatapoolCounter)
                  {
                     if (u32_DatapoolCounter == 0U)
                     {
                        c_DatapoolNames += C_GtGetText::h_GetText("Overlapping between:\n");
                     }
                     else
                     {
                        c_DatapoolNames += ", ";
                     }
                     tgl_assert(rc_Area.c_DataPoolIndexes[u32_DatapoolCounter] < pc_Node->c_DataPools.size());
                     c_UsageArea.c_ShowedString =
                        c_DatapoolNames += static_cast<QString>(
                           pc_Node->c_DataPools[rc_Area.c_DataPoolIndexes[u32_DatapoolCounter]].c_Name.c_str());
                  }

                  c_UsageArea.c_ShowedString = c_DatapoolNames;
                  c_UsageArea.q_Used = true;
                  c_UsageArea.c_Color = mc_STYLE_GUIDE_COLOR_24;
               }

               this->mc_VecUsageAreas.push_back(c_UsageArea);
            }
         }
      }
   }

   // save the values for hint
   this->mu32_Size = ou32_MaximumSize;

   oru32_Percentage = this->mu32_TotalPercentage;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a changed Datapool hover state

   \param[in]       ou32_DataPoolIndex    Index of the hovered Datapool
   \param[in]       oq_Hovered            Flag if the Datapool is now hovered or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewUsageWidget::DataPoolHoverStateChanged(const uint32_t ou32_DataPoolIndex, const bool oq_Hovered)
{
   this->mq_DataPoolHovered = oq_Hovered;
   this->mu32_HoveredDataPoolIndex = ou32_DataPoolIndex;
   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the usage bars

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewUsageWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   QWidget::paintEvent(opc_Event);

   c_Painter.setPen(this->mc_Pen);

   // paint the background first
   this->mc_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
   c_Painter.setBrush(this->mc_Brush);
   c_Painter.drawRect(0, 0, this->width(), this->height());

   this->mc_Distances.clear();

   if ((this->mc_VecUsageAreas.size() == 0U) ||
       ((this->mu32_TotalPercentage >= 100U) && (this->mc_VecUsageAreas.size() == 1)))
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
      c_Painter.drawRect(0, 0, this->width(), this->height());
   }
   else
   {
      int32_t s32_UsedWidthBegin = 0U;
      uint32_t u32_AreaCounter;
      // the available size without the space between the bars
      const int32_t s32_UsableWidth = this->width() - static_cast<int32_t>(this->mc_VecUsageAreas.size() * 2U);

      for (u32_AreaCounter = 0; u32_AreaCounter < this->mc_VecUsageAreas.size(); ++u32_AreaCounter)
      {
         const C_SdNdeDpViewUsageArea & rc_UsageArea = this->mc_VecUsageAreas[u32_AreaCounter];
         bool q_DrawHovered = false;
         bool q_AreaAfterHasSameDps = false;
         int32_t s32_DrawnAreaWidth;
         int32_t s32_UsedWidth;

         int32_t s32_AreaWidth;
         s32_UsedWidth = (100 * s32_UsableWidth) /
                         static_cast<int32_t>(10000U / rc_UsageArea.u32_Percentage);

         // draw the used area
         this->mc_Brush.setColor(rc_UsageArea.c_Color);
         c_Painter.setBrush(this->mc_Brush);
         if (u32_AreaCounter < (static_cast<uint32_t>(this->mc_VecUsageAreas.size()) - 1U))
         {
            s32_AreaWidth = s32_UsedWidth;

            // save distances for the tooltip
            this->mc_Distances.push_back(s32_UsedWidthBegin);

            if (u32_AreaCounter == (static_cast<uint32_t>(this->mc_VecUsageAreas.size()) - 1U))
            {
               // last used bar
               this->mc_Distances.push_back(s32_UsedWidthBegin + s32_UsedWidth);
            }
         }
         else
         {
            // special case: 100% and last bar
            // draw it to the end
            s32_AreaWidth = this->width() - s32_UsedWidthBegin;

            // save distances for the tooltip
            this->mc_Distances.push_back(s32_UsedWidthBegin);
            this->mc_Distances.push_back(this->width());
         }

         if (this->mq_DataPoolHovered == true)
         {
            // Check if the current area is part of the current hovered Datapool
            q_DrawHovered = this->m_CheckForContainingDataPool(u32_AreaCounter, this->mu32_HoveredDataPoolIndex);
         }

         // Calculating and drawing the concrete rectangles
         s32_DrawnAreaWidth = s32_AreaWidth;

         if (u32_AreaCounter < static_cast<uint32_t>(this->mc_VecUsageAreas.size() - 1U))
         {
            // Check the area after for overlapping Datapools
            q_AreaAfterHasSameDps = this->m_CheckForContainingDataPool(u32_AreaCounter + 1U,
                                                                       rc_UsageArea.c_NvmArea.c_DataPoolIndexes);
         }

         if ((q_AreaAfterHasSameDps == false) &&
             (u32_AreaCounter != (static_cast<uint32_t>(this->mc_VecUsageAreas.size() - 1U))))
         {
            s32_DrawnAreaWidth -= 2;
         }

         if (q_DrawHovered == false)
         {
            c_Painter.drawRect(s32_UsedWidthBegin, 0, s32_DrawnAreaWidth, this->height());
         }
         else
         {
            // Handle a hovered Datapool
            int32_t s32_HorizontalBegin = s32_UsedWidthBegin;
            bool q_AreaBeforeWithHoveredDp = false;
            bool q_AreaAfterWithHoveredDp = false;

            // Check if the area is only a part of the hovered Datapool
            if (u32_AreaCounter > 0U)
            {
               // Check the area before
               q_AreaBeforeWithHoveredDp = this->m_CheckForContainingDataPool(u32_AreaCounter - 1U,
                                                                              this->mu32_HoveredDataPoolIndex);
            }

            if (u32_AreaCounter < static_cast<uint32_t>(this->mc_VecUsageAreas.size() - 1U))
            {
               // Check the area after
               q_AreaAfterWithHoveredDp =
                  this->m_CheckForContainingDataPool(u32_AreaCounter + 1U, this->mu32_HoveredDataPoolIndex);
            }

            // Adapt the bars for not having gaps
            if (q_AreaBeforeWithHoveredDp == false)
            {
               s32_HorizontalBegin += 1;
               s32_DrawnAreaWidth -= 1;
            }
            if (q_AreaAfterWithHoveredDp == false)
            {
               s32_DrawnAreaWidth -= 1;
            }

            c_Painter.drawRect(s32_HorizontalBegin, 1, s32_DrawnAreaWidth, (this->height() - 2));
         }

         // for the next bar
         s32_UsedWidthBegin += s32_AreaWidth;
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
bool C_SdNdeDpViewUsageWidget::event(QEvent * const opc_Event)
{
   bool q_Return = true;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (mpc_ToolTip == NULL)
      {
         mpc_ToolTip = new C_NagToolTip();
      }

      if (this->mpc_ToolTip->isVisible() == false)
      {
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
   }
   else
   {
      q_Return = QWidget::event(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Update tooltip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewUsageWidget::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QWidget::mouseMoveEvent(opc_Event);

   const int32_t s32_PosHorizontal = this->mapFromGlobal(QCursor::pos()).x();

   if ((this->mpc_ToolTip != NULL) &&
       (this->mpc_ToolTip->isVisible() == true))
   {
      this->m_UpdateTooltip(s32_PosHorizontal);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the index of the usage area with the specific position

   \param[in]       os32_PosHorizontal     Position of mouse
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDpViewUsageWidget::m_GetUsageAreaIndex(const int32_t os32_PosHorizontal)
{
   uint32_t u32_Index = 0U;

   if (this->mc_Distances.size() > 1)
   {
      uint32_t u32_Counter;

      // multiple bars, search the correct one
      for (u32_Counter = 1; u32_Counter < this->mc_Distances.size(); ++u32_Counter)
      {
         if (os32_PosHorizontal <= this->mc_Distances[u32_Counter])
         {
            u32_Index = static_cast<int32_t>(u32_Counter - 1U);
            break;
         }
      }
   }

   return u32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the Datapool is part of the area

   \param[in]       ou32_DataPoolIndex     Datapool index of searched Datapool

   \retval   true   The Datapool is part of the area
   \retval   false  The Datapool is not part of the area
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpViewUsageWidget::m_CheckForContainingDataPool(const uint32_t ou32_AreaIndex,
                                                            const uint32_t ou32_DataPoolIndex)
{
   bool q_Return = false;
   const C_SdNdeDpViewUsageArea & rc_Area = this->mc_VecUsageAreas[ou32_AreaIndex];
   uint32_t u32_DatapoolCounter;

   for (u32_DatapoolCounter = 0; u32_DatapoolCounter < rc_Area.c_NvmArea.c_DataPoolIndexes.size();
        ++u32_DatapoolCounter)
   {
      if (rc_Area.c_NvmArea.c_DataPoolIndexes[u32_DatapoolCounter] == ou32_DataPoolIndex)
      {
         q_Return = true;
         break;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if one of the Datapools is part of the area

   \param[in]       orc_DataPoolIndexes     Datapool indexes of searched Datapools

   \retval   true   The Datapool is part of the area
   \retval   false  The Datapool is not part of the area
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpViewUsageWidget::m_CheckForContainingDataPool(const uint32_t ou32_AreaIndex,
                                                            const std::vector<uint32_t> & orc_DataPoolIndexes)
{
   bool q_Return = false;
   uint32_t u32_DpIndexCounter;

   for (u32_DpIndexCounter = 0; u32_DpIndexCounter < orc_DataPoolIndexes.size(); ++u32_DpIndexCounter)
   {
      q_Return = this->m_CheckForContainingDataPool(ou32_AreaIndex,
                                                    orc_DataPoolIndexes[u32_DpIndexCounter]);

      if (q_Return == true)
      {
         break;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpViewUsageWidget::m_UpdateTooltip(const int32_t os32_MouseHorizontal)
{
   if (this->mpc_ToolTip != NULL)
   {
      QString c_Description;
      QString c_MemoryState;
      uint32_t u32_UsedSize = 0U;
      uint32_t u32_ShowStartAddress = 0U;
      float32_t f32_ShowPercentage = 0.0F;
      bool q_ShowFree = true;
      QString c_Percentage;
      QString c_Text;
      bool q_ErrorCase = false;
      uint32_t u32_OverlappedBytes = 0U;

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
         u32_UsedSize = this->mc_VecUsageAreas[0].c_NvmArea.u32_Size;
         f32_ShowPercentage = this->mc_VecUsageAreas[0].f32_PercentageFloat;
         c_Description = this->mc_VecUsageAreas[0].c_ShowedString;
      }
      else
      {
         const uint32_t u32_UsageAreaIndex = m_GetUsageAreaIndex(os32_MouseHorizontal);
         // multiple bars, use the hoverd one
         // Using the concrete Datapool information in case of fragmented areas for the Datapool
         if (this->mc_VecUsageAreas[u32_UsageAreaIndex].c_NvmArea.c_DataPoolIndexes.size() == 1)
         {
            u32_ShowStartAddress = this->mc_VecUsageAreas[u32_UsageAreaIndex].u32_StartAddressDatapool;
            u32_UsedSize = this->mc_VecUsageAreas[u32_UsageAreaIndex].u32_SizeDatapool;
            f32_ShowPercentage = this->mc_VecUsageAreas[u32_UsageAreaIndex].f32_PercentageFloatDatapool;
         }
         else
         {
            u32_ShowStartAddress = this->mc_VecUsageAreas[u32_UsageAreaIndex].c_NvmArea.u32_StartAddress;
            u32_UsedSize = this->mc_VecUsageAreas[u32_UsageAreaIndex].c_NvmArea.u32_Size;
            f32_ShowPercentage = this->mc_VecUsageAreas[u32_UsageAreaIndex].f32_PercentageFloat;
         }
         c_Description = this->mc_VecUsageAreas[u32_UsageAreaIndex].c_ShowedString;
         q_ShowFree = !this->mc_VecUsageAreas[u32_UsageAreaIndex].q_Used;

         // Tooltip of concrete errorcase
         q_ErrorCase = (this->mc_VecUsageAreas[u32_UsageAreaIndex].c_NvmArea.c_DataPoolIndexes.size() > 1);
         if (q_ErrorCase == true)
         {
            u32_OverlappedBytes = this->mc_VecUsageAreas[u32_UsageAreaIndex].c_NvmArea.u32_Size;
         }
      }

      c_Percentage = QString::number(f32_ShowPercentage, 'f', 2);

      // Prepare the text
      c_Text = static_cast<QString>(C_GtGetText::h_GetText("Start Address: "));
      c_Text += "%1\n";

      if (q_ShowFree == true)
      {
         this->mpc_ToolTip->SetHeading(C_GtGetText::h_GetText("Free space"));
         c_MemoryState = C_GtGetText::h_GetText("free");
         this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);

         c_Text += "%2% %3 (%4 / %5)";
      }
      else if (q_ErrorCase == true)
      {
         this->mpc_ToolTip->SetHeading(c_Description);
         c_MemoryState = C_GtGetText::h_GetText("overlapped");
         this->mpc_ToolTip->SetType(C_NagToolTip::eERROR);

         c_Text += "%2 ";
         c_Text += static_cast<QString>(C_GtGetText::h_GetText("Bytes"));
         c_Text += " %3";
      }
      else
      {
         this->mpc_ToolTip->SetHeading(c_Description);
         c_MemoryState = C_GtGetText::h_GetText("reserved");
         this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);

         c_Text += static_cast<QString>(C_GtGetText::h_GetText("Used: "));
         c_Text += "%2% %3 (%4 / %5)";
      }

      if (q_ErrorCase == false)
      {
         c_Text = c_Text.arg(QString::number(u32_ShowStartAddress),
                             c_Percentage,
                             c_MemoryState,
                             C_Uti::h_GetByteCountAsString(u32_UsedSize),
                             C_Uti::h_GetByteCountAsString(this->mu32_Size));
      }
      else
      {
         c_Text = c_Text.arg(QString::number(u32_ShowStartAddress),
                             QString::number(u32_OverlappedBytes),
                             c_MemoryState);
      }

      this->mpc_ToolTip->SetContent(c_Text);
   }
}

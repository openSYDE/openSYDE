//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar on datapool list header (implementation)

   The widget draws two bars for used and free space in the complete area.

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

#include "C_SdNdeDpListHeaderUsageWidget.h"

#include "C_Uti.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
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
C_SdNdeDpListHeaderUsageWidget::C_SdNdeDpListHeaderUsageWidget(QWidget * const opc_Parent) :
   C_OgeWiProgressBar(opc_Parent),
   mq_ActivateToolTip(true),
   mpc_ToolTip(NULL),
   mu32_Size(0),
   mu32_Used(0),
   mq_ErrorToolTip(false)
{
   // set default inactive color
   this->SetColorFree(stw_opensyde_gui_logic::C_Uti::h_ScaleColor(mc_STYLE_GUIDE_COLOR_7, 20));
   this->SetColorReserved(mc_STYLE_GUIDE_COLOR_7);
   this->SetColorTooMuch(mc_STYLE_GUIDE_COLOR_24);

   // init the tooltip
   this->SetToolTipWidgetName(QString(C_GtGetText::h_GetText("Datapool")));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListHeaderUsageWidget::~C_SdNdeDpListHeaderUsageWidget()
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the actual percentage of the usage

   \param[in] ou32_Size       Maximum size
   \param[in] ou32_Used       Absolute used space of the available space

   \return
   Calculated percentage
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeDpListHeaderUsageWidget::SetUsage(const uint32 ou32_Size, const uint32 ou32_Used)
{
   uint32 u32_Percentage;
   bool q_Full = false;

   // Calculate the percentage
   if ((ou32_Used == 0U) || (ou32_Size == 0U))
   {
      u32_Percentage = 0U;
   }
   else
   {
      u32_Percentage = (ou32_Used * 100U) / ou32_Size;

      if (ou32_Used > ou32_Size)
      {
         q_Full = true;
      }
      else
      {
         q_Full = false;
      }
   }

   // save the values for hint
   this->mu32_Size = ou32_Size;
   this->mu32_Used = ou32_Used;

   this->SetProgress(u32_Percentage, q_Full);

   return u32_Percentage;
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
bool C_SdNdeDpListHeaderUsageWidget::event(QEvent * const opc_Event)
{
   if ((opc_Event->type() == QEvent::ToolTip) && (this->mq_ActivateToolTip == true))
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
            const QString c_Text = QString("%1% %2 (%3 / %4)").arg(QString::number(this->GetProgress()),
                                                                   C_GtGetText::h_GetText("used"),
                                                                   C_Uti::h_GetByteCountAsString(this->mu32_Used),
                                                                   C_Uti::h_GetByteCountAsString(this->mu32_Size));
            if (this->mq_ErrorToolTip == true)
            {
               this->mpc_ToolTip->SetHeading(this->mc_ToolTipErrorHeading);
               this->mpc_ToolTip->SetContent(this->mc_ToolTipErrorContent);
               this->mpc_ToolTip->SetType(C_NagToolTip::eERROR);
            }
            else
            {
               this->mpc_ToolTip->SetHeading(this->mc_ToolTipHeading);
               this->mpc_ToolTip->SetContent(c_Text);
               this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);
            }
            this->mpc_ToolTip->show();
            this->mpc_ToolTip->DoMove(this->mapToGlobal(pc_HelpEvent->pos()));
         }
      }

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

      opc_Event->accept();
      return true;
   }
   else
   {
      return QWidget::event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set displayed tool tip widget name

   \param[in] orc_Value New tool tip widget name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderUsageWidget::SetToolTipWidgetName(const QString & orc_Value)
{
   this->mc_ToolTipHeading = orc_Value + QString(C_GtGetText::h_GetText(" Usage"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set error tooltip override

   \param[in] orc_Heading Heading
   \param[in] orc_Content Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListHeaderUsageWidget::SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content)
{
   this->mc_ToolTipErrorHeading = orc_Heading;
   this->mc_ToolTipErrorContent = orc_Content;
   if ((orc_Heading.compare("") == 0) && (orc_Content.compare("") == 0))
   {
      this->mq_ErrorToolTip = false;
   }
   else
   {
      this->mq_ErrorToolTip = true;
   }
}

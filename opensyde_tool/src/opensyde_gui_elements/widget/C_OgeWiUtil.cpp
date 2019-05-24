//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for QWidget (implementation)

   Assortment of utility functions for QWidget

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>

#include <QPainter>
#include <QFileInfo>
#include <QApplication>
#include <QStyleOption>
#include <QDesktopWidget>

#include "C_OSCUtils.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Draw background for widget (once)

   \param[in,out] opc_Widget  Widget to draw background for
   \param[in,out] opc_Painter Optional painter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_DrawBackground(QWidget * const opc_Widget, QPainter * const opc_Painter)
{
   //draw background
   QStyleOption c_StyleOption;

   c_StyleOption.initFrom(opc_Widget);
   //Draw background with stylesheet settings
   if (opc_Painter != NULL)
   {
      opc_Widget->style()->drawPrimitive(QStyle::PE_Widget, &c_StyleOption, opc_Painter, opc_Widget);
   }
   else
   {
      QPainter c_Painter(opc_Widget);
      opc_Widget->style()->drawPrimitive(QStyle::PE_Widget, &c_StyleOption, &c_Painter, opc_Widget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheet property

   Apply stylesheet property and force update of style
   For more information see: https://wiki.qt.io/Dynamic_Properties_and_Stylesheets

   \param[in,out] opc_Widget  Widget where to set property
   \param[in,out] opcn_Name   Property name
   \param[in,out] orc_Value   Property value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_ApplyStylesheetProperty(QWidget * const opc_Widget, const stw_types::charn * const opcn_Name,
                                            const QVariant & orc_Value)
{
   //set property
   opc_Widget->setProperty(opcn_Name, orc_Value);

   //update style
   opc_Widget->style()->unpolish(opc_Widget);
   opc_Widget->style()->polish(opc_Widget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply stylesheet property to widget AND all children (recursive)

   \param[in,out] opc_Widget  Widget where to set property
   \param[in,out] opcn_Name   Property name
   \param[in,out] orc_Value   Property value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(QWidget * const opc_Widget,
                                                                  const stw_types::charn * const opcn_Name,
                                                                  const QVariant & orc_Value)
{
   h_ApplyStylesheetProperty(opc_Widget, opcn_Name, orc_Value);
   //Children
   for (stw_types::sint32 s32_ItChild = 0; s32_ItChild < opc_Widget->children().size(); ++s32_ItChild)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QWidget * const pc_Wid = dynamic_cast<QWidget * const>(opc_Widget->children()[s32_ItChild]);
      if (pc_Wid != NULL)
      {
         h_ApplyStylesheetPropertyToItselfAndAllChildren(pc_Wid, opcn_Name, orc_Value);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set window icon

   \param[in,out] opc_Widget Widget to set icon for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_SetWindowIcon(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      opc_Widget->setWindowIcon(QIcon("://images/openSYDE_logo.ico"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to check if widget inside allowed area

   \param[in,out] orc_GlobalPosition          Global position of top left corner (Set to default if necessary)
   \param[in,out] orc_Size                    Size of widget (Set to default if necessary)
   \param[in]     orc_DefaultSize             Default size of widget
   \param[in]     oq_AddLogEntryForWindowSize Optional flag to add log entry for window size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_CheckAndFixDialogPositionAndSize(QPoint & orc_GlobalPosition, QSize & orc_Size,
                                                     const QSize & orc_DefaultSize,
                                                     const bool oq_AddLogEntryForWindowSize)
{
   const QDesktopWidget * const pc_Desktop = QApplication::desktop();

   // Desktop size gets increase by one on each side
   //  because the check seems to exclude the bottom right corner if exact match
   const QRect c_RectDesktop = pc_Desktop->geometry().adjusted(-1, -1, 2, 2);

   // are the coordinates inside of all screen ares?
   if ((c_RectDesktop.contains(orc_GlobalPosition) == true) &&
       (c_RectDesktop.contains((orc_GlobalPosition +
                                QPoint(orc_Size.width(), orc_Size.height()))) == true))
   {
      // it fits into the screen
   }
   else
   {
      // it does not fit into the screen. restore to default values
      orc_GlobalPosition = QPoint(10, 30);
      orc_Size = orc_DefaultSize;
   }
   if (oq_AddLogEntryForWindowSize)
   {
      osc_write_log_info("Setup main window screen",
                         QString("Setup main window for screen width %1, height %2").arg(
                            pc_Desktop->geometry().width()).arg(pc_Desktop->
                                                                geometry()
                                                                .height()).toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update font depending on size

   \param[in] opc_Widget              Widget to adapt
   \param[in] orc_Text                Current text
   \param[in] of32_HeightScaling      Scaling factor for widget height
   \param[in] oq_IgnoreContentMargins Optional flag to ignore content margins
   \param[in] opc_ImprovedSize        Optional size to use for the widget (does ignore content margins!)

   \return
   -1   Error
   Else New point size
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeWiUtil::h_UpdateFontSize(QWidget * const opc_Widget, const QString & orc_Text,
                                    const float32 of32_HeightScaling, const bool oq_IgnoreContentMargins,
                                    const QSize * const opc_ImprovedSize)
{
   sintn sn_Retval = -1;

   if ((orc_Text.compare("") != 0) && (opc_Widget != NULL))
   {
      QFont c_Font = opc_Widget->font();
      QSize c_EffectiveSize;
      sintn sn_NewPointSize;
      if (opc_ImprovedSize != NULL)
      {
         c_EffectiveSize = *opc_ImprovedSize;
      }
      else
      {
         //Consider margins
         if (oq_IgnoreContentMargins == true)
         {
            c_EffectiveSize = opc_Widget->size();
         }
         else
         {
            sintn sn_MarginLeft;
            sintn sn_MarginTop;
            sintn sn_MarginRight;
            sintn sn_MarginBottom;
            opc_Widget->getContentsMargins(&sn_MarginLeft, &sn_MarginTop, &sn_MarginRight, &sn_MarginBottom);
            c_EffectiveSize = QSize(opc_Widget->size().width() - (sn_MarginLeft + sn_MarginRight),
                                    opc_Widget->size().height() - (sn_MarginTop + sn_MarginBottom));
         }
      }
      sn_NewPointSize = C_OgeWiUtil::h_GetNextOptimalPointSize(c_Font, c_EffectiveSize, orc_Text,
                                                               of32_HeightScaling);
      sn_Retval = sn_NewPointSize;
      c_Font.setPointSize(sn_NewPointSize);
      opc_Widget->setFont(c_Font);
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get optimal point size based on parameters

   \param[in] orc_Font           Current font
   \param[in] orc_Size           New size
   \param[in] orc_Text           Current text
   \param[in] of32_HeightScaling Scaling factor for widget height

   \return
   Font point size
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_OgeWiUtil::h_GetNextOptimalPointSize(const QFont & orc_Font, const QSize & orc_Size, const QString & orc_Text,
                                             const float32 of32_HeightScaling)
{
   sintn sn_Retval;
   static QMap<QString, sintn> hc_PreviousResults;
   //Find some way to uniquely identify the input parameter constellation
   const QString c_CompleteInput = QString("%1,%2,%3,%4,%5").arg(orc_Font.toString()).arg(orc_Size.width()).arg(
      orc_Size.height()).arg(orc_Text).arg(static_cast<float64>(of32_HeightScaling));
   //Look up
   const QMap<QString, sintn>::const_iterator c_It = hc_PreviousResults.find(c_CompleteInput);

   //Check if anything found
   if (c_It == hc_PreviousResults.end())
   {
      const float32 f32_WidgetHeight = static_cast<float32>(orc_Size.height()) * of32_HeightScaling;
      const sintn sn_WidgetHeight = static_cast<sintn>(f32_WidgetHeight);
      const sintn sn_Init = std::max(std::max(orc_Font.pointSize(), orc_Font.pixelSize()), 1);
      QFont c_Font(orc_Font.family(), sn_Init, orc_Font.weight(), orc_Font.italic());

      sn_Retval = sn_Init;
      for (sintn sn_StepWidth = 2; sn_StepWidth > 0; --sn_StepWidth)
      {
         //Check size increase (iterative)
         for (; sn_Retval < 2000; sn_Retval += sn_StepWidth)
         {
            c_Font.setPointSize(sn_Retval);
            {
               const QFontMetrics c_Metric(c_Font);
               if ((c_Metric.width(orc_Text) > orc_Size.width()) || (c_Metric.height() > sn_WidgetHeight))
               {
                  //Assuming previous one was ok
                  sn_Retval -= sn_StepWidth;
                  break;
               }
            }
         }
         //Check if size increase did at least one iteration
         //(We don't need to iterate in the other direction if increase already worked)
         if (sn_Retval < sn_Init)
         {
            //Undo last step
            sn_Retval += sn_StepWidth;
            //Check size decrease (iterative)
            for (; sn_Retval > sn_StepWidth; sn_Retval -= sn_StepWidth)
            {
               c_Font.setPointSize(sn_Retval);
               {
                  const QFontMetrics c_Metric(c_Font);
                  if ((c_Metric.width(orc_Text) <= orc_Size.width()) && (c_Metric.height() <= sn_WidgetHeight))
                  {
                     break;
                  }
               }
            }
         }
      }
      //Store new value for further calls
      hc_PreviousResults.insert(c_CompleteInput, sn_Retval);
   }
   else
   {
      sn_Retval = c_It.value();
   }

   return std::max(sn_Retval, 1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Logic to get the next widget to gray out

   \param[in] opc_Input Widget to search the parent hierarchy for

   \return
   Either highest parent or the one child below the next pop up which is also parent of the input widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_OgeWiUtil::h_GetWidgetUnderNextPopUp(QWidget * const opc_Input)
{
   QWidget * pc_Retval = NULL;

   if (opc_Input != NULL)
   {
      if (opc_Input->parentWidget() == NULL)
      {
         pc_Retval = opc_Input;
      }
      else
      {
         QWidget * pc_Child = opc_Input;
         QWidget * pc_Parent = opc_Input->parentWidget();
         do
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            const stw_opensyde_gui_elements::C_OgePopUpDialog * const pc_PopUp =
               dynamic_cast<const stw_opensyde_gui_elements::C_OgePopUpDialog * const>(pc_Parent);
            if (pc_PopUp == NULL)
            {
               //Current candidate for return
               pc_Retval = pc_Parent;
               //Continue with next value
               pc_Child = pc_Parent;
               pc_Parent = pc_Child->parentWidget();
            }
            else
            {
               //Stop here
               pc_Retval = pc_Child;
               break;
            }
         }
         while (pc_Parent != NULL);
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to check if the requested key is part of the global key handling

   \param[in] opc_Event Event information

   \return
   True  Global key handling requested
   False No special global key handling required
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeWiUtil::h_CheckGlobalKey(const QKeyEvent * const opc_Event)
{
   bool q_Retval;

   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_S)) &&
       (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      q_Retval = true;
   }
   else if ((opc_Event->key() == static_cast<sintn>(Qt::Key_F)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      q_Retval = true;
   }
   else if (opc_Event->key() == static_cast<sintn>(Qt::Key_F12))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get save file name via QFileDialog

   \param[in,out] opc_Parent          Parent widget
   \param[in]     orc_Heading         QFileDialog heading
   \param[in]     orc_StartingFolder  QFileDialog starting folder
   \param[in]     orc_Filter          QFileDialog filter
   \param[in]     orc_DefaultFileName QFileDialog default file name
   \param[in]     oc_Option           QFileDialog option

   \return
   Get save file name (empty if aborted)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeWiUtil::h_GetSaveFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                       const QString & orc_StartingFolder, const QString & orc_Filter,
                                       const QString & orc_DefaultFileName, const QFileDialog::Options oc_Option)
{
   QString c_Retval;
   bool q_Stop = false;
   QFileDialog c_FileDialog(opc_Parent, orc_Heading, orc_StartingFolder, orc_Filter);

   c_FileDialog.setFileMode(QFileDialog::AnyFile);
   c_FileDialog.setAcceptMode(QFileDialog::AcceptSave);
   c_FileDialog.setOptions(oc_Option);
   c_FileDialog.selectFile(orc_DefaultFileName);
   while (q_Stop == false)
   {
      const sintn sn_UserChoice = c_FileDialog.exec();
      if (sn_UserChoice == static_cast<sintn>(QDialog::Accepted))
      {
         // take file name (we save only one file therefore take first entry)
         const QString c_FullFilePath = c_FileDialog.selectedFiles().at(0);

         if (c_FullFilePath != "")
         {
            const QFileInfo c_Info(c_FullFilePath);
            if (C_OSCUtils::h_CheckValidCName(c_Info.baseName().toStdString().c_str(),
                                              std::numeric_limits<uint16>::max()) == true)
            {
               c_Retval = c_FullFilePath;
               q_Stop = true;
            }
            else
            {
               C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::eERROR);
               c_MessageBox.SetHeading(orc_Heading);
               c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText(
                                                      "File name invalid. Only alphanumeric characters + \"_\" are allowed.")));
               c_MessageBox.Execute();
            }
         }
         else
         {
            //Unexpected
            tgl_assert(false);
         }
      }
      else
      {
         q_Stop = true;
      }
   }
   return c_Retval;
   //lint -e{1746} Necessary because needs default parameter and is not recognized as const
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiUtil::C_OgeWiUtil(void)
{
}

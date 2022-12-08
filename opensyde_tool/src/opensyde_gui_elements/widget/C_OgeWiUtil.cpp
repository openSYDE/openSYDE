//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for QWidget (implementation)

   Assortment of utility functions for QWidget

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"

#include <limits>

#include <QPainter>
#include <QFileInfo>
#include <QApplication>
#include <QStyleOption>
#include <QDesktopWidget>

#include "C_OscUtils.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Draw background for widget (once)

   \param[in,out]  opc_Widget    Widget to draw background for
   \param[in,out]  opc_Painter   Optional painter
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

   \param[in,out]  opc_Widget    Widget where to set property
   \param[in,out]  opcn_Name     Property name
   \param[in,out]  orc_Value     Property value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_ApplyStylesheetProperty(QWidget * const opc_Widget, const char_t * const opcn_Name,
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

   \param[in,out]  opc_Widget    Widget where to set property
   \param[in,out]  opcn_Name     Property name
   \param[in,out]  orc_Value     Property value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(QWidget * const opc_Widget,
                                                                  const char_t * const opcn_Name,
                                                                  const QVariant & orc_Value)
{
   h_ApplyStylesheetProperty(opc_Widget, opcn_Name, orc_Value);
   //Children
   for (int32_t s32_ItChild = 0; s32_ItChild < opc_Widget->children().size(); ++s32_ItChild)
   {
      QWidget * const pc_Wid = dynamic_cast<QWidget * const>(opc_Widget->children()[s32_ItChild]);
      if (pc_Wid != NULL)
      {
         h_ApplyStylesheetPropertyToItselfAndAllChildren(pc_Wid, opcn_Name, orc_Value);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set window icon

   \param[in,out]  opc_Widget    Widget to set icon for
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

   \param[in,out]  orc_GlobalPosition           Global position of top left corner (Set to default if necessary)
   \param[in,out]  orc_Size                     Size of widget (Set to default if necessary)
   \param[in]      orc_DefaultSize              Default size of widget
   \param[in]      oq_AddLogEntryForWindowSize  Optional flag to add log entry for window size
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
                         static_cast<QString>("Setup main window for screen width %1, height %2").arg(
                            pc_Desktop->geometry().width()).arg(pc_Desktop->
                                                                geometry()
                                                                .height()).toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update font depending on size

   \param[in]  opc_Widget                 Widget to adapt
   \param[in]  orc_Text                   Current text
   \param[in]  of32_HeightScaling         Scaling factor for widget height
   \param[in]  oq_IgnoreContentMargins    Optional flag to ignore content margins
   \param[in]  opc_ImprovedSize           Optional size to use for the widget (does ignore content margins!)

   \return
   -1   Error
   Else New point size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeWiUtil::h_UpdateFontSize(QWidget * const opc_Widget, const QString & orc_Text,
                                      const float32_t of32_HeightScaling, const bool oq_IgnoreContentMargins,
                                      const QSize * const opc_ImprovedSize)
{
   int32_t s32_Retval = -1;

   if ((orc_Text.compare("") != 0) && (opc_Widget != NULL))
   {
      QFont c_Font = opc_Widget->font();
      QSize c_EffectiveSize;
      int32_t s32_NewPointSize;
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
            const int32_t s32_MarginLeft = opc_Widget->contentsMargins().left();
            const int32_t s32_MarginTop = opc_Widget->contentsMargins().top();
            const int32_t s32_MarginRight = opc_Widget->contentsMargins().right();
            const int32_t s32_MarginBottom = opc_Widget->contentsMargins().bottom();
            c_EffectiveSize = QSize(opc_Widget->size().width() - (s32_MarginLeft + s32_MarginRight),
                                    opc_Widget->size().height() - (s32_MarginTop + s32_MarginBottom));
         }
      }
      s32_NewPointSize = C_OgeWiUtil::h_GetNextOptimalPointSize(c_Font, c_EffectiveSize, orc_Text,
                                                                of32_HeightScaling);
      s32_Retval = s32_NewPointSize;
      c_Font.setPointSize(s32_NewPointSize);
      opc_Widget->setFont(c_Font);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get optimal point size based on parameters

   \param[in]  orc_Font             Current font
   \param[in]  orc_Size             New size
   \param[in]  orc_Text             Current text
   \param[in]  of32_HeightScaling   Scaling factor for widget height

   \return
   Font point size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeWiUtil::h_GetNextOptimalPointSize(const QFont & orc_Font, const QSize & orc_Size, const QString & orc_Text,
                                               const float32_t of32_HeightScaling)
{
   int32_t s32_Retval;
   static QMap<QString, int32_t> hc_PreviousResults;
   //Find some way to uniquely identify the input parameter situation
   const QString c_CompleteInput =
      static_cast<QString>("%1,%2,%3,%4,%5").arg(orc_Font.toString()).arg(orc_Size.width()).arg(
         orc_Size.height()).arg(orc_Text).arg(static_cast<float64_t>(of32_HeightScaling));
   //Look up
   const QMap<QString, int32_t>::const_iterator c_It = hc_PreviousResults.find(c_CompleteInput);

   //Check if anything found
   if (c_It == hc_PreviousResults.end())
   {
      const float32_t f32_WidgetHeight = static_cast<float32_t>(orc_Size.height()) * of32_HeightScaling;
      const int32_t s32_WidgetHeight = static_cast<int32_t>(f32_WidgetHeight);
      const int32_t s32_Init = std::max(std::max(orc_Font.pointSize(), orc_Font.pixelSize()), 1);
      QFont c_Font(orc_Font.family(), s32_Init, orc_Font.weight(), orc_Font.italic());

      s32_Retval = s32_Init;
      for (int32_t s32_StepWidth = 2; s32_StepWidth > 0; --s32_StepWidth)
      {
         //Check size increase (iterative)
         for (; s32_Retval < 2000; s32_Retval += s32_StepWidth)
         {
            c_Font.setPointSize(s32_Retval);
            {
               const QFontMetrics c_Metric(c_Font);
               if ((c_Metric.horizontalAdvance(orc_Text) > orc_Size.width()) || (c_Metric.height() > s32_WidgetHeight))
               {
                  //Assuming previous one was ok
                  //lint -e{850} No problem because of break
                  s32_Retval -= s32_StepWidth;
                  break;
               }
            }
         }
         //Check if size increase did at least one iteration
         //(We don't need to iterate in the other direction if increase already worked)
         if (s32_Retval < s32_Init)
         {
            //Undo last step
            s32_Retval += s32_StepWidth;
            //Check size decrease (iterative)
            for (; s32_Retval > s32_StepWidth; s32_Retval -= s32_StepWidth)
            {
               c_Font.setPointSize(s32_Retval);
               {
                  const QFontMetrics c_Metric(c_Font);
                  if ((c_Metric.horizontalAdvance(orc_Text) <= orc_Size.width()) &&
                      (c_Metric.height() <= s32_WidgetHeight))
                  {
                     break;
                  }
               }
            }
         }
      }
      //Store new value for further calls
      hc_PreviousResults.insert(c_CompleteInput, s32_Retval);
   }
   else
   {
      s32_Retval = c_It.value();
   }

   return std::max(s32_Retval, 1);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Logic to get the next widget to gray out

   \param[in]  opc_Input   Widget to search the parent hierarchy for

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
            const stw::opensyde_gui_elements::C_OgePopUpDialog * const pc_PopUp =
               dynamic_cast<const stw::opensyde_gui_elements::C_OgePopUpDialog * const>(pc_Parent);
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

   \param[in]  opc_Event   Event information

   \return
   True  Global key handling requested
   False No special global key handling required
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeWiUtil::h_CheckGlobalKey(const QKeyEvent * const opc_Event)
{
   bool q_Retval;

   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_S)) &&
       (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      q_Retval = true;
   }
   else if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_F)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      q_Retval = true;
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F12))
   {
      q_Retval = true;
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F8))
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
/*! \brief   Get open file name via QFileDialog

   Do not use QFileDialog::getOpenFileName because it does not support default suffix,
   that we want to be added automatically (e.g. if typing "project" instead of "project.syde": load project.syde)
   Furthermore we have to forbid some characters, like German umlauts. The reason for that is that our file base
   classes like the XML parser cannot handle such characters.

   \param[in,out]  opc_Parent          Parent widget
   \param[in]      orc_Heading         QFileDialog heading
   \param[in]      orc_StartingFolder  QFileDialog starting folder
   \param[in]      orc_Filter          QFileDialog filter
   \param[in]      orc_DefaultSuffix   QFile Dialog default suffix
   \param[in]      orc_Option          QFileDialog option

   \return
   File name for opening (empty if aborted)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeWiUtil::h_GetOpenFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                       const QString & orc_StartingFolder, const QString & orc_Filter,
                                       const QString & orc_DefaultSuffix, const QFileDialog::Options & orc_Option)
{
   return mh_GetFileName(opc_Parent, orc_Heading, orc_StartingFolder, orc_Filter, "", orc_DefaultSuffix,
                         QFileDialog::AcceptOpen, orc_Option);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get save file name via QFileDialog

   \param[in,out]  opc_Parent             Parent widget
   \param[in]      orc_Heading            QFileDialog heading
   \param[in]      orc_StartingFolder     QFileDialog starting folder
   \param[in]      orc_Filter             QFileDialog filter
   \param[in]      orc_DefaultFileName    QFileDialog default file name
   \param[in]      orc_Option             QFileDialog option

   \return
   file name for saving (empty if aborted)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeWiUtil::h_GetSaveFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                       const QString & orc_StartingFolder, const QString & orc_Filter,
                                       const QString & orc_DefaultFileName, const QFileDialog::Options & orc_Option)
{
   return mh_GetFileName(opc_Parent, orc_Heading, orc_StartingFolder, orc_Filter, orc_DefaultFileName, "",
                         QFileDialog::AcceptSave, orc_Option);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show error popup for information about invalid paths.

   \param[in]  opc_Parent        parent widget (for parent of message box)
   \param[in]  orc_InvalidPaths  Paths that contain invalid characters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiUtil::h_ShowPathInvalidError(QWidget * const opc_Parent, const QString & orc_InvalidPaths)
{
   C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eERROR);

   c_Message.SetHeading(C_GtGetText::h_GetText("Invalid Path"));
   c_Message.SetDescription(C_GtGetText::h_GetText("Path contains invalid characters."));
   c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("Path(s):\n%1")).arg(orc_InvalidPaths));
   c_Message.SetCustomMinHeight(180, 250);
   c_Message.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the given file is existing.

            If file exists a message is shown "Overwrite existing file". When confirmed old file gets deleted.


   \param[in]       opc_Parent     Parent widget
   \param[out]      orc_FilePath   File path to check

  \return
   C_NO_ERR    user wants to overwrite
   C_NOACT     user does not want to overwrite
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeWiUtil::h_AskOverwriteFile(QWidget * const opc_Parent, const QString & orc_FilePath)
{
   int32_t s32_Return = C_NO_ERR;

   const QFile c_File(orc_FilePath);

   if (c_File.exists())
   {
      C_OgeWiCustomMessage c_MessageBox(opc_Parent, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Save File"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("File does already exist. Do you want to overwrite?"));
      c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Overwrite"));
      c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Back"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();

      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
      {
         s32_Return = C_NO_ERR;
      }
      else
      {
         // user does not want to overwrite
         s32_Return = C_NOACT;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get save file name via QFileDialog

   \param[in,out]  opc_Parent             Parent widget
   \param[in]      orc_Heading            QFileDialog heading
   \param[in]      orc_StartingFolder     QFileDialog starting folder
   \param[in]      orc_Filter             QFileDialog filter
   \param[in]      orc_DefaultFileName    QFileDialog default file name
   \param[in]      orc_DefaultSuffix      Default suffix
   \param[in]      ore_SaveOrOpen         Save or open
   \param[in]      orc_Option             QFileDialog option

   \return
   Get save file name (empty if aborted)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeWiUtil::mh_GetFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                    const QString & orc_StartingFolder, const QString & orc_Filter,
                                    const QString & orc_DefaultFileName, const QString & orc_DefaultSuffix,
                                    const QFileDialog::AcceptMode & ore_SaveOrOpen,
                                    const QFileDialog::Options & orc_Option)
{
   QString c_Retval = "";
   bool q_Stop = false;
   QFileDialog c_FileDialog(opc_Parent, orc_Heading, orc_StartingFolder, orc_Filter);

   c_FileDialog.setFileMode(QFileDialog::AnyFile);
   c_FileDialog.setAcceptMode(ore_SaveOrOpen);
   c_FileDialog.setOptions(orc_Option);
   c_FileDialog.setDefaultSuffix(orc_DefaultSuffix);
   c_FileDialog.selectFile(orc_DefaultFileName);
   while (q_Stop == false)
   {
      const int32_t s32_UserChoice = c_FileDialog.exec();
      if (s32_UserChoice == static_cast<int32_t>(QDialog::Accepted))
      {
         // take file name (we save only one file therefore take first entry)
         const QString c_FullFilePath = c_FileDialog.selectedFiles().at(0);

         if (c_FullFilePath != "")
         {
            // check if file name contains invalid characters
            if (C_OscUtils::h_CheckValidFilePath(c_FullFilePath.toStdString().c_str()) == true)
            {
               c_Retval = c_FullFilePath;
               q_Stop = true;
            }
            else
            {
               C_OgeWiUtil::h_ShowPathInvalidError(opc_Parent, c_FullFilePath);
               // update file path (open dialog on last "failed" location, not on startup location)
               c_FileDialog.setDirectory(static_cast<QFileInfo>(c_FullFilePath).dir());
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

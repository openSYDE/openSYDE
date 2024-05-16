//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SyvDaDashboardSelectorTabWidget.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeGbxTransparent.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_SyvClipBoardHelper.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvDaDashboardTabProperties.hpp"
#include "C_SyvDaCopyPasteManager.hpp"
#include "C_OscUtils.hpp"
#include "C_SclString.hpp"
#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_SyvDaDashboardScreenshot.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_LIGHT =
   "://images/system_views/IconScreenshot.svg";
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_IN_ACTION_ICON_LIGHT =
   "://images/system_views/IconScreenshotInAction.svg";
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_DARK =
   "://images/system_views/IconScreenshotDark.svg";
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_IN_ACTION_ICON_DARK =
   "://images/system_views/IconScreenshotInActionDark.svg";
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_WATERMARK_ICON =
   "://images/openSYDE_logo.ico";
const QString C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_SUCCESS_GIF =
   "../../src/images/ScreenshotCustom.gif";

const int32_t C_SyvDaDashboardScreenshot::mhs32_SCREENSHOT_GIF_LABEL_WIDTH = 200;
const int32_t C_SyvDaDashboardScreenshot::mhs32_SCREENSHOT_GIF_LABEL_HEIGHT = 200;
const int32_t C_SyvDaDashboardScreenshot::mhs32_SCREENSHOT_ICON_CHANGE_TIMEOUT = 250;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */
QString C_SyvDaDashboardScreenshot::mhc_ScreenshotFolderPath;

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]       opc_Parent                   Parent widget
   \param[in]       opc_ScreenshotPushButton     pointer to parent class screenshot button
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardScreenshot::C_SyvDaDashboardScreenshot(QWidget * const opc_Parent,
                                                       stw::opensyde_gui_elements::C_OgePubIconEvents * const opc_ScreenshotPushButton)
   :
   QWidget(opc_Parent),
   mpc_ScreenshotPushButton(opc_ScreenshotPushButton),
   mq_IsDarkModeActive(false),
   mq_IsMessageBoxToBeShown(false)
{
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   //Add screenshot button
   this->mpc_ScreenshotPushButton->SetIconSize(24);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_LIGHT));
   this->mpc_ScreenshotPushButton->SetToolTipInformation(C_GtGetText::h_GetText("Screenshot (F10)"),
                                                         C_GtGetText::h_GetText("Take dashboard screenshot. "
                                                                                "\nImage is copied to clipboard and"
                                                                                " saved at /Screenshots folder."));

   this->mpc_ScreenshotPushButton->setEnabled(!q_ServiceModeActive);

   //lint -e{1938}  static const is guaranteed preinitialized before main
   const QFileInfo c_FileInfo(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_SUCCESS_GIF);

   if (c_FileInfo.exists() == true)
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Movie = new QMovie(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_SUCCESS_GIF);
      this->mpc_GifLabel = new QLabel(opc_Parent);
      const QSize c_SIZE(mhs32_SCREENSHOT_GIF_LABEL_WIDTH, mhs32_SCREENSHOT_GIF_LABEL_HEIGHT);
      this->mpc_GifLabel->setFixedSize(c_SIZE);

      mc_GifTimer.setSingleShot(true);
      connect(&mc_GifTimer, &QTimer::timeout, this, &C_SyvDaDashboardScreenshot::m_GifTimeout);
      this->mpc_GifLabel->setVisible(false);
   }
   else
   {
      this->mpc_Movie = NULL;
      this->mpc_GifLabel = NULL;
   }

   mc_ScreenshotIconChangeTimer.setSingleShot(true);
   connect(&mc_ScreenshotIconChangeTimer, &QTimer::timeout, this,
           &C_SyvDaDashboardScreenshot::m_ChangeToDefaultScreenshotIcon);

   ms32_ScreenshotSuccessGifTimeout = C_UsHandler::h_GetInstance()->GetScreenshotGifSucessTimeout();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------

C_SyvDaDashboardScreenshot::~C_SyvDaDashboardScreenshot()
{
   mpc_ScreenshotPushButton = NULL;
   this->mpc_GifLabel = NULL;
   delete (this->mpc_Movie);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gives string value for saving screenshot with name of format yyyy_MM_dd__HH_mm_ss_zzz_ProjectName_
 *  DashboardName

   \param[in]       orc_DashboardName     Current dashboard name

   \return
   return c_StrDateTime, the name of the screenshot to be saved as
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaDashboardScreenshot::mh_SaveScreenshotAs(const C_SclString & orc_DashboardName)
{
   const QDateTime c_DateTime = QDateTime::currentDateTime();
   const QString c_StrDateTime = c_DateTime.toString("yyyy_MM_dd__HH_mm_ss_zzz") + "__" +
                                 C_PuiProject::h_GetInstance()->GetName() + "__" + orc_DashboardName.c_str();

   return c_StrDateTime;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Shoot screenshot on pressing screenshot button or on Shortcut F10
 *  create folder "Screenshot" at destination if it is not exists

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_ShootScreenshot()
{
   //Initialize path. Do *not* do this const. Otherwise we'd get the currentPath before it might
   // be changed by the startup logic.
   C_SyvDaDashboardScreenshot::mhc_ScreenshotFolderPath = QDir::currentPath() + "/Screenshots/";
   const QDir c_Dir(mhc_ScreenshotFolderPath);

   C_SyvDaDashboardScreenshot::m_ChangeToActionScreenshotIcon();
   QPixmap oc_CurrentTabPixmap(this->parentWidget()->size());
   this->parentWidget()->render(&oc_CurrentTabPixmap);

   if (!c_Dir.exists())
   {
      c_Dir.mkpath(".");
      mq_IsMessageBoxToBeShown = true;
   }

   mc_ScreenshotIconChangeTimer.start(mhs32_SCREENSHOT_ICON_CHANGE_TIMEOUT);
   C_SyvDaDashboardScreenshot::m_SaveScreenshot(oc_CurrentTabPixmap, this->accessibleName().toStdString().c_str());

   if ((this->mpc_GifLabel != NULL) && (this->mpc_Movie != NULL))
   {
      mc_GifTimer.start(ms32_ScreenshotSuccessGifTimeout);
   }
   if (mq_IsMessageBoxToBeShown)
   {
      C_SyvDaDashboardScreenshot::m_ShowMessageBox(this->parentWidget());
      this->mq_IsMessageBoxToBeShown = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On mc_GifTimer timeout,stop mpc_Movie and close the mpc_GifLabel

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_GifTimeout()
{
   if ((this->mpc_GifLabel != NULL) && (this->mpc_Movie != NULL))
   {
      this->mpc_Movie->stop();
      this->mpc_GifLabel->setVisible(false);
      this->mpc_GifLabel->close();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On mc_ScreenshotIconChangeTimer timeout, change screenshot Icon to default
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_ChangeToDefaultScreenshotIcon()
{
   if (!mq_IsDarkModeActive)
   {
      this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_LIGHT));
   }
   else
   {
      this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_DARK));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On ShootScreenshot change ScreenshotIcon from default to Action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_ChangeToActionScreenshotIcon()
{
   if ((this->mpc_GifLabel != NULL) && (this->mpc_Movie != NULL))
   {
      if (this->mpc_Movie->state() == QMovie::Running)
      {
         this->mpc_Movie->stop();
         this->mpc_GifLabel->setVisible(false);
         this->mpc_GifLabel->close();
      }
   }

   if (!mq_IsDarkModeActive)
   {
      this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::
                                                    mhc_SCREENSHOT_IN_ACTION_ICON_LIGHT));
   }
   else
   {
      this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::
                                                    mhc_SCREENSHOT_IN_ACTION_ICON_DARK));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  After rendering screenshot save screenshot at destination
 *  create watermark icon on right bottom of screenshot
 *  Copy screenshot to clipboard
 *  After saving screenshot play the GIF if available

   \param[in]       oc_CurrentTabPixmap     Taken screenshot pixmap
   \param[in]      orc_DashboardName   Current dashboard name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_SaveScreenshot(QPixmap oc_CurrentTabPixmap, const C_SclString & orc_DashboardName)
{
   QClipboard * const pc_Clipboard = QApplication::clipboard();
   const QString c_ScreenshotFormat = "png";

   //Set WaterMarkIcon for the screenshot on right bottom corner
   const QPixmap c_WatermarkPixmap(mhc_SCREENSHOT_WATERMARK_ICON);
   QPainter c_WatermarkPainter(&oc_CurrentTabPixmap);
   const int32_t s32_Xpos = (oc_CurrentTabPixmap.width() - c_WatermarkPixmap.width()) - 20;
   const int32_t s32_Ypos = (oc_CurrentTabPixmap.height() - c_WatermarkPixmap.height()) - 20;

   if ((this->mpc_GifLabel != NULL) && (this->mpc_Movie != NULL))
   {
      this->mpc_GifLabel->setParent(this->parentWidget());
      this->mpc_GifLabel->setVisible(true);
   }

   c_WatermarkPainter.setOpacity(0.5);
   c_WatermarkPainter.drawPixmap(s32_Xpos, s32_Ypos, c_WatermarkPixmap);

   //Copy screenshot to clipboard
   pc_Clipboard->setPixmap(oc_CurrentTabPixmap);

   const QString c_ScreenshotSavedPath = mhc_ScreenshotFolderPath + C_SyvDaDashboardScreenshot::mh_SaveScreenshotAs(
      orc_DashboardName);

   const QString c_ScreenshotFileName = c_ScreenshotSavedPath + "." + c_ScreenshotFormat;

   if (!c_ScreenshotFileName.isEmpty())
   {
      tgl_assert(oc_CurrentTabPixmap.save(c_ScreenshotFileName, c_ScreenshotFormat.toLatin1().constData()));
   }

   if ((this->mpc_GifLabel != NULL) && (this->mpc_Movie != NULL))
   {
      if (this->mpc_Movie->state() == QMovie::NotRunning)
      {
         this->mpc_GifLabel->show();
         const QSize c_SIZE(200, 200);
         this->mpc_Movie->setScaledSize(c_SIZE);
         this->mpc_GifLabel->move((this->parentWidget()->width() - mhs32_SCREENSHOT_GIF_LABEL_WIDTH) - 10,
                                  (this->parentWidget()->height() - mhs32_SCREENSHOT_GIF_LABEL_HEIGHT) - 10);
         this->mpc_GifLabel->setMovie(this->mpc_Movie);
         this->mpc_Movie->start();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show information message box with local path link where screenshots are saved

   \param[in]       opc_ParentWidget     Current widget parent widget

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_ShowMessageBox(QWidget * const opc_ParentWidget)
{
   C_OgeWiCustomMessage c_MessageBox(opc_ParentWidget, C_OgeWiCustomMessage::E_Type::eINFORMATION);

   const QString c_ScreenshotLocationPath = C_Uti::h_GetLink(mhc_ScreenshotFolderPath,
                                                             mc_STYLE_GUIDE_COLOR_4,
                                                             mhc_ScreenshotFolderPath);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard Screenshot"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                  "Dashboard screenshot taken. Image is saved at /Screenshots folder."));
   c_MessageBox.SetDetails(c_ScreenshotLocationPath);
   c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("OK"));
   c_MessageBox.SetCustomMinHeight(180, 250);
   c_MessageBox.Execute();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open screenshot save location
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::m_OpenScreenshotLocation() const
{
   const QString c_FolderPath = QDir::currentPath() + "/Screenshots/";
   const QUrl c_FolderUrl = QUrl::fromLocalFile(c_FolderPath);

   QDesktopServices::openUrl(c_FolderUrl);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reposition of screenshot icon on window/application size changes

   \param[in]       os32_Xpos     Screenshot icon Xpos to be placed
   \param[in]      os32_Ypos   Screenshot icon Ypos to be placed

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::RepositionScreenshotIcon(const int32_t os32_Xpos, const int32_t os32_Ypos)
{
   if (this->mpc_ScreenshotPushButton != NULL)
   {
      this->mpc_ScreenshotPushButton->move(os32_Xpos, os32_Ypos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set screenshot Icon to Dark/Light mode based on selection

   \param[in]       oq_IsDarkModeActive     bool to check for darkmode/lightmode

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::SetDarkModeActive(const bool oq_IsDarkModeActive)
{
   mq_IsDarkModeActive = oq_IsDarkModeActive;

   if (this->mpc_ScreenshotPushButton != NULL)
   {
      if (oq_IsDarkModeActive == true)
      {
         this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_DARK));
      }
      else
      {
         this->mpc_ScreenshotPushButton->setIcon(QIcon(C_SyvDaDashboardScreenshot::mhc_SCREENSHOT_ICON_LIGHT));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On Ctrl + F10/Screenshot button combination press, shoot screenshot and open screenshot save location in file explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::PerformScreenshotWithCtrlPress()
{
   C_SyvDaDashboardScreenshot::m_ShootScreenshot();
   C_SyvDaDashboardScreenshot::m_OpenScreenshotLocation();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On Screenshot button press shoot screenshot
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScreenshot::PerformScreenshotWithoutCtrlPress()
{
   C_SyvDaDashboardScreenshot::m_ShootScreenshot();
}

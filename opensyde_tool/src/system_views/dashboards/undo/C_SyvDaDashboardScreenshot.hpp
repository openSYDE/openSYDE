//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSCREENSHOT_HPP
#define C_SYVDADASHBOARDSCREENSHOT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QMovie>
#include <QLabel>
#include <QTimer>
#include <QClipboard>
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QUrl>
#include <QDesktopServices>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OgePubIconText.hpp"
#include "C_OgePubIconEvents.hpp"
#include "C_UsHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardScreenshot :
   public QWidget
{
   Q_OBJECT

public:
   C_SyvDaDashboardScreenshot(QWidget * const opc_Parent = NULL,
                              stw::opensyde_gui_elements::C_OgePubIconEvents * const opc_ScreenshotPushButton = NULL);
   ~C_SyvDaDashboardScreenshot(void) override;

   void RepositionScreenshotIcon(const int32_t os32_Xpos, const int32_t os32_Ypos);
   void SetDarkModeActive(const bool oq_IsDarkModeActive);
   void PerformScreenshotWithCtrlPress(void);
   void PerformScreenshotWithoutCtrlPress(void);

protected:
private:
   static const QString mhc_SCREENSHOT_FOLDER_PATH;
   static const int32_t mhs32_SCREENSHOT_GIF_LABEL_WIDTH;
   static const int32_t mhs32_SCREENSHOT_GIF_LABEL_HEIGHT;
   static const QString mhc_SCREENSHOT_ICON_LIGHT;
   static const QString mhc_SCREENSHOT_IN_ACTION_ICON_LIGHT;
   static const QString mhc_SCREENSHOT_ICON_DARK;
   static const QString mhc_SCREENSHOT_IN_ACTION_ICON_DARK;
   static const QString mhc_SCREENSHOT_WATERMARK_ICON;
   static const QString mhc_SCREENSHOT_SUCCESS_GIF;
   static const int32_t mhs32_SCREENSHOT_ICON_CHANGE_TIMEOUT;

   stw::opensyde_gui_elements::C_OgePubIconEvents * mpc_ScreenshotPushButton;
   QTimer mc_GifTimer;
   QTimer mc_ScreenshotIconChangeTimer;

   void m_ShootScreenshot(void);
   static QString mh_SaveScreenshotAs(const stw::scl::C_SclString & orc_DashboardName);
   void m_GifTimeout(void);
   void m_ChangeToDefaultScreenshotIcon(void);
   void m_ChangeToActionScreenshotIcon(void);
   void m_SaveScreenshot(QPixmap oc_CurrentTabPixmap, const stw::scl::C_SclString & orc_DashboardName);
   void m_ShowMessageBox(QWidget * const opc_ParentWidget);
   void m_OpenScreenshotLocation(void) const;

   bool mq_IsDarkModeActive;
   bool mq_IsMessageBoxToBeShown;
   int32_t ms32_ScreenshotSuccessGifTimeout;

   QMovie * mpc_Movie;
   QLabel * mpc_GifLabel;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

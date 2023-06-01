//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling system view

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SYVHANDLERWIDGET_H
#define C_SYVHANDLERWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointer>

#include "C_NagUseCaseWidget.hpp"
#include "C_SyvSeSetupWidget.hpp"
#include "C_SyvUpUpdateWidget.hpp"
#include "C_SyvDaDashboardsWidget.hpp"
#include "C_SyvDaDashboardInteraction.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvHandlerWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvHandlerWidget :
   public C_NagUseCaseWidget
{
   Q_OBJECT

public:
   explicit C_SyvHandlerWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvHandlerWidget(void) override;

   void UserInputFunc(const uint32_t ou32_FuncNumber) override;
   void SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag) override;
   void Save(void) override;
   bool PrepareToClose(void) override;
   bool GlobalUserKeyPress(QKeyEvent * const opc_Event) override;
   //Generic push button
   void OnPushButtonIconPress(void) override;
   void CallHelp(void) override;

private:
   //Avoid call
   C_SyvHandlerWidget(const C_SyvHandlerWidget &);
   C_SyvHandlerWidget & operator =(const C_SyvHandlerWidget &) &; //lint !e1511 //we want to hide the base func.

   void m_DataChanged(void);
   void m_DashboardCountChanged(void);
   void m_ErrorChanged(const uint32_t ou32_Index);
   void m_EnableConfiguration(const bool oq_State);
   void m_SetPushButtonDarkIconSvg(const QString & orc_Path);
   void m_SetConnectPushButtonIcon(const QString & orc_IconPath, const bool oq_DisplayAnimation);
   void m_OnPushButtonConnectPress(void);
   void m_SetConfigurationAvailable(const bool oq_State);

   Ui::C_SyvHandlerWidget * mpc_Ui;

   C_SyvSeSetupWidget * mpc_SetupWidget;
   C_SyvUpUpdateWidget * mpc_UpdateWidget;
   C_SyvDaDashboardsWidget * mpc_DashboardsWidget;

   QPointer<C_SyvDaDashboardInteraction> mc_Interaction;

   int32_t ms32_SubMode;
   uint32_t mu32_Index;
   bool mq_DataChanged;

   static const uint32_t mhu32_USER_INPUT_FUNC_APPLY;
   static const uint32_t mhu32_USER_INPUT_FUNC_SETTINGS;
   static const uint32_t mhu32_USER_INPUT_FUNC_DEVICECONFIG;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

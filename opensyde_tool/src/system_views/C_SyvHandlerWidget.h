//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling system view

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_SYVHANDLERWIDGET_H
#define C_SYVHANDLERWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include "C_NagUseCaseWidget.h"
#include "C_SyvSeSetupWidget.h"
#include "C_SyvUpUpdateWidget.h"
#include "C_SyvDaDashboardsWidget.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvHandlerWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvHandlerWidget :
   public C_NagUseCaseWidget
{
   Q_OBJECT

public:
   explicit C_SyvHandlerWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvHandlerWidget();

   virtual void UserInputFunc(const stw_types::uint32 ou32_FuncNumber) override;
   virtual void SetSubMode(const stw_types::sint32 os32_SubMode, const stw_types::uint32 ou32_Index,
                           const stw_types::uint32 ou32_Flag) override;
   virtual void Save(void);
   virtual bool PrepareToClose(void);
   virtual bool GlobalUserKeyPress(QKeyEvent * const opc_Event) override;
   //Generic push button
   virtual void OnPushButtonIconPress(void);
   virtual void CallHelp(void);

private:
   //Avoid call
   C_SyvHandlerWidget(const C_SyvHandlerWidget &);
   C_SyvHandlerWidget & operator =(const C_SyvHandlerWidget &);

   void m_ActivateEditButton(void);
   void m_DataChanged(void);
   void m_ErrorChanged(const stw_types::uint32 ou32_Index);
   void m_EnableConfiguration(const bool oq_State);
   void m_SetConfigError(const bool oq_Active);

   Ui::C_SyvHandlerWidget * mpc_Ui;

   C_SyvSeSetupWidget * mpc_SetupWidget;
   C_SyvUpUpdateWidget * mpc_UpdateWidget;
   C_SyvDaDashboardsWidget * mpc_DashboardsWidget;

   stw_types::sint32 ms32_SubMode;
   stw_types::uint32 mu32_Index;
   bool mq_DataChanged;

   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_APPLY;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_EDIT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_SETTINGS;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_CONNECT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_DISCONNECT;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_CONNECTING;
   static const stw_types::uint32 mhu32_USER_INPUT_FUNC_DEVICECONFIG;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

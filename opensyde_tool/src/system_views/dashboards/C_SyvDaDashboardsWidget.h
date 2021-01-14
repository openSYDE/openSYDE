//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSWIDGET_H
#define C_SYVDADASHBOARDSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "stwtypes.h"
#include "C_OgeWiHover.h"
#include "C_SyvComDriverDiag.h"
#include "C_SyvDaDashboardToolbox.h"
#include "C_SyvComDriverDiagConnect.h"
#include "C_OgeWiFixPosition.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaDashboardsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardsWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_ToolboxParent,
                                    QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardsWidget(void);

   void InitText() const;
   void LoadDarkMode(void);
   void Save(void) const;
   void OpenSettings(void);
   void ToggleDarkMode(void);

   void SetEditMode(const bool oq_Active);
   bool GetEditMode(void) const;

   bool GetConnectActive(void) const;
   void SetConnectActive(const bool oq_Value);

   bool PrepareToClose(void) const;
   void CheckError(void) const;
   void OnPushButtonConnectPress(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNumberDashboardsChanged(void);
   void SigSetConfigurationAvailable(const bool oq_State);
   void SigSetDarkModePushButtonIcon(const QString & orc_IconPath);
   void SigSetConnectPushButtonIcon(const QString & orc_IconPath, const bool oq_DisplayAnimation);
   void SigBlockDragAndDrop(const bool oq_Block);

protected:
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardsWidget(const C_SyvDaDashboardsWidget &);
   C_SyvDaDashboardsWidget & operator =(const C_SyvDaDashboardsWidget &);

   void m_ConfirmClicked(void);
   void m_CancelClicked(void);
   void m_ApplyDarkMode(const bool oq_Active);
   void m_InitToolBox(void);
   void m_CleanUpToolBox(void) const;
   void m_UpdateShowValues(void) const;

   stw_types::sint32 m_InitOsyDriver(QString & orc_Message);
   void m_CloseOsyDriver(void);

   void m_DataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                        const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void m_DataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);
   void m_NvmReadList(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Index);
   void m_HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);
   void m_ConnectStepFinished(void);
   void m_HandleConnectionResult(const stw_types::sint32 os32_Result, const QString & orc_Message,
                                 const QString & orc_MessageDetails);

   void m_WiFixPosMaxBtnClicked(void);
   void m_WiHoverMinBtnClicked(void);
   void m_AdaptSpaceHolderWidgetColor(void) const;

   enum E_ConnectState
   {
      eCS_DISCONNECTED,
      eCS_CONNECTING,
      eCS_CONNECTED,
      eCS_DISCONNECTING
   };

   Ui::C_SyvDaDashboardsWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_SyvComDriverDiag * mpc_ComDriver;
   stw_opensyde_gui_logic::C_SyvComDriverDiagConnect * const mpc_ConnectionThread;
   stw_opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;
   stw_opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedToolbox;
   QWidget * const mpc_ToolboxParent;
   C_SyvDaDashboardToolbox * mpc_ToolboxContent;
   QTimer mc_Timer;
   stw_types::uint32 mu32_ViewIndex;
   bool mq_EditModeActive;
   bool mq_DarkModeActive;
   bool mq_ConnectActive;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListId> mc_MissedReadNvmOperations;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_MissedReadOperations;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_MissedWriteOperations;
   E_ConnectState me_ConnectState;
   stw_types::sintn msn_InitToolboxCounter;
   static stw_types::uint32 mhu32_DisconnectTime;
   static const QString mhc_DarkModeEnabledIconPath;
   static const QString mhc_DarkModeDisabledIconPath;
   static const stw_types::sintn mhsn_WidgetBorder;
   static const stw_types::sintn mhsn_ToolboxInitPosY;
};
}

#endif

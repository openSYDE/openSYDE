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

#include "stwtypes.hpp"
#include "C_OgeWiHover.hpp"
#include "C_SyvComDriverDiag.hpp"
#include "C_SyvDaDashboardToolbox.hpp"
#include "C_SyvComDriverDiagConnect.hpp"
#include "C_OgeWiFixPosition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaDashboardsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardsWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_ToolboxParent,
                                    QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardsWidget(void) override;

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
   void hideEvent(QHideEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardsWidget(const C_SyvDaDashboardsWidget &);
   C_SyvDaDashboardsWidget & operator =(const C_SyvDaDashboardsWidget &) &;

   void m_ConfirmClicked(void);
   void m_CancelClicked(void);
   void m_ApplyDarkMode(const bool oq_Active);
   void m_InitToolBox(void);
   void m_CleanUpToolBox(void) const;
   void m_UpdateShowValues(void) const;

   int32_t m_InitOsyDriver(QString & orc_Message);
   void m_CloseOsyDriver(void);

   void m_DataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                        const uint16_t ou16_ElementIndex);
   void m_DataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index);
   void m_NvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Index);
   void m_HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc);
   void m_ConnectStepFinished(void);
   void m_HandleConnectionResult(const int32_t os32_Result, const QString & orc_Message,
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
   stw::opensyde_gui_logic::C_SyvComDriverDiag * mpc_ComDriver;
   stw::opensyde_gui_logic::C_SyvComDriverDiagConnect * const mpc_ConnectionThread;
   stw::opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;
   stw::opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedToolbox;
   QWidget * const mpc_ToolboxParent;
   C_SyvDaDashboardToolbox * mpc_ToolboxContent;
   QTimer mc_Timer;
   uint32_t mu32_ViewIndex;
   bool mq_EditModeActive;
   bool mq_DarkModeActive;
   bool mq_ConnectActive;
   std::set<stw::opensyde_core::C_OscNodeDataPoolListId> mc_MissedReadNvmOperations;
   std::set<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_MissedReadOperations;
   std::set<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_MissedWriteOperations;
   E_ConnectState me_ConnectState;
   int32_t ms32_InitToolboxCounter;
   static uint32_t mhu32_DisconnectTime;
   static const QString mhc_DARK_MODE_ENABLED_ICON_PATH;
   static const QString mhc_DARK_MODE_DISABLED_ICON_PATH;
   static const int32_t mhs32_WIDGET_BORDER;
   static const int32_t mhs32_TOOLBOX_INIT_POS_Y;
};
}
}

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEWIDGET_H
#define C_SYVUPUPDATEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <mutex>

#include <QWidget>
#include <QTimer>

#include "stwtypes.h"

#include "C_OgeWiHover.h"
#include "C_SyvUpScene.h"
#include "C_SyvUpSequences.h"
#include "C_SyvUpProgressLog.h"
#include "C_OgeWiFixPosition.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpUpdateWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdateWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdateWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_ProgressLogParent,
                                QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdateWidget();

   void InitText(void) const;
   void InitIcon(void) const;
   bool PrepareToClose(void);
   void LoadScene(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigBlockDragAndDrop(const bool oq_Block);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvUpUpdateWidget(const C_SyvUpUpdateWidget &);
   C_SyvUpUpdateWidget & operator =(const C_SyvUpUpdateWidget &);

   void m_CheckError(void);

   stw_types::sint32 m_InitSequence(void);
   void m_CleanUpSequence(void);

   void m_UpdatePackageState(const stw_types::sint32 os32_State);

   void m_ReportProgress(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                         const stw_types::uint8 ou8_Progress);

   void m_ReportProgressForServer(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                  const stw_types::uint8 ou8_Progress, const stw_types::uint8 ou8_BusIdentifier,
                                  const stw_types::uint8 ou8_NodeIdentifier);

   void m_ReportOpenSydeFlashloaderInformationRead(void);
   void m_ReportStwFlashloaderInformationRead(void);

   void m_CheckOpenSydeFlashloaderInformation(const std::vector<stw_types::uint32> & orc_OsyNodeIndexes,
                                              const std::vector<stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation> & orc_OsyDeviceInformation);

   void m_Connect(void);
   void m_Update(void);

   void m_Disconnect(void);
   void m_DisconnectAction(const bool oq_ClearLogAndResetScene = true);
   void m_Cancel(void);

   void m_Timer(void);

   void m_UpdateReportText(const QString & orc_NewTextPart) const;

   void m_HandleConnectionFailure(void);
   void m_HandleUpdateFailure(void);

   void m_InitToolBox(void);
   void m_CleanUpToolBox(void);
   void m_AddProgressLogConnectEntry(const stw_types::uint32 ou32_NodeIndex);

   void m_ReplaceOriginalWithTempPaths(void);
   void m_DiscardInfo(const stw_types::uint32 ou32_NodeIndex);

   std::vector<bool> m_GetIsFileBasedFlagForEach(void) const;

   static bool mh_IsConnectionStart(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsConnectionSuccess(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsConnectionFailure(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateAppStart(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step,
                                   bool & orq_IsParam);
   static bool mh_IsUpdateAppSuccess(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step,
                                     bool & orq_IsParam);
   static bool mh_IsUpdateNodeStart(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateNodeSuccess(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateFailure(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateAbort(const stw_opensyde_core::C_OSCSuSequences::E_ProgressStep oe_Step);

   void m_WiFixPosMaxBtnClicked(void);
   void m_WiHoverMinBtnClicked(void);

   Ui::C_SyvUpUpdateWidget * mpc_Ui;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_ApplicationIndex;
   C_SyvUpScene * mpc_Scene;
   stw_opensyde_gui_elements::C_OgeWiHover * mpc_ProgressLog;
   stw_opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedProgressLog;
   QWidget * const mpc_ProgressLogParent;
   C_SyvUpProgressLog * mpc_ProgressLogContent;

   QTimer mc_Timer;

   stw_opensyde_gui_logic::C_SyvUpSequences * mpc_UpSequences;
   stw_opensyde_gui_logic::C_SyvUpSequences::E_Sequence me_Step;
   bool mq_Connected;
   bool mq_ClearProgressLog;
   bool mq_StartUpdateAfterConnect;
   bool mq_ConnectFailed;

   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> mc_NodesToFlash;
   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> mc_NodesWithAllApplications;
   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> mc_NodesWithAllApplicationsAndTempPath;
   std::vector<stw_types::uint32> mc_NodesOrder;
   bool mq_NodesPreconditionError;
   std::vector<stw_types::uint32> mc_NodesPreconditionNvmWriteError;
   std::vector<stw_types::uint32> mc_NodesPreconditionEthToEthError;

   stw_types::uint32 mu32_DisconnectTime;
   stw_types::uint32 mu32_UpdateTime;

   static const QString mhc_TempFolder;
   static const stw_types::sintn mhsn_WidgetBorder;
   static const stw_types::sintn mhsn_ToolboxInitPosY;
   static const stw_types::uint32 mhu32_WaitTime;
};
}

#endif

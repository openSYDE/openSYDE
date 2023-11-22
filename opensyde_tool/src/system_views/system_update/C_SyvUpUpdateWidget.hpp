//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEWIDGET_HPP
#define C_SYVUPUPDATEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <mutex>

#include <QWidget>
#include <QTimer>

#include "stwtypes.hpp"

#include "C_OgeWiHover.hpp"
#include "C_SyvUpScene.hpp"
#include "C_GiSvNodeData.hpp"
#include "C_SyvUpSequences.hpp"
#include "C_SyvUpProgressLog.hpp"
#include "C_OgeWiFixPosition.hpp"
#include "C_OscSuSequencesNodeStates.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpUpdateWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdateWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdateWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_ProgressLogParent,
                                QWidget * const opc_Parent = NULL);
   ~C_SyvUpUpdateWidget() override;

   void InitText(void) const;
   void InitIcon(void) const;
   bool PrepareToClose(void);
   void LoadScene(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigInvalidView(void);
   void SigBlockDragAndDrop(const bool oq_Block);
   void SigNodeConnectStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SigNodeUpdateStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeUpdateStates> & orc_NodeStates);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void hideEvent(QHideEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvUpUpdateWidget(const C_SyvUpUpdateWidget &);
   C_SyvUpUpdateWidget & operator =(const C_SyvUpUpdateWidget &) &;

   void m_CheckError(void);

   int32_t m_InitSequence(void);
   void m_CleanUpSequence(void);

   void m_UpdatePackageState(const int32_t os32_State);

   void m_ReportProgress(const uint32_t ou32_Step, const int32_t os32_Result, const uint8_t ou8_Progress);

   void m_ReportProgressForServer(const uint32_t ou32_Step, const int32_t os32_Result, const uint8_t ou8_Progress,
                                  const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier);

   void m_ReportOpenSydeFlashloaderInformationRead(void);
   void m_ReportStwFlashloaderInformationRead(void);

   void m_CheckOpenSydeFlashloaderInformation(const std::vector<uint32_t> & orc_OsyNodeIndexes,
                                              const std::vector<stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation> & orc_OsyDeviceInformation);

   void m_Connect(void);
   void m_Update(void);

   void m_Disconnect(void);
   void m_DisconnectAction(void);
   void m_Cancel(void);

   void m_Timer(void);

   void m_HandlePreconditionErrorType(const std::vector<uint32_t> & orc_ErrorNodeIndexes,
                                      const QString & orc_Description, const QString & orc_DetailsStart);
   void m_HandleNodePreconditionError(QString & orc_ErrorText, const uint32_t ou32_ErrorNodeIndex);
   void m_UpdateReportText(const QString & orc_NewTextPart) const;

   void m_HandleConnectionFailure(const bool oq_SuppressMessageBox = false);
   void m_HandleUpdateFailure(void);

   void m_InitProgressLog(void);
   void m_CleanUpProgressLog(void);
   void m_AddProgressLogConnectEntry(const uint32_t ou32_NodeIndex);

   void m_ReplaceOriginalWithTempPaths(void);
   void m_DiscardInfo(const uint32_t ou32_NodeIndex);

   std::vector<bool> m_GetIsFileBasedFlagForEach(void) const;

   static bool mh_IsConnectionStart(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsConnectionSuccess(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsConnectionFailure(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateAppStart(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step,
                                   bool & orq_IsParam, bool & orq_IsPemFile);
   static bool mh_IsUpdateAppSuccess(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step,
                                     bool & orq_IsParam, bool & orq_IsPemFile);
   static bool mh_IsUpdateNodeStart(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateNodeSuccess(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateFailure(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsUpdateAbort(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step);
   static bool mh_IsSecurityWarningNecessary(
      const std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_NodesToFlash);

   void m_UpdateUpdatePackageStatus(void);
   void m_WiFixPosMaxBtnClicked(void);
   void m_WiHoverMinBtnClicked(void);

   Ui::C_SyvUpUpdateWidget * mpc_Ui;
   uint32_t mu32_ViewIndex;
   uint32_t mu32_ApplicationIndex;
   C_SyvUpScene * mpc_Scene;
   stw::opensyde_gui_elements::C_OgeWiHover * mpc_ProgressLog;
   stw::opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedProgressLog;
   QWidget * const mpc_ProgressLogParent;
   C_SyvUpProgressLog * mpc_ProgressLogContent;

   QTimer mc_Timer;

   stw::opensyde_gui_logic::C_SyvUpSequences * mpc_UpSequences;
   stw::opensyde_gui_logic::C_SyvUpSequences::E_Sequence me_Step;
   bool mq_Connected;
   bool mq_ClearProgressLog;
   bool mq_StartUpdateAfterConnect;
   bool mq_ConnectFailed;
   bool mq_ErrorDetected;

   std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> mc_NodesToFlash;
   std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> mc_NodesWithAllApplications;
   std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> mc_NodesWithAllApplicationsAndTempPath;
   std::vector<uint32_t> mc_NodesOrder;
   bool mq_NodesPreconditionError;
   C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors mc_NodePreconditionErrors;

   uint32_t mu32_DisconnectTime;
   uint32_t mu32_UpdateTime;

   static const QString mhc_TEMP_FOLDER;
   static const int32_t mhs32_WIDGET_BORDER;
   static const int32_t mhs32_TOOLBOX_INIT_POS_Y;
   static const uint32_t mhu32_WAIT_TIME;
};
}
}

#endif

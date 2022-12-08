//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update package and summary.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPINFORMATIONWIDGET_H
#define C_SYVUPINFORMATIONWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QWinTaskbarProgress>

#include "C_OscSuSequences.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_SyvUpFileSizeInformation.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpInformationWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpInformationWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpInformationWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpInformationWidget(void) override;

   void SetViewIndex(const uint32_t ou32_ViewIndex);

   void SetConnected(void) const;
   void SetUpdateStarted(void) const;
   void SetUpdateNodeStarted(const uint32_t ou32_NodeIndex);
   void SetUpdateNodeSuccess(const uint32_t ou32_NodeIndex);
   void SetUpdateApplicationStarted(const uint32_t ou32_NodeIndex, const bool oq_IsParam,
                                    const bool oq_IsPemFile) const;
   void SetUpdateApplicationFinished(const uint32_t ou32_NodeIndex, const bool oq_IsParam, const bool oq_IsPemFile);
   void SetUpdateApplicationError(const uint32_t ou32_NodeIndex) const;
   void DiscardApplicationStatus(const uint32_t ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void) const;
   void UpdateDeviceInformation(const std::vector<uint32_t> & orc_NodeIndexes,
                                const std::vector<stw::opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void SetNodeProgress(const uint32_t ou32_NodeIndex, const uint8_t ou8_Progress) const;

   int32_t GetUpdatePackage(std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                            std::vector<uint32_t> & orc_NodesOrder, std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> * const opc_AllApplications =
                               NULL) const;

   void ResetSummary(void);
   void SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure = false,
                   const bool oq_Success = false) const;
   void InitUpdatePackage(const std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_Flashpackage,
                          const std::vector<uint32_t> & orc_Order, const std::vector<bool> & orc_IsFileBased,
                          const bool oq_IncludesCurrentNodeStatus);

   void UpdateProgress(const uint16_t ou16_Progress100, const bool oq_Finished = false);
   void StartElapsedTimer(void);
   void StopElapsedTimer(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdatePackageState(const int32_t os32_State) const;

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SyvUpInformationWidget * mpc_Ui;

   //Avoid call
   C_SyvUpInformationWidget(const C_SyvUpInformationWidget &);
   C_SyvUpInformationWidget & operator =(const C_SyvUpInformationWidget &) &;

   void m_UpdateTime(void) const;
   void m_UpdateDataTransfer(const uint64_t & oru64_OverallFlashedBytes, const bool oq_ShowDataRate);
   void m_UpdateLabel(void) const;
   void m_UpdateEstimatedWaitTime(const bool oq_IncludesCurrentNodeStatus);
   void m_UpdateDataRate(void);
   void m_UpdateWinProgress(const bool oq_Visible, const int32_t os32_Value);
   void m_UpdateProgressVisualization(const uint16_t ou16_Progress100, const bool oq_Finished = false) const;

   void m_HideBigUpdateSummary(void) const;
   void m_HideSmallUpdateSummary(void) const;

   void m_LoadUserSettings(void);
   void m_SaveUserSettings(void) const;

   QTimer mc_Timer;
   QElapsedTimer mc_ElapsedTimer;
   QWinTaskbarProgress * mpc_Progress;
   stw::opensyde_gui_logic::C_SyvUpFileSizeInformation mc_FileSizeInformation;
   uint32_t mu32_ItFile;
   uint32_t mu32_ItParamFile;
   uint32_t mu32_ItPemFile;
   uint32_t mu32_ViewIndex;
   uint64_t mu64_NodeStartTimeMs;
   uint64_t mu64_LastKnownDataRateSeconds;
   QString mc_EstimatedTime;
   QString mc_DataRateAbsoluteBytes;
   QString mc_DataRateBytesPerSecond;
   bool mq_ShowDataRateBytesPerSecond;
   uint64_t mu64_CurrentFlashedBytes;
   uint64_t mu64_FlashedBytesHistoryPrev;
   uint64_t mu64_FlashedBytesHistoryPrevPrev;
   uint64_t mu64_FlashedBytesTimestampPrev;
   uint64_t mu64_FlashedBytesTimestampPrevPrev;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

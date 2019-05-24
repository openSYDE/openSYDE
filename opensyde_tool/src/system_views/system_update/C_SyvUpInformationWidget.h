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

#include "C_OSCSuSequences.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_SyvUpFileSizeInformation.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpInformationWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpInformationWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpInformationWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpInformationWidget(void);

   void SetViewIndex(const stw_types::uint32 ou32_ViewIndex);

   void SetConnected(void) const;
   void SetUpdateStarted(void) const;
   void SetUpdateNodeStarted(const stw_types::uint32 ou32_NodeIndex);
   void SetUpdateNodeSuccess(const stw_types::uint32 ou32_NodeIndex);
   void SetUpdateApplicationStarted(const stw_types::uint32 ou32_NodeIndex, const bool oq_IsParam) const;
   void SetUpdateApplicationFinished(const stw_types::uint32 ou32_NodeIndex, const bool oq_IsParam);
   void SetUpdateApplicationError(const stw_types::uint32 ou32_NodeIndex) const;
   void DiscardApplicationStatus(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void) const;
   void UpdateDeviceInformation(const std::vector<stw_types::uint32> & orc_NodeIndexes,
                                const std::vector<stw_opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void SetNodeProgress(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_Progress) const;

   stw_types::sint32 GetUpdatePackage(
      std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
      std::vector<stw_types::uint32> & orc_NodesOrder,
      std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> * const opc_AllApplications = NULL) const;

   void ResetSummary(void);
   void SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure = false,
                   const bool oq_Success = false) const;
   void InitUpdatePackage(const std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_Flashpackage,
                          const std::vector<stw_types::uint32> & orc_Order, const std::vector<bool> & orc_IsFileBased,
                          const bool oq_IncludesCurrentNodeStatus);

   void UpdateProgress(const stw_types::uint16 ou16_Progress100, const bool oq_Finished = false);
   void StartElapsedTimer(void);
   void StopElapsedTimer(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdatePackageState(const stw_types::sint32 os32_State) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SyvUpInformationWidget * mpc_Ui;

   //Avoid call
   C_SyvUpInformationWidget(const C_SyvUpInformationWidget &);
   C_SyvUpInformationWidget & operator =(const C_SyvUpInformationWidget &);

   void m_UpdateTime(void) const;
   void m_UpdateDataTransfer(const stw_types::uint64 & oru64_OverallFlashedBytes, const bool oq_ShowDataRate);
   void m_UpdateLabel(void) const;
   void m_UpdateEstimatedWaitTime(const bool oq_IncludesCurrentNodeStatus);
   void m_UpdateDataRate(void);
   void m_UpdateWinProgress(const bool oq_Visible, const stw_types::sint32 os32_Value);
   void m_UpdateProgressVisualization(const stw_types::uint16 ou16_Progress100, const bool oq_Finished = false) const;

   void m_HideBigUpdateSummary(void) const;
   void m_HideSmallUpdateSummary(void) const;

   QTimer mc_Timer;
   QElapsedTimer mc_ElapsedTimer;
   QWinTaskbarProgress * mpc_Progress;
   stw_opensyde_gui_logic::C_SyvUpFileSizeInformation mc_FileSizeInformation;
   stw_types::uint32 mu32_ItFile;
   stw_types::uint32 mu32_ItParamFile;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint64 mu64_NodeStartTimeMs;
   stw_types::uint64 mu64_LastKnownDataRateS;
   QString mc_EstimatedTime;
   QString mc_DataRateAbsoluteBytes;
   QString mc_DataRateBytesPerSecond;
   bool mq_ShowDataRateBytesPerSecond;
   stw_types::uint64 mu64_CurrentFlashedBytes;
   stw_types::uint64 mu64_FlashedBytesHistoryPrev;
   stw_types::uint64 mu64_FlashedBytesHistoryPrevPrev;
   stw_types::uint64 mu64_FlashedBytesTimestampPrev;
   stw_types::uint64 mu64_FlashedBytesTimestampPrevPrev;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

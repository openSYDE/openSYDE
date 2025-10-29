//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update package and summary.

   Widget to put together update package and update summary widgets and handle some progress calculation.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFileInfo>

#include "stwerrors.hpp"
#include "C_SyvUpInformationWidget.hpp"
#include "ui_C_SyvUpInformationWidget.h"
#include "C_PuiSvData.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OscHexFile.hpp"
#include "C_OscParamSetHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpInformationWidget::C_SyvUpInformationWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpInformationWidget),
   mu32_ItFile(0),
   mu32_ItParamFile(0U),
   mu32_ItPemFile(0U),
   mu32_ViewIndex(0U),
   mu64_NodeStartTimeMs(0ULL),
   mu64_LastKnownDataRateSeconds(0ULL),
   mu64_CurrentFlashedBytes(0ULL),
   mu64_FlashedBytesHistoryPrev(0ULL),
   mu64_FlashedBytesHistoryPrevPrev(0ULL),
   mu64_FlashedBytesTimestampPrev(0ULL),
   mu64_FlashedBytesTimestampPrevPrev(0ULL)
{
   this->mpc_Ui->setupUi(this);

   // splitter
   this->mpc_Ui->pc_SplitterVert->SetMargins(10, 38);
   this->mpc_Ui->pc_SplitterVert->setStretchFactor(0, 1);

   this->ResetSummary();

   this->m_UpdateEstimatedWaitTime(false);

   this->mc_Timer.setInterval(1000);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpInformationWidget::m_UpdateTime);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpInformationWidget::m_UpdateDataRate);
   connect(this->mpc_Ui->pc_WidgetUpdatePackage, &C_SyvUpPacWidget::SigUpdatePackageState,
           this, &C_SyvUpInformationWidget::SigUpdatePackageState);

   connect(this->mpc_Ui->pc_WidgetUpdateSummary, &C_SyvUpSummaryWidget::SigHideBigSummaryWidget,
           this, &C_SyvUpInformationWidget::m_HideBigUpdateSummary);
   connect(this->mpc_Ui->pc_WidgetUpdateSummarySmall, &C_SyvUpSummaryWidgetSmall::SigHideSmallSummaryWidget,
           this, &C_SyvUpInformationWidget::m_HideSmallUpdateSummary);
   connect(this, &C_SyvUpInformationWidget::SigInvalidView, this->mpc_Ui->pc_WidgetUpdatePackage,
           &C_SyvUpPacWidget::DisableUpdatePackage);
} //lint !e429  //no memory leak because of the parent of pc_Button and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpInformationWidget::~C_SyvUpInformationWidget()
{
   m_SaveUserSettings();

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the view index of sub widgets.

   \param[in]  ou32_ViewIndex    View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetViewIndex(const uint32_t ou32_ViewIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->mpc_Ui->pc_WidgetUpdatePackage->SetViewIndex(ou32_ViewIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward connected state to update package widget.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetConnected(void) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetConnected();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward update started state to update package widget.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateStarted(void) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateStarted();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that the node update was started

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateNodeStarted(const uint32_t ou32_NodeIndex)
{
   Q_UNUSED(ou32_NodeIndex)
   this->mu64_NodeStartTimeMs = this->mc_ElapsedTimer.elapsed();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that the node update was finished

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateNodeSuccess(const uint32_t ou32_NodeIndex)
{
   const uint64_t u64_NodeEndTimeMs = this->mc_ElapsedTimer.elapsed();

   this->mc_FileSizeInformation.SetElapsedTimeForNode(ou32_NodeIndex, u64_NodeEndTimeMs - this->mu64_NodeStartTimeMs);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that the application update was started

   \param[in]  ou32_NodeIndex    Index of node
   \param[in]  oq_IsParam        Flag if "application" was a parameter set file
   \param[in]  oq_IsPemFile      Flag if "application" was a PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateApplicationStarted(const uint32_t ou32_NodeIndex, const bool oq_IsParam,
                                                           const bool oq_IsPemFile) const
{
   Q_UNUSED(oq_IsParam)
   Q_UNUSED(oq_IsPemFile)
   this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationStarted(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that the application update was finished

   \param[in]  ou32_NodeIndex    Index of node
   \param[in]  oq_IsParam        Flag if "application" was a parameter set file
   \param[in]  oq_IsPemFile      Flag if "application" was a PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateApplicationFinished(const uint32_t ou32_NodeIndex, const bool oq_IsParam,
                                                            const bool oq_IsPemFile)
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationFinished(ou32_NodeIndex);

   // Count file up to assign the progress data to the correct file
   if (oq_IsParam == true)
   {
      ++this->mu32_ItParamFile;
   }
   else if (oq_IsPemFile == true)
   {
      ++this->mu32_ItPemFile;
   }
   else
   {
      ++this->mu32_ItFile;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that errors occured on application update

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateApplicationError(const uint32_t ou32_NodeIndex) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateApplicationError(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward trigger for application information discard

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::DiscardApplicationStatus(const uint32_t ou32_NodeIndex) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->DiscardApplicationStatus(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward information that the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetUpdateFinished(void) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetUpdateFinished();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward disconnected state to update package widget.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetDisconnected(void) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetDisconnected();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward update device information

   \param[in]  orc_NodeIndexes         Node indices
   \param[in]  orc_DeviceInformation   Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::UpdateDeviceInformation(const std::vector<uint32_t> & orc_NodeIndexes,
                                                       const std::vector<stw::opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->UpdateDeviceInformation(orc_NodeIndexes, orc_DeviceInformation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward set node progress

   \param[in]  ou32_NodeIndex    Index of node
   \param[in]  ou8_Progress      Entire progress of node of update process
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetNodeProgress(const uint32_t ou32_NodeIndex, const uint8_t ou8_Progress) const
{
   this->mpc_Ui->pc_WidgetUpdatePackage->SetNodeProgress(ou32_NodeIndex, ou8_Progress);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward update package.

   \param[in,out]  orc_ApplicationsToWrite   Applications to write
   \param[in,out]  orc_NodesOrder            Nodes order
   \param[in,out]  opc_AllApplications       All applications

   \return
   STW error code, see called function for detailed description
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpInformationWidget::GetUpdatePackage(
   std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
   std::vector<uint32_t> & orc_NodesOrder,
   std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> * const opc_AllApplications) const
{
   return this->mpc_Ui->pc_WidgetUpdatePackage->GetUpdatePackage(orc_ApplicationsToWrite, orc_NodesOrder,
                                                                 opc_AllApplications);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset the summary widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::ResetSummary(void)
{
   //File size reset is important
   this->mq_ShowDataRateBytesPerSecond = false;
   this->mc_FileSizeInformation.Reset();

   //Reset summary widgets
   this->SetHeading("", C_GtGetText::h_GetText("Disconnected!"), false, false);
   this->mpc_Ui->pc_WidgetUpdateSummary->SetElapsedTime("00:00:00");
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetElapsedTime("00:00:00");
   this->mpc_Ui->pc_WidgetUpdateSummary->SetProgress(0);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetProgress(0);

   this->mc_EstimatedTime = "";
   this->mc_DataRateAbsoluteBytes = C_GtGetText::h_GetText("Data Transfer 0.0 kB/0.0 kB");
   this->mc_DataRateBytesPerSecond = C_GtGetText::h_GetText("0 B/s");

   this->m_UpdateEstimatedWaitTime(false);
   this->m_UpdateLabel();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle new heading.

   \param[in]  orc_Icon    Icon
   \param[in]  orc_Text    Heading text
   \param[in]  oq_Failure  Failure
   \param[in]  oq_Success  Success
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure,
                                          const bool oq_Success) const
{
   this->mpc_Ui->pc_WidgetUpdateSummary->SetHeading(orc_Icon, orc_Text, oq_Failure, oq_Success);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetHeading(orc_Icon, orc_Text, oq_Failure, oq_Success);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set known update configuration for flash progress assumptions

   \param[in]  orc_Flashpackage              Complete flash package
   \param[in]  orc_Order                     Flash order
   \param[in]  orc_IsFileBased               File based flag for all included devices
   \param[in]  oq_IncludesCurrentNodeStatus  Flag if the current call includes the current node status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::InitUpdatePackage(
   const std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_Flashpackage,
   const std::vector<uint32_t> & orc_Order, const std::vector<bool> & orc_IsFileBased,
   const bool oq_IncludesCurrentNodeStatus)
{
   this->mu32_ItFile = 0UL;
   this->mu32_ItParamFile = 0UL;
   this->mu32_ItPemFile = 0UL;
   this->mc_FileSizeInformation.Reset();
   this->mc_FileSizeInformation.ReserveSpace(orc_Order.size());
   //Search for matching value in order
   for (uint32_t u32_ItOrder = 0; u32_ItOrder < orc_Order.size(); ++u32_ItOrder)
   {
      const uint32_t u32_NodeIndex = orc_Order[u32_ItOrder];
      if (u32_NodeIndex < orc_Flashpackage.size())
      {
         //Check if there are any files
         const C_OscSuSequences::C_DoFlash & rc_Device = orc_Flashpackage[u32_NodeIndex];
         if (rc_Device.c_FilesToFlash.size() > 0)
         {
            //Convert and add hex files
            std::vector<uint64_t> c_Files;
            std::vector<uint64_t> c_ParamFiles;
            c_Files.reserve(rc_Device.c_FilesToFlash.size());
            for (uint32_t u32_ItFile = 0; u32_ItFile < rc_Device.c_FilesToFlash.size(); ++u32_ItFile)
            {
               const stw::scl::C_SclString & rc_File = rc_Device.c_FilesToFlash[u32_ItFile];
               bool q_UseFileSize = true;
               //Check which case it is
               if ((u32_NodeIndex < orc_IsFileBased.size()) && (orc_IsFileBased[u32_NodeIndex] == false))
               {
                  //Do the address based byte count stuff
                  C_OscHexFile c_HexFile;
                  const uint32_t u32_Result = c_HexFile.LoadFromFile(rc_File.c_str());
                  if (u32_Result == stw::hex_file::NO_ERR)
                  {
                     q_UseFileSize = false;
                     c_Files.push_back(static_cast<uint64_t>(c_HexFile.ByteCount()));
                  }
               }
               //Check if fall back to file based approach is necessary
               if (q_UseFileSize == true)
               {
                  const QFileInfo c_FileInfo(rc_File.c_str());
                  if (c_FileInfo.exists() == true)
                  {
                     c_Files.push_back(c_FileInfo.size());
                  }
                  else
                  {
                     c_Files.push_back(0ULL);
                  }
               }
            }
            //New vector for the parameter set files to the other flashed ones
            c_ParamFiles.reserve(rc_Device.c_FilesToWriteToNvm.size());
            for (uint32_t u32_ItFile = 0; u32_ItFile < rc_Device.c_FilesToWriteToNvm.size(); ++u32_ItFile)
            {
               const stw::scl::C_SclString & rc_File = rc_Device.c_FilesToWriteToNvm[u32_ItFile];
               C_OscParamSetHandler c_FileHandler;
               if (c_FileHandler.ReadFile(rc_File, false, true, NULL) == C_NO_ERR)
               {
                  const C_OscParamSetInterpretedData & rc_InterpretedData = c_FileHandler.GetInterpretedData();
                  if (rc_InterpretedData.c_InterpretedNodes.size() >= 1UL)
                  {
                     const C_OscParamSetInterpretedNode & rc_Node = rc_InterpretedData.c_InterpretedNodes[0UL];
                     uint64_t u64_Size = 0ULL;
                     for (uint32_t u32_ItDp = 0UL; u32_ItDp < rc_Node.c_DataPools.size(); ++u32_ItDp)
                     {
                        const C_OscParamSetInterpretedDataPool & rc_Dp = rc_Node.c_DataPools[u32_ItDp];
                        for (uint32_t u32_ItLi = 0UL; u32_ItLi < rc_Dp.c_Lists.size(); ++u32_ItLi)
                        {
                           const C_OscParamSetInterpretedList & rc_Li = rc_Dp.c_Lists[u32_ItLi];
                           for (uint32_t u32_ItEl = 0UL; u32_ItEl < rc_Li.c_Elements.size(); ++u32_ItEl)
                           {
                              const C_OscParamSetInterpretedElement & rc_El = rc_Li.c_Elements[u32_ItEl];
                              u64_Size += rc_El.c_NvmValue.GetSizeByte();
                           }
                           //Write 2 byte CRC twice (invalidate+validate)
                           u64_Size += 2 * 2ULL;
                        }
                     }
                     c_ParamFiles.push_back(u64_Size);
                  }
                  else
                  {
                     c_ParamFiles.push_back(0ULL);
                  }
               }
               else
               {
                  c_ParamFiles.push_back(0ULL);
               }
            }
            this->mc_FileSizeInformation.AppendFiles(u32_NodeIndex, c_Files, c_ParamFiles);
         }
      }
   }
   m_UpdateEstimatedWaitTime(oq_IncludesCurrentNodeStatus);
   m_UpdateDataTransfer(0ULL, false);
   m_UpdateLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update progress value

   \param[in]  ou16_Progress100  Progress in percent
   \param[in]  oq_Finished       Finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::UpdateProgress(const uint16_t ou16_Progress100, const bool oq_Finished)
{
   // TODO BAY: Handle PEM files
   if (oq_Finished == false)
   {
      uint16_t u16_ActualProgress = 0;
      uint32_t u32_Counter = 0;
      uint64_t u64_FlashedBytes = 0;
      bool q_Stop = false;

      //Add file info
      for (uint32_t u32_ItDevice = 0;
           (u32_ItDevice < this->mc_FileSizeInformation.GetNumOtherFiles()) && (q_Stop == false); ++u32_ItDevice)
      {
         const uint32_t u32_NumFiles = this->mc_FileSizeInformation.GetNumOtherFilesForDevice(u32_ItDevice);
         for (uint32_t u32_ItFile = 0; (u32_ItFile < u32_NumFiles) && (q_Stop == false); ++u32_ItFile)
         {
            const uint64_t u64_CurFileSize = this->mc_FileSizeInformation.GetOtherFileSizeForDevice(u32_ItDevice,
                                                                                                    u32_ItFile);
            if (u32_Counter == this->mu32_ItFile)
            {
               const uint64_t u64_OverallFilesSize = this->mc_FileSizeInformation.GetOverallFilesSize();
               //Match
               //Adapt to progress
               u64_FlashedBytes += (u64_CurFileSize * static_cast<uint64_t>(ou16_Progress100)) / 100ULL;
               //Calculate overall progress
               if (u64_OverallFilesSize != 0U)
               {
                  u16_ActualProgress = static_cast<uint16_t>((u64_FlashedBytes * 100ULL) / u64_OverallFilesSize);
               }
               else
               {
                  u16_ActualProgress = 100U;
               }
               //Stop iteration
               q_Stop = true;
            }
            else
            {
               u64_FlashedBytes += u64_CurFileSize;
            }
            //Update counter
            ++u32_Counter;
         }
      }
      //Add param info
      //New counter
      u32_Counter = 0UL;
      for (uint32_t u32_ItDevice = 0;
           (u32_ItDevice < this->mc_FileSizeInformation.GetNumParamFiles()) && (q_Stop == false);
           ++u32_ItDevice)
      {
         const uint32_t u32_NumFiles = this->mc_FileSizeInformation.GetNumParamFilesForDevice(u32_ItDevice);
         for (uint32_t u32_ItFile = 0; (u32_ItFile < u32_NumFiles) && (q_Stop == false); ++u32_ItFile)
         {
            const uint64_t u64_CurFileSize = this->mc_FileSizeInformation.GetParamFileSizeForDevice(u32_ItDevice,
                                                                                                    u32_ItFile);
            if (u32_Counter == this->mu32_ItParamFile)
            {
               const uint64_t u64_OverallFilesSize = this->mc_FileSizeInformation.GetOverallFilesSize();
               //Match
               //Adapt to progress
               u64_FlashedBytes += (u64_CurFileSize * static_cast<uint64_t>(ou16_Progress100)) / 100ULL;
               //Calculate overall progress
               if (u64_OverallFilesSize != 0U)
               {
                  u16_ActualProgress = static_cast<uint16_t>((u64_FlashedBytes * 100ULL) / u64_OverallFilesSize);
               }
               else
               {
                  u16_ActualProgress = 100U;
               }
               //Stop iteration
               q_Stop = true;
            }
            else
            {
               u64_FlashedBytes += u64_CurFileSize;
            }
            //Update counter
            ++u32_Counter;
         }
      }

      //Convert to overall progress
      m_UpdateProgressVisualization(u16_ActualProgress, oq_Finished);

      //Update data transfer
      m_UpdateDataTransfer(u64_FlashedBytes, true);

      //Handle bytes per second info (only update if change)
      if ((static_cast<uint64_t>(this->mc_ElapsedTimer.elapsed()) != this->mu64_FlashedBytesTimestampPrev) &&
          (this->mu64_FlashedBytesHistoryPrev != this->mu64_CurrentFlashedBytes))
      {
         this->mu64_FlashedBytesHistoryPrevPrev = this->mu64_FlashedBytesHistoryPrev;
         this->mu64_FlashedBytesTimestampPrevPrev = this->mu64_FlashedBytesTimestampPrev;
         this->mu64_FlashedBytesHistoryPrev = this->mu64_CurrentFlashedBytes;
         this->mu64_FlashedBytesTimestampPrev = this->mc_ElapsedTimer.elapsed();
      }
   }
   else
   {
      uint64_t u64_AverageDataRate;
      m_UpdateProgressVisualization(ou16_Progress100, oq_Finished);

      //Update data transfer
      m_UpdateDataTransfer(this->mc_FileSizeInformation.GetOverallFilesSize(), true);
      //Calculate data rate base on current elapsed time and flashed byte count
      if (this->mc_ElapsedTimer.elapsed() > 0)
      {
         u64_AverageDataRate = (this->mu64_CurrentFlashedBytes * 1000ULL) /
                               static_cast<uint64_t>(this->mc_ElapsedTimer.elapsed());
      }
      else
      {
         u64_AverageDataRate = 0ULL;
      }
      //Remember data rate
      if (u64_AverageDataRate != 0ULL)
      {
         //Remember for next update steps
         this->mc_FileSizeInformation.SaveUserSettings(this->mu32_ViewIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::StartElapsedTimer(void)
{
   this->mc_Timer.start();
   this->mc_ElapsedTimer.start();
   //Reset flashed byte count
   this->mu64_CurrentFlashedBytes = 0ULL;
   this->mu64_FlashedBytesHistoryPrev = 0ULL;
   this->mu64_FlashedBytesHistoryPrevPrev = 0ULL;
   this->mu64_FlashedBytesTimestampPrev = 0ULL;
   this->mu64_FlashedBytesTimestampPrevPrev = 0ULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Stop timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::StopElapsedTimer(void)
{
   this->mc_Timer.stop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::showEvent(QShowEvent * const opc_Event)
{
   m_LoadUserSettings();
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update time and forward to update summaries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateTime(void) const
{
   //Current time
   const uint64_t u64_ElapsedMs = this->mc_ElapsedTimer.elapsed();
   const uint64_t u64_ElapsedSeconds = (u64_ElapsedMs / 1000ULL) % 60ULL;
   const uint64_t u64_ElapsedMin = (u64_ElapsedMs / 60000ULL) % 60ULL;
   const uint64_t u64_ElapsedHours = u64_ElapsedMs / 3600000ULL;

   const QString c_Time = static_cast<QString>("%1:%2:%3").arg(u64_ElapsedHours, 2, 10, QChar('0')).
                          arg(u64_ElapsedMin, 2, 10, QChar('0')).arg(u64_ElapsedSeconds, 2, 10, QChar('0'));

   this->mpc_Ui->pc_WidgetUpdateSummary->SetElapsedTime(c_Time);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetElapsedTime(c_Time);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update transfer data count

 *

   \param[in]  oru64_OverallFlashedBytes  Overall number of flashed bytes
   \param[in]  oq_ShowDataRate            Flag to request show of data rate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateDataTransfer(const uint64_t & oru64_OverallFlashedBytes,
                                                    const bool oq_ShowDataRate)
{
   QString c_Value;
   QString c_Progress;
   QString c_Complete;
   QString c_Unit;
   const float64_t f64_OverallFlashedBytes = static_cast<float64_t>(oru64_OverallFlashedBytes);
   const float64_t f64_OverallFilesSize = static_cast<float64_t>(this->mc_FileSizeInformation.GetOverallFilesSize());

   this->mu64_CurrentFlashedBytes = oru64_OverallFlashedBytes;
   this->mq_ShowDataRateBytesPerSecond = oq_ShowDataRate;

   if (this->mc_FileSizeInformation.GetOverallFilesSize() > (1024ULL * 1024ULL * 1024ULL))
   {
      //GB
      c_Unit = "GB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (1024.0 * 1024.0 * 1024.0), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (1024.0 * 1024.0 * 1024.0), 'f', 1);
   }
   else if (this->mc_FileSizeInformation.GetOverallFilesSize() > (1024ULL * 1024ULL))
   {
      //MB
      c_Unit = "MB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (1024.0 * 1024.0), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (1024.0 * 1024.0), 'f', 1);
   }
   else if (this->mc_FileSizeInformation.GetOverallFilesSize() > 1024ULL)
   {
      //kB
      c_Unit = "kB";
      c_Progress = QString::number(f64_OverallFlashedBytes / 1024.0, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / static_cast<float32_t>(1024ULL), 'f', 1);
   }
   else
   {
      //Bytes
      c_Unit = "B";
      c_Progress = QString::number(f64_OverallFlashedBytes, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize, 'f', 1);
   }
   //Translation: 1:Flashed bytes, 2:File size, 3: File size unit
   c_Value = static_cast<QString>("%1 %3/%2 %3").arg(c_Progress).arg(c_Complete).arg(c_Unit);

   //Translation: 1:File progress
   this->mc_DataRateAbsoluteBytes = static_cast<QString>(C_GtGetText::h_GetText("Data transfer %1")).arg(c_Value);

   m_UpdateLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update displayed data transfer information.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateLabel(void) const
{
   QString c_DataRateCombined;

   if (this->mq_ShowDataRateBytesPerSecond == true)
   {
      c_DataRateCombined = this->mc_DataRateAbsoluteBytes + " @ " + this->mc_DataRateBytesPerSecond;
   }
   else
   {
      c_DataRateCombined = this->mc_DataRateAbsoluteBytes;
   }

   if (this->mc_EstimatedTime.compare("") == 0)
   {
      this->mpc_Ui->pc_WidgetUpdateSummary->SetDataTransfer(c_DataRateCombined);
   }
   else
   {
      this->mpc_Ui->pc_WidgetUpdateSummary->SetDataTransfer(static_cast<QString>("%1\n%2").arg(
                                                               this->mc_EstimatedTime).arg(
                                                               c_DataRateCombined));
   }

   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetDataTransfer(c_DataRateCombined);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetEstimatedTime(this->mc_EstimatedTime);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update estimated wait time calculation

   \param[in]  oq_IncludesCurrentNodeStatus  Flag if the current call includes the current node status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateEstimatedWaitTime(const bool oq_IncludesCurrentNodeStatus)
{
   const QString c_Default = C_GtGetText::h_GetText("Estimated waiting time --.");

   if (oq_IncludesCurrentNodeStatus == true)
   {
      if (this->mc_FileSizeInformation.GetOverallFilesSize() > 0ULL)
      {
         bool q_Worked;
         const uint64_t u64_WaitingTimeSeconds = this->mc_FileSizeInformation.GetEstimatedTimeSeconds(&q_Worked);
         //After all estimations were attempted check if any succeeded
         if (q_Worked == true)
         {
            const uint64_t u64_WaitingTimeMin = u64_WaitingTimeSeconds / 60ULL;

            if (u64_WaitingTimeMin == 0ULL)
            {
               this->mc_EstimatedTime = static_cast<QString>(C_GtGetText::h_GetText(
                                                                "Estimated waiting time less than one minute."));
            }
            else
            {
               //One min for rounding errors has to be added before
               //-> we should round ceil (include the last seconds) but we are rounding floor (integer)
               const uint64_t u64_WaitingTimeHours = (u64_WaitingTimeMin + 1ULL) / 60ULL;
               //lint -e{774,831,948} False positive
               if (u64_WaitingTimeHours == 0ULL)
               {
                  this->mc_EstimatedTime =
                     static_cast<QString>(C_GtGetText::h_GetText("Estimated waiting time %1 min.")).arg(
                        u64_WaitingTimeMin + 1ULL);
               }
               else
               {
                  this->mc_EstimatedTime =
                     static_cast<QString>(C_GtGetText::h_GetText("Estimated waiting time %1 h %2 min.")).arg(
                        u64_WaitingTimeHours).arg((u64_WaitingTimeMin + 1ULL) % 60ULL);
               }
            }
         }
         else
         {
            this->mc_EstimatedTime = c_Default;
         }
      }
      else
      {
         this->mc_EstimatedTime = c_Default;
      }
   }
   else
   {
      this->mc_EstimatedTime = c_Default;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data rate history
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateDataRate()
{
   if (this->mu64_FlashedBytesHistoryPrev > 0ULL)
   {
      QString c_DataTransferRate;
      QString c_Unit;
      const uint64_t u64_Timespan = this->mu64_FlashedBytesTimestampPrev - this->mu64_FlashedBytesTimestampPrevPrev;
      const float64_t f64_Timespan = static_cast<float64_t>(u64_Timespan);
      const uint64_t u64_NumberOfFlashedBytesInTimeSpan = this->mu64_FlashedBytesHistoryPrev -
                                                          this->mu64_FlashedBytesHistoryPrevPrev;
      const float64_t f64_NumberOfFlashedBytesInTimeSpan = static_cast<float64_t>(u64_NumberOfFlashedBytesInTimeSpan);
      const float64_t f64_NumberOfFlashedBytesThisSecond = f64_NumberOfFlashedBytesInTimeSpan / (f64_Timespan * 0.001);
      const uint64_t u64_NumberOfFlashedBytesThisSecond = static_cast<uint64_t>(f64_NumberOfFlashedBytesThisSecond);

      //Remember last value

      //Update data rate
      if (u64_NumberOfFlashedBytesThisSecond > (1024ULL * 1024ULL * 1024ULL))
      {
         //GB
         c_Unit = "GB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / (1024.0 * 1024.0 * 1024.0), 'f', 0);
      }
      else if (u64_NumberOfFlashedBytesThisSecond > (1024ULL * 1024ULL))
      {
         //MB
         c_Unit = "MB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / (1024.0 * 1024.0), 'f', 0);
      }
      else if (u64_NumberOfFlashedBytesThisSecond > 1024ULL)
      {
         //kB
         c_Unit = "kB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / 1024.0, 'f', 0);
      }
      else
      {
         //Bytes
         c_Unit = "B";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond, 'f', 0);
      }
      this->mc_DataRateBytesPerSecond =
         static_cast<QString>(C_GtGetText::h_GetText("%1 %2/s")).arg(c_DataTransferRate).arg(c_Unit);

      m_UpdateLabel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward progress to update visualization (bar/circle)

   \param[in]  ou16_Progress100  progress as number between 0 and 100
   \param[in]  oq_Finished       Flag if already finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_UpdateProgressVisualization(const uint16_t ou16_Progress100,
                                                             const bool oq_Finished) const
{
   this->mpc_Ui->pc_WidgetUpdateSummary->SetProgress(ou16_Progress100, oq_Finished);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->SetProgress(ou16_Progress100, oq_Finished);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide big update summary widget on button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_HideBigUpdateSummary(void) const
{
   this->mpc_Ui->pc_SplitterVert->setVisible(false);
   this->mpc_Ui->pc_LayoutNoSplitter->addWidget(this->mpc_Ui->pc_WidgetUpdatePackage);
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide small update summary widget on button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_HideSmallUpdateSummary(void) const
{
   this->mpc_Ui->pc_WidgetUpdateSummarySmall->setVisible(false);
   this->mpc_Ui->pc_LayoutSplitter->insertWidget(0, this->mpc_Ui->pc_WidgetUpdatePackage);
   this->mpc_Ui->pc_SplitterVert->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_LoadUserSettings()
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // restore configuration of the view
   if (pc_View != NULL)
   {
      const C_UsSystemView c_UserView = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName().c_str());

      //splitter
      int32_t s32_LastSegmentWidth  = c_UserView.GetUpdateSplitterHorizontal();

      //Revert to default if necessary
      if (s32_LastSegmentWidth <= 0)
      {
         s32_LastSegmentWidth = 320;
      }

      this->mpc_Ui->pc_SplitterVert->SetSecondSegment(s32_LastSegmentWidth);

      if (c_UserView.GetUpdateSummaryBig() == true)
      {
         this->m_HideSmallUpdateSummary();
      }
      else
      {
         this->m_HideBigUpdateSummary();
      }
   }
   this->mc_FileSizeInformation.LoadUserSettings(this->mu32_ViewIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpInformationWidget::m_SaveUserSettings() const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      // splitter
      const QList<int32_t> c_Sizes = this->mpc_Ui->pc_SplitterVert->sizes();
      if (c_Sizes.count() > 1)
      {
         C_UsHandler::h_GetInstance()->SetProjSvUpdateSplitterHorizontal(pc_View->GetName().c_str(), c_Sizes.at(1));
      }

      // summary widget style type
      C_UsHandler::h_GetInstance()->SetProjSvUpdateSummaryBig(pc_View->GetName().c_str(),
                                                              this->mpc_Ui->pc_SplitterVert->isVisibleTo(this));
   }
}

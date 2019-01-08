//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing the progress and the state of the update process

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include <QFileInfo>
#include <QFontMetrics>
#include <QWinTaskbarButton>
#include "stwerrors.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_OsyHexFile.h"
#include "C_PuiProject.h"
#include "C_PuiSvHandler.h"
#include "C_SyvUpSummaryWidget.h"
#include "ui_C_SyvUpSummaryWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpSummaryWidget::C_SyvUpSummaryWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpSummaryWidget),
   mu32_ItFile(0),
   mu32_ViewIndex(0),
   mu64_OverallFilesSize(0ULL),
   mu64_LastKnownDataRateS(0ULL),
   mu64_CurrentFlashedBytes(0ULL),
   mu64_FlashedBytesHistoryPrev(0ULL),
   mu64_FlashedBytesHistoryPrevPrev(0ULL),
   mu64_FlashedBytesTimestampPrev(0ULL),
   mu64_FlashedBytesTimestampPrevPrev(0ULL)
{
   QWidget * const pc_Top = C_OgeWiUtil::h_GetWidgetUnderNextPopUp(this);
   QWinTaskbarButton * const pc_Button = new QWinTaskbarButton(pc_Top);
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   mpc_Ui->setupUi(this);

   //handle task bar button
   pc_Button->setWindow(pc_Top->windowHandle());
   this->mpc_Progress = pc_Button->progress();

   Reset();

   //Deactivate debug string
   this->mpc_Ui->pc_LabelHeadingIcon->setText("");

   //Init update rate
   if (pc_View != NULL)
   {
      const uint32 u32_Checksum = pc_View->CalcSetupHash();
      const C_UsSystemView c_View = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
      const QMap<uint32, uint64> & rc_UpdateDataRateHistory = c_View.GetUpdateDataRateHistory();
      const QMap<uint32, uint64>::const_iterator c_It = rc_UpdateDataRateHistory.find(u32_Checksum);
      if (c_It != rc_UpdateDataRateHistory.end())
      {
         this->mu64_LastKnownDataRateS = c_It.value();
      }
   }
   m_UpdateEstimatedWaitTime();

   this->mc_Timer.setInterval(1000);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpSummaryWidget::m_UpdateTime);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpSummaryWidget::m_UpdateDataRate);
   //lint -e{429}  no memory leak because of the parent of pc_Button and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpSummaryWidget::~C_SyvUpSummaryWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reset the summary widget

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::Reset(void)
{
   //File size reset is important
   this->mq_ShowDataRateBytesPerSecond = false;
   this->mu64_OverallFilesSize = 0ULL;
   //Reset labels
   this->mpc_Ui->pc_LabelHeadingIcon->setVisible(false);
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("System disconnected!"));
   this->mpc_Ui->pc_LabelElapsed->setText("00:00:00");
   this->mc_EstimatedTime = "";
   this->mc_DataRateAbsoluteBytes = C_GtGetText::h_GetText("Data Transfer 0.0 KB/0.0 KB");
   this->mc_DataRateBytesPerSecond = C_GtGetText::h_GetText("0 Bytes/sec");
   m_UpdateWinProgress(false, 0);
   this->mpc_Ui->pc_WidgetProgress->SetProgress100(0);
   this->mpc_Ui->pc_WidgetProgress->SetActiveColor(mc_STYLE_GUIDE_COLOR_13);
   //Initial style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Success", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Error", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelElapsed, "Error", false);
   m_UpdateEstimatedWaitTime();
   m_UpdateLabelLayout();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set heading text

   \param[in] orc_Text Text

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure,
                                      const bool oq_Success)
{
   if (orc_Icon.compare("") != 0)
   {
      m_UpdateWinProgress(false, 0);
      this->mpc_Ui->pc_LabelHeadingIcon->setVisible(true);
      this->mpc_Ui->pc_LabelHeadingIcon->setPixmap(QIcon(orc_Icon).pixmap(mc_ICON_SIZE_24));
   }
   else
   {
      this->mpc_Ui->pc_LabelHeadingIcon->setVisible(false);
   }
   this->mpc_Ui->pc_LabelHeading->setText(orc_Text);

   if (oq_Failure == true)
   {
      this->mpc_Ui->pc_WidgetProgress->SetActiveColor(mc_STYLE_GUIDE_COLOR_24);
   }
   else
   {
      this->mpc_Ui->pc_WidgetProgress->SetActiveColor(mc_STYLE_GUIDE_COLOR_13);
   }

   //Style label
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Success", oq_Success);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelHeading, "Error", oq_Failure);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelElapsed, "Error", oq_Failure);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set known update configuration for flash progress assumptions

   \param[in] orc_Flashpackage Complete flash package
   \param[in] orc_Order        Flash order

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::InitUpdatePackage(const std::vector<C_OSCSuSequences::C_DoFlash> & orc_Flashpackage,
                                             const std::vector<uint32> & orc_Order,
                                             const std::vector<bool> & orc_IsFileBased)
{
   this->mu32_ItFile = 0;
   this->mu64_OverallFilesSize = 0ULL;
   this->mc_FileSizesByte.clear();
   this->mc_FileSizesByte.reserve(orc_Order.size());
   //Search for matching value in order
   for (uint32 u32_ItOrder = 0; u32_ItOrder < orc_Order.size(); ++u32_ItOrder)
   {
      const uint32 u32_NodeIndex = orc_Order[u32_ItOrder];
      if (u32_NodeIndex < orc_Flashpackage.size())
      {
         //Check if there are any files
         const C_OSCSuSequences::C_DoFlash & rc_Device = orc_Flashpackage[u32_NodeIndex];
         if (rc_Device.c_FilesToFlash.size() > 0)
         {
            //Convert and add files
            std::vector<uint64> c_Files;
            c_Files.reserve(rc_Device.c_FilesToFlash.size());
            for (uint32 u32_ItFile = 0; u32_ItFile < rc_Device.c_FilesToFlash.size(); ++u32_ItFile)
            {
               const stw_scl::C_SCLString & rc_File = rc_Device.c_FilesToFlash[u32_ItFile];
               bool q_UseFileSize = true;
               //Check which case it is
               if ((u32_NodeIndex < orc_IsFileBased.size()) && (orc_IsFileBased[u32_NodeIndex] == false))
               {
                  //Do the address based byte count stuff
                  C_OsyHexFile c_HexFile;
                  const uint32 u32_Result = c_HexFile.LoadFromFile(rc_File.c_str());
                  if (u32_Result == stw_hex_file::NO_ERR)
                  {
                     q_UseFileSize = false;
                     c_Files.push_back(static_cast<uint64>(c_HexFile.ByteCount()));
                     this->mu64_OverallFilesSize += static_cast<uint64>(c_HexFile.ByteCount());
                  }
               }
               //Check if fall back to file based approach is necessary
               if (q_UseFileSize == true)
               {
                  const QFileInfo c_FileInfo(rc_File.c_str());
                  if (c_FileInfo.exists() == true)
                  {
                     c_Files.push_back(c_FileInfo.size());
                     this->mu64_OverallFilesSize += static_cast<uint64>(c_FileInfo.size());
                  }
                  else
                  {
                     c_Files.push_back(0ULL);
                  }
               }
            }
            this->mc_FileSizesByte.push_back(c_Files);
         }
      }
   }
   m_UpdateEstimatedWaitTime();
   m_UpdateDataTransfer(0ULL, false);
   m_UpdateLabelLayout();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update progress value

   \param[in] ou16_Progress100 Progress in percent

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetProgressFile100(const uint16 ou16_Progress100, const bool oq_Finished)
{
   if (oq_Finished == false)
   {
      uint16 u16_ActualProgress = 0;
      uint32 u32_Counter = 0;
      uint64 u64_FlashedBytes = 0;
      bool q_Stop = false;

      for (uint32 u32_ItDevice = 0; (u32_ItDevice < this->mc_FileSizesByte.size()) && (q_Stop == false); ++u32_ItDevice)
      {
         const std::vector<uint64> & rc_DeviceFiles = this->mc_FileSizesByte[u32_ItDevice];
         for (uint32 u32_ItFile = 0; (u32_ItFile < rc_DeviceFiles.size()) && (q_Stop == false); ++u32_ItFile)
         {
            const uint64 & ru64_CurFileSize = rc_DeviceFiles[u32_ItFile];
            if (u32_Counter == this->mu32_ItFile)
            {
               //Match
               //Adapt to progress
               u64_FlashedBytes += (ru64_CurFileSize * static_cast<uint64>(ou16_Progress100)) / 100ULL;
               //Calculate overall progress
               u16_ActualProgress = static_cast<uint16>((u64_FlashedBytes * 100ULL) / this->mu64_OverallFilesSize);
               //Stop iteration
               q_Stop = true;
            }
            else
            {
               u64_FlashedBytes += ru64_CurFileSize;
            }
            //Update counter
            ++u32_Counter;
         }
      }
      //Convert to overall progress
      m_UpdateWinProgress(true, u16_ActualProgress);
      this->mpc_Ui->pc_WidgetProgress->SetProgress100(u16_ActualProgress, oq_Finished);
      //Update data transfer
      m_UpdateDataTransfer(u64_FlashedBytes, true);
      //Handle bytes per second info (only update if change)
      if ((static_cast<uint64>(this->mc_ElapsedTimer.elapsed()) != this->mu64_FlashedBytesTimestampPrev) &&
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
      uint64 u64_AverageDataRate;
      m_UpdateWinProgress(false, ou16_Progress100);
      this->mpc_Ui->pc_WidgetProgress->SetProgress100(ou16_Progress100, oq_Finished);
      //Update data transfer
      m_UpdateDataTransfer(this->mu64_OverallFilesSize, true);
      //Calculate data rate base on current elapsed time and flashed byte count
      if (this->mc_ElapsedTimer.elapsed() > 0)
      {
         u64_AverageDataRate = (this->mu64_CurrentFlashedBytes * 1000ULL) /
                               static_cast<uint64>(this->mc_ElapsedTimer.elapsed());
      }
      else
      {
         u64_AverageDataRate = 0ULL;
      }
      //Remember data rate
      if (u64_AverageDataRate != 0ULL)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            const uint32 u32_Checksum = pc_View->CalcSetupHash();
            C_UsHandler::h_GetInstance()->AddProjSvUpdateDataRate(pc_View->GetName(), u32_Checksum,
                                                                  u64_AverageDataRate);
            //Remember for next update steps
            this->mu64_LastKnownDataRateS = u64_AverageDataRate;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update signal to assign the progress data to the correct file

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SignalFileFinished(void)
{
   ++this->mu32_ItFile;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start timer

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::StartElapsedTimer(void)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Stop timer

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::StopElapsedTimer(void)
{
   this->mc_Timer.stop();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set view index

   \param[in] ou32_Value New view index

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::SetViewIndex(const uint32 ou32_Value)
{
   this->mu32_ViewIndex = ou32_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: update label layout

   \param[in,out] opc_Event Event identification and information

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::showEvent(QShowEvent * const opc_Event)
{
   QWidget::showEvent(opc_Event);
   m_UpdateLabelLayout();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: update label layout

   \param[in,out] opc_Event Event identification and information

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   m_UpdateLabelLayout();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update timing related labels

   \created     22.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateTime(void) const
{
   //Current time
   const uint64 u64_ElapsedMs = this->mc_ElapsedTimer.elapsed();
   const uint64 u64_ElapsedS = (u64_ElapsedMs / 1000ULL) % 60ULL;
   const uint64 u64_ElapsedMin = (u64_ElapsedMs / 60000ULL) % 60ULL;
   const uint64 u64_ElapsedH = u64_ElapsedMs / 3600000ULL;

   this->mpc_Ui->pc_LabelElapsed->setText(QString("%1:%2:%3").arg(u64_ElapsedH, 2, 10, QChar('0')).arg(u64_ElapsedMin,
                                                                                                       2, 10, QChar(
                                                                                                          '0')).arg(
                                             u64_ElapsedS, 2, 10, QChar('0')));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update transfer data count

   \param[in] oru64_OverallFlashedBytes Overall number of flashed bytes
   \param[in] oq_ShowDataRate           Flag to request show of data rate

   \return
   Current average data rate

   \created     23.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateDataTransfer(const uint64 & oru64_OverallFlashedBytes, const bool oq_ShowDataRate)
{
   QString c_Value;
   QString c_Progress;
   QString c_Complete;
   QString c_Unit;
   const float64 f64_OverallFlashedBytes = static_cast<float64>(oru64_OverallFlashedBytes);
   const float64 f64_OverallFilesSize = static_cast<float64>(this->mu64_OverallFilesSize);

   this->mu64_CurrentFlashedBytes = oru64_OverallFlashedBytes;
   this->mq_ShowDataRateBytesPerSecond = oq_ShowDataRate;

   if (this->mu64_OverallFilesSize > (1024ULL * 1024ULL * 1024ULL))
   {
      //GB
      c_Unit = "GB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (1024.0 * 1024.0 * 1024.0), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (1024.0 * 1024.0 * 1024.0), 'f', 1);
   }
   else if (this->mu64_OverallFilesSize > (1024ULL * 1024ULL))
   {
      //MB
      c_Unit = "MB";
      c_Progress = QString::number(f64_OverallFlashedBytes / (1024.0 * 1024.0), 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / (1024.0 * 1024.0), 'f', 1);
   }
   else if (this->mu64_OverallFilesSize > 1024ULL)
   {
      //KB
      c_Unit = "KB";
      c_Progress = QString::number(f64_OverallFlashedBytes / 1024.0, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize / static_cast<float32>(1024ULL), 'f', 1);
   }
   else
   {
      //Bytes
      c_Unit = "Bytes";
      c_Progress = QString::number(f64_OverallFlashedBytes, 'f', 1);
      c_Complete = QString::number(f64_OverallFilesSize, 'f', 1);
   }
   //Translation: 1:Flashed bytes, 2:File size, 3: File size unit
   c_Value = QString("%1 %3/%2 %3").arg(c_Progress).arg(c_Complete).arg(c_Unit);

   //Translation: 1:File progress
   this->mc_DataRateAbsoluteBytes = QString(C_GtGetText::h_GetText("Data transfer %1")).arg(c_Value);

   m_UpdateLabelLayout();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update labels

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateLabelLayout(void) const
{
   QFont c_Font("Segoe UI");
   QString c_DataRateCombined;

   c_Font.setPixelSize(16);

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
      this->mpc_Ui->pc_LabelEstimated->setVisible(false);
      this->mpc_Ui->pc_LabelDataTransfer->setText(c_DataRateCombined);
   }
   else
   {
      const QString c_AllCombinedString = QString("%1     %2").arg(this->mc_EstimatedTime).arg(c_DataRateCombined);
      QFontMetrics c_FontMetrics(c_Font);

      //Check combined length
      if (c_FontMetrics.width(c_AllCombinedString) <= this->width())
      {
         this->mpc_Ui->pc_LabelEstimated->setVisible(false);
         this->mpc_Ui->pc_LabelDataTransfer->setText(c_AllCombinedString);
      }
      else
      {
         this->mpc_Ui->pc_LabelEstimated->setVisible(true);
         this->mpc_Ui->pc_LabelEstimated->setText(this->mc_EstimatedTime);
         this->mpc_Ui->pc_LabelDataTransfer->setText(c_DataRateCombined);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update estimated wait time calculation

   \param[in] ou64_FlashedBytes Number of already flashed bytes

   \created     27.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateEstimatedWaitTime(void)
{
   if (this->mu64_OverallFilesSize > 0ULL)
   {
      if (this->mu64_LastKnownDataRateS > 0ULL)
      {
         const uint64 u64_WaitingTimeS = this->mu64_OverallFilesSize / this->mu64_LastKnownDataRateS;
         const uint64 u64_WaitingTimeMin = u64_WaitingTimeS / 60ULL;

         if (u64_WaitingTimeMin == 0ULL)
         {
            this->mc_EstimatedTime = QString(C_GtGetText::h_GetText(
                                                "Estimated waiting time less than one min."));
         }
         else
         {
            //One min for rounding errors has to be added before
            const uint64 u64_WaitingTimeH = (u64_WaitingTimeMin + 1ULL) / 60ULL;
            if (u64_WaitingTimeH == 0ULL)
            {
               this->mc_EstimatedTime = QString(C_GtGetText::h_GetText("Estimated waiting time %1 min.")).arg(
                  u64_WaitingTimeMin + 1ULL);
            }
            else
            {
               this->mc_EstimatedTime = QString(C_GtGetText::h_GetText("Estimated waiting time %1 h %2 min.")).arg(
                  u64_WaitingTimeH).arg((u64_WaitingTimeMin + 1ULL) % 60ULL);
            }
         }
      }
      else
      {
         this->mc_EstimatedTime = "";
      }
   }
   else
   {
      this->mc_EstimatedTime = QString(C_GtGetText::h_GetText("Estimated waiting time --."));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update data rate history

   \created     20.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateDataRate(void)
{
   if (this->mu64_FlashedBytesHistoryPrev > 0ULL)
   {
      QString c_DataTransferRate;
      QString c_Unit;
      const uint64 u64_Timespan = this->mu64_FlashedBytesTimestampPrev - this->mu64_FlashedBytesTimestampPrevPrev;
      const float64 f64_Timespan = static_cast<float64>(u64_Timespan);
      const uint64 u64_NumberOfFlashedBytesInTimeSpan = this->mu64_FlashedBytesHistoryPrev -
                                                        this->mu64_FlashedBytesHistoryPrevPrev;
      const float64 f64_NumberOfFlashedBytesInTimeSpan = static_cast<float64>(u64_NumberOfFlashedBytesInTimeSpan);
      const float64 f64_NumberOfFlashedBytesThisSecond = f64_NumberOfFlashedBytesInTimeSpan / (f64_Timespan * 0.001);
      const uint64 u64_NumberOfFlashedBytesThisSecond = static_cast<uint64>(f64_NumberOfFlashedBytesThisSecond);

      //Remember last value

      //Update data rate
      if (u64_NumberOfFlashedBytesThisSecond > (1024ULL * 1024ULL * 1024ULL))
      {
         //GB
         c_Unit = "GB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / (1024.0 * 1024.0 * 1024.0), 'f', 0);
      }
      if (u64_NumberOfFlashedBytesThisSecond > (1024ULL * 1024ULL))
      {
         //MB
         c_Unit = "MB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / (1024.0 * 1024.0), 'f', 0);
      }
      else if (u64_NumberOfFlashedBytesThisSecond > 1024ULL)
      {
         //KB
         c_Unit = "KB";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond / 1024.0, 'f', 0);
      }
      else
      {
         //Bytes
         c_Unit = "Bytes";
         c_DataTransferRate = QString::number(f64_NumberOfFlashedBytesThisSecond, 'f', 0);
      }
      this->mc_DataRateBytesPerSecond =
         QString(C_GtGetText::h_GetText("%1 %2/sec")).arg(c_DataTransferRate).arg(c_Unit);

      m_UpdateLabelLayout();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set task bar progress

   \param[in] oq_Visible Progress is visible flag
   \param[in] os32_Value Progress value (0-100)

   \created     15.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvUpSummaryWidget::m_UpdateWinProgress(const bool oq_Visible, const sint32 os32_Value)
{
   this->mpc_Progress->setVisible(oq_Visible);
   this->mpc_Progress->setMinimum(0);
   this->mpc_Progress->setMaximum(100);
   this->mpc_Progress->setValue(os32_Value);
}

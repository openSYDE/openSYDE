//-----------------------------------------------------------------------------
/*!
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
#ifndef C_SYVUPSUMMARYWIDGET_H
#define C_SYVUPSUMMARYWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>
#include <QWinTaskbarProgress>
#include "stwtypes.h"
#include "C_OSCSuSequences.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvUpSummaryWidget;
}

//class QElapsedTimer;

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpSummaryWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpSummaryWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpSummaryWidget();

   void Reset(void);
   void SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure = false,
                   const bool oq_Success = false);
   void InitUpdatePackage(const std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_Flashpackage,
                          const std::vector<stw_types::uint32> & orc_Order, const std::vector<bool> & orc_IsFileBased);

   void SetProgressFile100(const stw_types::uint16 ou16_Progress100, const bool oq_Finished = false);
   void SignalFileFinished(void);
   void StartElapsedTimer(void);
   void StopElapsedTimer(void);

   void SetViewIndex(const stw_types::uint32 ou32_Value);

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvUpSummaryWidget(const C_SyvUpSummaryWidget &);
   C_SyvUpSummaryWidget & operator =(const C_SyvUpSummaryWidget &);

   void m_UpdateTime(void) const;
   void m_UpdateDataTransfer(const stw_types::uint64 & oru64_OverallFlashedBytes, const bool oq_ShowDataRate);
   void m_UpdateLabelLayout(void) const;
   void m_UpdateEstimatedWaitTime(void);
   void m_UpdateDataRate(void);
   void m_UpdateWinProgress(const bool oq_Visible, const stw_types::sint32 os32_Value);

   Ui::C_SyvUpSummaryWidget * mpc_Ui;
   QTimer mc_Timer;
   QElapsedTimer mc_ElapsedTimer;
   QWinTaskbarProgress * mpc_Progress;
   std::vector<std::vector<stw_types::uint64> > mc_FileSizesByte;
   stw_types::uint32 mu32_ItFile;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint64 mu64_OverallFilesSize;
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

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

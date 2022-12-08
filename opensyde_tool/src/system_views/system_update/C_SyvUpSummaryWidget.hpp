//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the progress and the state of the update process

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPSUMMARYWIDGET_H
#define C_SYVUPSUMMARYWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>
#include <QWinTaskbarProgress>
#include "stwtypes.hpp"
#include "C_OscSuSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpSummaryWidget;
}

//class QElapsedTimer;

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpSummaryWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpSummaryWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpSummaryWidget() override;

   void SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure = false,
                   const bool oq_Success = false) const;
   void SetProgress(const uint16_t ou16_Progress100, const bool oq_Finished = false) const;
   void SetElapsedTime(const QString & orc_Time) const;
   void SetDataTransfer(const QString & orc_DataTransfer) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideBigSummaryWidget(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvUpSummaryWidget(const C_SyvUpSummaryWidget &);
   C_SyvUpSummaryWidget & operator =(const C_SyvUpSummaryWidget &) &;

   Ui::C_SyvUpSummaryWidget * mpc_Ui;

   static const int32_t mhs32_LAYOUT_THRESHOLD_1;
   static const int32_t mhs32_LAYOUT_THRESHOLD_2;
   static const int32_t mhs32_LAYOUT_THRESHOLD_3;
   static const int32_t mhs32_LAYOUT_THRESHOLD_4;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

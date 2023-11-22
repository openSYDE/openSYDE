//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Small summary widget to show below update package.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPSUMMARYWIDGETSMALL_HPP
#define C_SYVUPSUMMARYWIDGETSMALL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpSummaryWidgetSmall;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpSummaryWidgetSmall :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpSummaryWidgetSmall(QWidget * const opc_Parent = NULL);
   ~C_SyvUpSummaryWidgetSmall(void) override;

   void SetHeading(const QString & orc_Icon, const QString & orc_Text, const bool oq_Failure = false,
                   const bool oq_Success = false) const;
   void SetProgress(const uint16_t ou16_Progress100, const bool oq_Finished = false) const;
   void SetElapsedTime(const QString & orc_Time) const;
   void SetDataTransfer(const QString & orc_DataTransfer) const;
   void SetEstimatedTime(const QString & orc_DataTransfer) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideSmallSummaryWidget(void) const;

private:
   Ui::C_SyvUpSummaryWidgetSmall * mpc_Ui;

   //Avoid call
   C_SyvUpSummaryWidgetSmall(const C_SyvUpSummaryWidgetSmall &);
   C_SyvUpSummaryWidgetSmall & operator =(const C_SyvUpSummaryWidgetSmall &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

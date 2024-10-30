//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log jobs list
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSWIDGET_HPP
#define C_SDNDEDALLOGJOBSWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void SetNode(const uint32_t ou32_NodeIndex);

private:
   Ui::C_SdNdeDalLogJobsWidget * mpc_Ui;

   //Avoid call
   C_SdNdeDalLogJobsWidget(const C_SdNdeDalLogJobsWidget &);
   C_SdNdeDalLogJobsWidget & operator =(const C_SdNdeDalLogJobsWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

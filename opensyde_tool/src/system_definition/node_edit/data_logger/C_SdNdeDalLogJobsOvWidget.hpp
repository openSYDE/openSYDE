//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       LogJobs Overview Widget
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSOVWIDGET_H
#define C_SDNDEDALLOGJOBSOVWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobsOvWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsOvWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobsOvWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsOvWidget(void) override;
   void UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                uint32_t> > & orc_AllLoggerJobElementsLocationCount);
   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void SetNodeIndex(const uint32_t ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigLoadSelectedLogJob(uint32_t ou32_LogJobIndex);

private:
   Ui::C_SdNdeDalLogJobsOvWidget * mpc_Ui;

   void m_InitStaticNames(void) const;
   void m_UpdateUi(void);

   //Avoid call
   C_SdNdeDalLogJobsOvWidget(const C_SdNdeDalLogJobsOvWidget &);
   C_SdNdeDalLogJobsOvWidget & operator =(const C_SdNdeDalLogJobsOvWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

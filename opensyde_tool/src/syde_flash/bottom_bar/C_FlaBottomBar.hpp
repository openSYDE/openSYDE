//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Present Update Bottom Bar
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLABOTTOMBAR_H
#define C_FLABOTTOMBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaBottomBar;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaBottomBar :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaBottomBar(QWidget * const opc_Parent = NULL);
   ~C_FlaBottomBar(void) override;

   void SetHeadingText(const QString & orc_Text, const uint8_t & oru8_ProgressState) const;
   void SetHeadingIcon(const uint8_t & oru8_ProgressState);

   void SetProgress(const uint16_t ou16_Progress100, const bool oq_Finished) const;
   void SetProgressBarColor(const bool & orq_Success) const;
   void SetElapsedTime(const QString & orc_Time) const;
   void SetDataTransfer(const QString & orc_DataTransfer) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideSmallSummaryWidget(void) const;

private:
   void m_StyleHeadingText(const uint8_t & oru8_ProgressState) const;
   void m_SetSizePolicy(void);

   Ui::C_FlaBottomBar * mpc_Ui;

   //Avoid call
   C_FlaBottomBar(const C_FlaBottomBar &);
   C_FlaBottomBar & operator =(const C_FlaBottomBar &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

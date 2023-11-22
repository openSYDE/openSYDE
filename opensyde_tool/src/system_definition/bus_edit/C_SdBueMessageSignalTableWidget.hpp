//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing messages or signals in a table

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGESIGNALTABLEWIDGET_HPP
#define C_SDBUEMESSAGESIGNALTABLEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMessageSignalTableWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageSignalTableWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageSignalTableWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageSignalTableWidget(void) override;

   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const;
   void UpdateData(void);
   void InitStaticNames(void) const;
   void LoadUserSettings(const std::vector<int32_t> & orc_MessageValues,
                         const std::vector<int32_t> & orc_SignalValues) const;
   void SaveUserSettings(std::vector<int32_t> & orc_MessageValues, std::vector<int32_t> & orc_SignalValues) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SigSignalSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                          const uint32_t & oru32_SignalIndex);

private:
   //Avoid call
   C_SdBueMessageSignalTableWidget(const C_SdBueMessageSignalTableWidget &);
   C_SdBueMessageSignalTableWidget & operator =(const C_SdBueMessageSignalTableWidget &) &;
   void m_OnMessagesToggle(const bool & orq_Checked);
   void m_OnSignalsToggle(const bool & orq_Checked);
   void m_OnMessageSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_OnSignalSelected(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t & oru32_SignalIndex);

   void m_UpdateUi(void);

   Ui::C_SdBueMessageSignalTableWidget * mpc_Ui;
   bool mq_Messages;
};
}
}

#endif // C_SDBUEMESSAGESIGNALTABLEWIDGET_HPP

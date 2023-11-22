//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver timeout configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXTIMEOUTCONFIG_HPP
#define C_SDBUEMESSAGERXTIMEOUTCONFIG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_PuiSdNodeCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxTimeoutConfig;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageRxTimeoutConfig :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxTimeoutConfig(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_ReceiveTimeoutMode, const bool oq_TxMethodOnEvent, const bool oq_DisablePossible, const uint32_t ou32_ReceiveTimeoutValue, const uint32_t ou32_LastKnownCycleTimeValue, const uint32_t ou32_AutoReceiveTimeoutValue,
                                          const QString & orc_NameForTitle);
   ~C_SdBueMessageRxTimeoutConfig(void) override;

   void InitStaticNames(void) const;

   stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode GetReceiveTimeoutMode(void) const;
   uint32_t GetReceiveTimeoutValue(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdBueMessageRxTimeoutConfig * mpc_Ui;

   stw::opensyde_gui_elements::C_OgePopUpDialog * const mpc_ParentDialog;

   const uint32_t mu32_AutoReceiveTimeoutValue;

   static const int32_t mhs32_INDEX_AUTO;
   static const int32_t mhs32_INDEX_CUSTOM;
   static const int32_t mhs32_INDEX_DISABLED;

   //Avoid call
   C_SdBueMessageRxTimeoutConfig(const C_SdBueMessageRxTimeoutConfig &);
   C_SdBueMessageRxTimeoutConfig & operator =(const C_SdBueMessageRxTimeoutConfig &) &;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_HandleInactiveStates(void) const;
   void m_OnReceiveTimeoutModeChanged(void) const;
   void m_UpdateAutoReceiveTimeoutValue(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

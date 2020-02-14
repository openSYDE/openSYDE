//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver timeout configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXTIMEOUTCONFIG_H
#define C_SDBUEMESSAGERXTIMEOUTCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_PuiSdNodeCanMessage.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxTimeoutConfig;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageRxTimeoutConfig :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxTimeoutConfig(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_ReceiveTimeoutMode, const bool oq_TxMethodOnEvent, const stw_types::uint32 ou32_ReceiveTimeoutValue, const stw_types::uint32 ou32_LastKnownCycleTimeValue, const stw_types::uint32 ou32_AutoReceiveTimeoutValue,
                                          const QString & orc_NameForTitle);
   ~C_SdBueMessageRxTimeoutConfig(void);

   void InitStaticNames(void) const;

   stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode GetReceiveTimeoutMode(void) const;
   stw_types::uint32 GetReceiveTimeoutValue(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdBueMessageRxTimeoutConfig * mpc_Ui;

   stw_opensyde_gui_elements::C_OgePopUpDialog * const mpc_ParentDialog;

   const stw_types::uint32 mu32_AutoReceiveTimeoutValue;

   static const stw_types::sint32 mhs32_IndexAuto;
   static const stw_types::sint32 mhs32_IndexCustom;
   static const stw_types::sint32 mhs32_IndexDisabled;

   //Avoid call
   C_SdBueMessageRxTimeoutConfig(const C_SdBueMessageRxTimeoutConfig &);
   C_SdBueMessageRxTimeoutConfig & operator =(const C_SdBueMessageRxTimeoutConfig &);

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_HandleInactiveStates(void) const;
   void m_OnReceiveTimeoutModeChanged(void) const;
   void m_UpdateAutoReceiveTimeoutValue(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

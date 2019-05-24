//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for C_SyvComDriverDiag connect steps
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERDIAGCONNECT_H
#define C_SYVCOMDRIVERDIAGCONNECT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QThread>

#include "stwtypes.h"
#include "C_SyvComDriverDiag.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverDiagConnect :
   public QThread
{
public:
   enum E_ConnectState
   {
      eCDCS_WAITING,
      eCDCS_WAITING_FINISHED,
      eCDCS_SET_DIAGNOSTIC_MODE,
      eCDCS_SET_DIAGNOSTIC_MODE_FINISHED,
      eCDCS_SET_UP_CYCLIC_TRANSMISSIONS,
      eCDCS_SET_UP_CYCLIC_TRANSMISSIONS_FINISHED,
      eCDCS_UNINITIALIZED
   };

   C_SyvComDriverDiagConnect(QObject * const opc_Parent = NULL);

   E_ConnectState GetStep(void) const;
   void SetWaitingStepParameters(const stw_types::uint32 ou32_DisconnectTime);
   void SetSetDiagnosticModeParameters(C_SyvComDriverDiag * const opc_ComDriver);
   void SetSetUpCyclicTransmissionsParameters(C_SyvComDriverDiag * const opc_ComDriver);
   void GetLastOperationResult(stw_types::sint32 & ors32_Result, QString & orc_Message,
                               QString & orc_MessageDetails) const;

protected:
   virtual void run(void) override;

private:
   stw_types::uint32 mu32_DisconnectTime;
   E_ConnectState me_ConnectState;
   C_SyvComDriverDiag * mpc_ComDriverDiag;
   stw_types::sint32 ms32_OperationResult;
   QString mc_ErrorMessage;
   QString mc_ErrorMessageDetails;

   void m_RunWaitingStep(void);
   void m_RunSetDiagnosticMode(void);
   void m_RunSetUpCyclicTransmissions(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

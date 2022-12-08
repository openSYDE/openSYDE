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

#include "stwtypes.hpp"
#include "C_SyvComDriverDiag.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   void SetWaitingStepParameters(const uint32_t ou32_DisconnectTime);
   void SetSetDiagnosticModeParameters(C_SyvComDriverDiag * const opc_ComDriver);
   void SetSetUpCyclicTransmissionsParameters(C_SyvComDriverDiag * const opc_ComDriver);
   void GetLastOperationResult(int32_t & ors32_Result, QString & orc_Message, QString & orc_MessageDetails) const;

protected:
   void run(void) override;

private:
   uint32_t mu32_DisconnectTime;
   E_ConnectState me_ConnectState;
   C_SyvComDriverDiag * mpc_ComDriverDiag;
   int32_t ms32_OperationResult;
   QString mc_ErrorMessage;
   QString mc_ErrorMessageDetails;

   void m_RunWaitingStep(void);
   void m_RunSetDiagnosticMode(void);
   void m_RunSetUpCyclicTransmissions(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

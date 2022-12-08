//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Structure for imported data assignment

   For every imported DBC node we need assigned node and interface index (Datapool index is selected later).
   To avoid vector size comparisons just put this data together.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_CieImportDataAssignment.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportDataAssignment::C_CieImportDataAssignment() :
   u32_OsyNodeIndex(0UL),
   u32_OsyInterfaceIndex(0UL),
   s32_DatapoolIndexForNew(0L),
   u32_NewMessageCount(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check internal consistency
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportDataAssignment::CheckInternalConsistency() const
{
   tgl_assert(this->c_ImportData.c_Core.c_OscRxMessageData.size() == this->c_ImportData.c_Ui.c_UiRxMessageData.size());
   tgl_assert(this->c_ImportData.c_Core.c_OscTxMessageData.size() == this->c_ImportData.c_Ui.c_UiTxMessageData.size());
   tgl_assert(this->c_ImportData.c_Core.c_OscRxSignalData.size() == this->c_ImportData.c_Ui.c_UiRxSignalData.size());
   tgl_assert(this->c_ImportData.c_Core.c_OscTxSignalData.size() == this->c_ImportData.c_Ui.c_UiTxSignalData.size());
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->c_ImportData.c_Core.c_OscRxMessageData.size();
        ++u32_ItMessage)
   {
      const C_OscCanMessage & rc_Message = this->c_ImportData.c_Core.c_OscRxMessageData[u32_ItMessage];
      for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
      {
         const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
         tgl_assert(rc_Signal.u32_ComDataElementIndex < this->c_ImportData.c_Core.c_OscRxSignalData.size());
      }
   }
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < this->c_ImportData.c_Core.c_OscTxMessageData.size();
        ++u32_ItMessage)
   {
      const C_OscCanMessage & rc_Message = this->c_ImportData.c_Core.c_OscTxMessageData[u32_ItMessage];
      for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
      {
         const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
         tgl_assert(rc_Signal.u32_ComDataElementIndex < this->c_ImportData.c_Core.c_OscTxSignalData.size());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to execute polled service in a separate thread

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMPOLLINGTHREADH
#define C_SYVCOMPOLLINGTHREADH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QThread>
#include "stwtypes.h"
#include "C_SyvComDataDealer.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComPollingThreadDiag :
   public QThread
{
private:
   //prevent copying ...
   C_SyvComPollingThreadDiag(const C_SyvComPollingThreadDiag &);
   C_SyvComPollingThreadDiag & operator =(const C_SyvComPollingThreadDiag &);

   enum E_Service
   {
      eDPREAD,
      eDPWRITE,
      eNVMREAD,
      eNVMWRITE,
      eNVMREADLIST,
      eNVMSAFEWRITECHANGEDVALUES,
      eNVMSAFEREAD,
      eNVMSAFEWRITECRCS,
      eNVMNOTIFYOFCHANGES,
      eNVMSAFEREADPARAMETERVALUES
   };

   void m_SetRunParams(const E_Service oe_Service, stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                       const stw_types::uint16 ou16_ElementIndex);
   void m_SetRunParams(const E_Service oe_Service, stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex);
   void m_SetRunParams(const E_Service oe_Service, stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);
   void m_SetRunParams(const E_Service oe_Service, stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);

   //service execution parameters:
   E_Service me_Service;
   // Input
   stw_opensyde_gui_logic::C_SyvComDataDealer * mpc_Dealer;
   stw_types::uint8 mu8_DataPoolIndex;
   stw_types::uint16 mu16_ListIndex;
   stw_types::uint16 mu16_ElementIndex;
   const stw_opensyde_core::C_OSCNode * mpc_ParamNodeValues;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId>  mc_ListIds;
   // Output
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_ChangedElements;
   bool mq_ApplicationAcknowledge;
   //service execution result:
   stw_types::sint32 ms32_Result;
   stw_types::uint8 mu8_NRC;
   bool mq_AcceptNextRequest;

protected:
   virtual void run(void);

public:
   C_SyvComPollingThreadDiag(void);
   ~C_SyvComPollingThreadDiag(void);

   stw_types::sint32 StartDataPoolRead(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                       const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                       const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 StartDataPoolWrite(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                        const stw_types::uint8 ou8_DataPoolIndex,
                                        const stw_types::uint16 ou16_ListIndex,
                                        const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 StartNvmRead(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                  const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                  const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 StartNvmWrite(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                   const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex,
                                   const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 StartNvmReadList(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                      const stw_types::uint8 ou8_DataPoolIndex, const stw_types::uint16 ou16_ListIndex);
   stw_types::sint32 StartNvmSafeWriteChangedValues(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                                    const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);
   stw_types::sint32 GetNvmSafeWriteChangedValuesOutput(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedElements) const;
   stw_types::sint32 StartNvmSafeReadValues(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);
   stw_types::sint32 GetNvmSafeReadValuesOutput(const stw_opensyde_core::C_OSCNode * & orpc_ParamNodeValues) const;
   stw_types::sint32 StartNvmSafeWriteCrcs(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);
   stw_types::sint32 StartNvmNotifyOfChanges(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                             const stw_types::uint8 ou8_DataPoolIndex,
                                             const stw_types::uint16 ou16_ListIndex);
   stw_types::sint32 GetNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const;
   stw_types::sint32 StartNvmSafeReadParameterValues(stw_opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                                     const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);

   stw_types::sint32 GetResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetNegativeResponseCode(stw_types::uint8 & oru8_NRC) const;
   void AcceptNextRequest(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

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
#include "stwtypes.hpp"
#include "C_SyvComDataDealer.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComPollingThreadDiag :
   public QThread
{
private:
   //prevent copying ...
   C_SyvComPollingThreadDiag(const C_SyvComPollingThreadDiag &);
   C_SyvComPollingThreadDiag & operator =(const C_SyvComPollingThreadDiag &) &;

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

   void m_SetRunParams(const E_Service oe_Service, stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                       const uint16_t ou16_ElementIndex);
   void m_SetRunParams(const E_Service oe_Service, stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex);
   void m_SetRunParams(const E_Service oe_Service, stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                       const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);
   void m_SetRunParams(const E_Service oe_Service, stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);

   //service execution parameters:
   E_Service me_Service;
   // Input
   stw::opensyde_gui_logic::C_SyvComDataDealer * mpc_Dealer;
   uint8_t mu8_DataPoolIndex;
   uint16_t mu16_ListIndex;
   uint16_t mu16_ElementIndex;
   const stw::opensyde_core::C_OscNode * mpc_ParamNodeValues;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListId>  mc_ListIds;
   // Output
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_ChangedElements;
   bool mq_ApplicationAcknowledge;
   //service execution result:
   int32_t ms32_Result;
   uint8_t mu8_Nrc;
   bool mq_AcceptNextRequest;

protected:
   void run(void) override;

public:
   C_SyvComPollingThreadDiag(void);
   ~C_SyvComPollingThreadDiag(void) override;

   int32_t StartDataPoolRead(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                             const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t StartDataPoolWrite(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                              const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t StartNvmRead(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                        const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t StartNvmWrite(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                         const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t StartNvmReadList(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                            const uint16_t ou16_ListIndex);
   int32_t StartNvmSafeWriteChangedValues(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                          const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);
   int32_t GetNvmSafeWriteChangedValuesOutput(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedElements) const;
   int32_t StartNvmSafeReadValues(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);
   int32_t GetNvmSafeReadValuesOutput(const stw::opensyde_core::C_OscNode * & orpc_ParamNodeValues) const;
   int32_t StartNvmSafeWriteCrcs(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer);
   int32_t StartNvmNotifyOfChanges(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                   const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex);
   int32_t GetNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const;
   int32_t StartNvmSafeReadParameterValues(stw::opensyde_gui_logic::C_SyvComDataDealer & orc_Dealer,
                                           const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);

   int32_t GetResults(int32_t & ors32_Result) const;
   int32_t GetNegativeResponseCode(uint8_t & oru8_Nrc) const;
   void AcceptNextRequest(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

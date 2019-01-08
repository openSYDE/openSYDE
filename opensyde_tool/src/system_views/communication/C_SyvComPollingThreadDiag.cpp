//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class to execute polled service in a separate thread

   The class provides functions for each service that is intended for polled execution in parallel to async reception.
   Only one polled service can be executed at a time.

   When the service is finished the results will be reported to the application (signal->slot).
   To register for such a notification use the base QThread's finished signal.

   Typical usage sequence:
   1 create one instance of this class
   2 call the service function that is required
   3 the class will start the thread and poll for the response
   4 the class will store the result and finish the thread
   5 continue with 2 with the next service

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.08.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SyvComPollingThreadDiag.h"

#include "TGLUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: remember parameters for service execution

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                               const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mu8_DataPoolIndex = ou8_DataPoolIndex;
   mu16_ListIndex = ou16_ListIndex;
   mu16_ElementIndex = ou16_ElementIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utlity: remember parameters for service execution

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                               const uint16 ou16_ListIndex)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mu8_DataPoolIndex = ou8_DataPoolIndex;
   mu16_ListIndex = ou16_ListIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: remember parameters for service execution

   \created     13.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer,
                                               const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mc_ListIds = orc_ListIds;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: remember parameters for service execution

   \created     06.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Thread function

   Thread execution function

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::run(void)
{
   switch (me_Service)
   {
   case eDPREAD:
      ms32_Result = mpc_Dealer->DataPoolRead(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_NRC);
      break;
   case eDPWRITE:
      ms32_Result = mpc_Dealer->DataPoolWrite(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_NRC);
      break;
   case eNVMREAD:
      ms32_Result = mpc_Dealer->NvmRead(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_NRC);
      break;
   case eNVMWRITE:
      ms32_Result = mpc_Dealer->NvmWrite(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_NRC);
      break;
   case eNVMREADLIST:
      ms32_Result = mpc_Dealer->NvmReadList(mu8_DataPoolIndex, mu16_ListIndex, &mu8_NRC);
      break;
   case eNVMSAFEWRITECHANGEDVALUES:
      // Special case: Output parameter
      this->mc_ChangedElements.clear();
      ms32_Result = mpc_Dealer->NvmSafeWriteChangedValues(this->mc_ChangedElements, &this->mc_ListIds, &mu8_NRC);
      break;
   case eNVMSAFEREAD:
      ms32_Result = mpc_Dealer->NvmSafeReadValues(this->mpc_ParamNodeValues, &mu8_NRC);
      break;
   case eNVMSAFEWRITECRCS:
      ms32_Result = mpc_Dealer->NvmSafeWriteCrcs(&mu8_NRC);
      break;
   case eNVMNOTIFYOFCHANGES:
      ms32_Result = mpc_Dealer->NvmNotifyOfChanges(mu8_DataPoolIndex, static_cast<uint8>(mu16_ListIndex),
                                                   mq_ApplicationAcknowledge, &mu8_NRC);
      break;
   case eNVMSAFEREADPARAMETERVALUES:
      ms32_Result = mpc_Dealer->NvmSafeReadParameterValues(this->mc_ListIds, &mu8_NRC);
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up class elements

   \created     18.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SyvComPollingThreadDiag::C_SyvComPollingThreadDiag(void) :
   QThread(),
   me_Service(eDPREAD),
   mpc_Dealer(NULL),
   mu8_DataPoolIndex(0U),
   mu16_ListIndex(0U),
   mu16_ElementIndex(0U),
   mpc_ParamNodeValues(NULL),
   mq_ApplicationAcknowledge(false),
   ms32_Result(C_UNKNOWN_ERR),
   mu8_NRC(0),
   mq_AcceptNextRequest(true)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     18.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SyvComPollingThreadDiag::~C_SyvComPollingThreadDiag(void)
{
   mpc_Dealer = NULL;
   //lint -e{1540}  no memory leak because of we never took ownership
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to read from
   \param[in]  ou16_ListIndex     index of list to read from
   \param[in]  ou16_ElementIndex  index of element to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartDataPoolRead(C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                                    const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eDPREAD, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to write to
   \param[in]  ou16_ListIndex     index of list to write to
   \param[in]  ou16_ElementIndex  index of element to write to

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartDataPoolWrite(C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                                     const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eDPWRITE, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to read from
   \param[in]  ou16_ListIndex     index of list to read from
   \param[in]  ou16_ElementIndex  index of element to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmRead(C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                               const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMREAD, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to write to
   \param[in]  ou16_ListIndex     index of list to write to
   \param[in]  ou16_ElementIndex  index of element to write to

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmWrite(C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                                const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMWRITE, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to read from
   \param[in]  ou16_ListIndex     index of list to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmReadList(C_SyvComDataDealer & orc_Dealer, const uint8 ou8_DataPoolIndex,
                                                   const uint16 ou16_ListIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMREADLIST, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  orc_ListIds          Lists to update CRC only

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     06.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmSafeWriteChangedValues(C_SyvComDataDealer & orc_Dealer,
                                                                 const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMSAFEWRITECHANGEDVALUES, orc_Dealer, orc_ListIds);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the output of the function NvmSafeWriteChangedValues

   Must be called after the thread was finished after calling StartNvmSafeWriteChangedValues

   \param[out]    orc_ChangedElements  All changed datapool elements

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on

   \created     07.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::GetNvmSafeWriteChangedValuesOutput(
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedElements) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      // Copy the result vector
      orc_ChangedElements = this->mc_ChangedElements;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  orc_ChangedElements  All changed datapool elements
   \param[in]  orc_CommLock         critical section to lock during service execution

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     06.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmSafeReadValues(C_SyvComDataDealer & orc_Dealer)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMSAFEREAD, orc_Dealer);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the output of the function NvmSafeReadValues

   Must be called after the thread was finished after calling NvmSafeReadValues

   \param[out]    orpc_ParamNodeValues  Pointer to node with read values

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on

   \created     24.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::GetNvmSafeReadValuesOutput(const stw_opensyde_core::C_OSCNode * & orpc_ParamNodeValues)
const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
      orpc_ParamNodeValues = NULL;
   }
   else
   {
      // Add output
      orpc_ParamNodeValues = this->mpc_ParamNodeValues;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     08.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmSafeWriteCrcs(C_SyvComDataDealer & orc_Dealer)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMSAFEWRITECRCS, orc_Dealer);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  ou8_DataPoolIndex  index of data pool to read from
   \param[in]  ou16_ListIndex     index of list to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     09.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmNotifyOfChanges(C_SyvComDataDealer & orc_Dealer,
                                                          const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMNOTIFYOFCHANGES, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the output of the function NvmNotifyOfChanges

   Must be called after the thread was finished after calling StartNvmNotifyOfChanges

   \param[out]    orq_ApplicationAcknowledge  true: positive acknowledge from server
                                              false: negative acknowledge from server

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on

   \created     07.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::GetNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      // Copy the result vector
      orq_ApplicationAcknowledge = this->mq_ApplicationAcknowledge;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  orc_ListIds        List IDs

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on

   \created     13.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::StartNvmSafeReadParameterValues(C_SyvComDataDealer & orc_Dealer,
                                                                  const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds)
{
   sint32 s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eNVMSAFEREADPARAMETERVALUES, orc_Dealer, orc_ListIds);
      this->start();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function
                                   for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
   C_UNKNOWN_ERR  no concrete state was set yet

   \created     23.08.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::GetResults(sint32 & ors32_Result) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      ors32_Result = ms32_Result;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  oru8_NRC       negative response code of executed service function
                               for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on

   \created     17.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComPollingThreadDiag::GetNegativeResponseCode(uint8 & oru8_NRC) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      oru8_NRC = mu8_NRC;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reset flag to accept next request

   \created     14.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::AcceptNextRequest(void)
{
   this->mq_AcceptNextRequest = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
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

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SyvComPollingThreadDiag.hpp"

#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: remember parameters for service execution

   \param[in]      oe_Service          Service
   \param[in,out]  orc_Dealer          Dealer
   \param[in]      ou8_DataPoolIndex   Data pool index
   \param[in]      ou16_ListIndex      List index
   \param[in]      ou16_ElementIndex   Element index
   \param[in]      opc_DashboardWidget Optional pointer to dashboard widget data element
                                          Valid pointer: read value only for this widget with this Datapool element
                                          NULL pointer:  read value for all widgets with this Datapool element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                               const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                                               stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DashboardWidget)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mu8_DataPoolIndex = ou8_DataPoolIndex;
   mu16_ListIndex = ou16_ListIndex;
   mu16_ElementIndex = ou16_ElementIndex;
   mpc_DashboardWidget = opc_DashboardWidget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: remember parameters for service execution

   \param[in]      oe_Service          Service
   \param[in,out]  orc_Dealer          Dealer
   \param[in]      ou8_DataPoolIndex   Data pool index
   \param[in]      ou16_ListIndex      List index
   \param[in]      ou16_ElementIndex   Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                               const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mu8_DataPoolIndex = ou8_DataPoolIndex;
   mu16_ListIndex = ou16_ListIndex;
   mu16_ElementIndex = ou16_ElementIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utlity: remember parameters for service execution

   \param[in]      oe_Service          Service
   \param[in,out]  orc_Dealer          Dealer
   \param[in]      ou8_DataPoolIndex   Data pool index
   \param[in]      ou16_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                               const uint16_t ou16_ListIndex)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mu8_DataPoolIndex = ou8_DataPoolIndex;
   mu16_ListIndex = ou16_ListIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: remember parameters for service execution

   \param[in]      oe_Service    Service
   \param[in,out]  orc_Dealer    Dealer
   \param[in]      orc_ListIds   List ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer,
                                               const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
   mc_ListIds = orc_ListIds;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: remember parameters for service execution

   \param[in]      oe_Service    Service
   \param[in,out]  orc_Dealer    Dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::m_SetRunParams(const C_SyvComPollingThreadDiag::E_Service oe_Service,
                                               C_SyvComDataDealer & orc_Dealer)
{
   me_Service = oe_Service;
   mpc_Dealer = &orc_Dealer;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Thread function

   Thread execution function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::run(void)
{
   switch (me_Service)
   {
   case eDPREAD:
      ms32_Result = mpc_Dealer->DataPoolReadWithWidget(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex,
                                                       mpc_DashboardWidget, &mu8_Nrc);
      break;
   case eDPWRITE:
      ms32_Result = mpc_Dealer->DataPoolWrite(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_Nrc);
      break;
   case eNVMREAD:
      ms32_Result = mpc_Dealer->NvmRead(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_Nrc);
      break;
   case eNVMWRITE:
      ms32_Result = mpc_Dealer->NvmWrite(mu8_DataPoolIndex, mu16_ListIndex, mu16_ElementIndex, &mu8_Nrc);
      break;
   case eNVMREADLIST:
      ms32_Result = mpc_Dealer->NvmReadList(mu8_DataPoolIndex, mu16_ListIndex, &mu8_Nrc);
      break;
   case eNVMSAFEWRITECHANGEDVALUES:
      // Special case: Output parameter
      this->mc_ChangedElements.clear();
      ms32_Result = mpc_Dealer->NvmSafeWriteChangedValues(this->mc_ChangedElements, &this->mc_ListIds, &mu8_Nrc);
      break;
   case eNVMSAFEREAD:
      ms32_Result = mpc_Dealer->NvmSafeReadValues(this->mpc_ParamNodeValues, &mu8_Nrc);
      break;
   case eNVMSAFEWRITECRCS:
      ms32_Result = mpc_Dealer->NvmSafeWriteCrcs(&mu8_Nrc);
      break;
   case eNVMNOTIFYOFCHANGES:
      ms32_Result = mpc_Dealer->NvmNotifyOfChanges(mu8_DataPoolIndex, static_cast<uint8_t>(mu16_ListIndex),
                                                   mq_ApplicationAcknowledge, &mu8_Nrc);
      break;
   case eNVMSAFEREADPARAMETERVALUES:
      ms32_Result = mpc_Dealer->NvmSafeReadParameterValues(this->mc_ListIds, &mu8_Nrc);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComPollingThreadDiag::C_SyvComPollingThreadDiag(void) :
   QThread(),
   me_Service(eDPREAD),
   mpc_Dealer(NULL),
   mu8_DataPoolIndex(0U),
   mu16_ListIndex(0U),
   mu16_ElementIndex(0U),
   mpc_DashboardWidget(NULL),
   mpc_ParamNodeValues(NULL),
   mq_ApplicationAcknowledge(false),
   ms32_Result(C_UNKNOWN_ERR),
   mu8_Nrc(0),
   mq_AcceptNextRequest(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of we never took ownership
C_SyvComPollingThreadDiag::~C_SyvComPollingThreadDiag(void)
{
   mpc_Dealer = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to read from
   \param[in]  ou16_ListIndex       index of list to read from
   \param[in]  ou16_ElementIndex    index of element to read from
   \param[in]  opc_DashboardWidget  Optional pointer to dashboard widget data element
                                       Valid pointer: read value only for this widget with this Datapool element
                                       NULL pointer:  read value for all widgets with this Datapool element

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartDataPoolRead(C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                                     const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                                                     stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DashboardWidget)
{
   int32_t s32_Return = C_NO_ERR;

   if ((this->isRunning() == true) || (this->mq_AcceptNextRequest == false))
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AcceptNextRequest = false;
      this->m_SetRunParams(eDPREAD, orc_Dealer, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                           opc_DashboardWidget);
      this->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to write to
   \param[in]  ou16_ListIndex       index of list to write to
   \param[in]  ou16_ElementIndex    index of element to write to

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartDataPoolWrite(C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                                      const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to read from
   \param[in]  ou16_ListIndex       index of list to read from
   \param[in]  ou16_ElementIndex    index of element to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmRead(C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                                const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to write to
   \param[in]  ou16_ListIndex       index of list to write to
   \param[in]  ou16_ElementIndex    index of element to write to

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmWrite(C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                                 const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to read from
   \param[in]  ou16_ListIndex       index of list to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmReadList(C_SyvComDataDealer & orc_Dealer, const uint8_t ou8_DataPoolIndex,
                                                    const uint16_t ou16_ListIndex)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer     data dealer to use
   \param[in]  orc_ListIds    Lists to update CRC only

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmSafeWriteChangedValues(C_SyvComDataDealer & orc_Dealer,
                                                                  const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the output of the function NvmSafeWriteChangedValues

   Must be called after the thread was finished after calling StartNvmSafeWriteChangedValues

   \param[out]  orc_ChangedElements    All changed datapool elements

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::GetNvmSafeWriteChangedValuesOutput(
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedElements) const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer  data dealer to use

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmSafeReadValues(C_SyvComDataDealer & orc_Dealer)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the output of the function NvmSafeReadValues

   Must be called after the thread was finished after calling NvmSafeReadValues

   \param[out]  orpc_ParamNodeValues   Pointer to node with read values

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::GetNvmSafeReadValuesOutput(
   const stw::opensyde_core::C_OscNode * & orpc_ParamNodeValues)
const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer  data dealer to use

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmSafeWriteCrcs(C_SyvComDataDealer & orc_Dealer)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Start executing service in thread

   \param[in]  orc_Dealer           data dealer to use
   \param[in]  ou8_DataPoolIndex    index of data pool to read from
   \param[in]  ou16_ListIndex       index of list to read from

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmNotifyOfChanges(C_SyvComDataDealer & orc_Dealer,
                                                           const uint8_t ou8_DataPoolIndex,
                                                           const uint16_t ou16_ListIndex)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the output of the function NvmNotifyOfChanges

   Must be called after the thread was finished after calling StartNvmNotifyOfChanges

   \param[out]  orq_ApplicationAcknowledge   true: positive acknowledge from server
                                             false: negative acknowledge from server

   \return
   C_NO_ERR   result returned
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::GetNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*!
   Start executing service in thread

   \param[in]  orc_Dealer         data dealer to use
   \param[in]  orc_ListIds        List IDs

   \return
   C_NO_ERR   started polling
   C_BUSY     previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::StartNvmSafeReadParameterValues(C_SyvComDataDealer & orc_Dealer,
                                                                   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds)
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result  result code of executed service function
                              for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
   C_UNKNOWN_ERR  no concrete state was set yet
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::GetResults(int32_t & ors32_Result) const
{
   int32_t s32_Return = C_NO_ERR;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  oru8_Nrc   negative response code of executed service function
                           for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComPollingThreadDiag::GetNegativeResponseCode(uint8_t & oru8_Nrc) const
{
   int32_t s32_Return = C_NO_ERR;

   if (this->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      oru8_Nrc = mu8_Nrc;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset flag to accept next request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComPollingThreadDiag::AcceptNextRequest(void)
{
   this->mq_AcceptNextRequest = true;
}

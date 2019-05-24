//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list error manager (implementation)

   Node data pool list error manager

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeDataPoolListErrorManager.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolListErrorManager::C_SdNdeDataPoolListErrorManager(void) :
   QObject(),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mq_ErrorInList(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new data pool

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListErrorManager::Init(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex)
{
   const C_OSCNode * pc_Node;

   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   if (pc_Node != NULL)
   {
      pc_Node->CheckErrorDataPool(this->mu32_DataPoolIndex, NULL, NULL, &this->mq_ErrorInList, NULL);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error state change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListErrorManager::OnErrorChange(void)
{
   const bool q_PreviousErrorState = this->mq_ErrorInList;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      pc_Node->CheckErrorDataPool(this->mu32_DataPoolIndex, NULL, NULL, &this->mq_ErrorInList, NULL);
   }
   if (q_PreviousErrorState != this->mq_ErrorInList)
   {
      Q_EMIT this->SigErrorChange(this->mq_ErrorInList);
   }
}

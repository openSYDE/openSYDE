//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list error manager (implementation)

   Node data pool list error manager

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDpListErrorManager.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SdNdeDpListErrorManager::C_SdNdeDpListErrorManager(void) :
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
void C_SdNdeDpListErrorManager::Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex)
{
   const C_OscNode * pc_Node;

   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   if (pc_Node != NULL)
   {
      pc_Node->CheckErrorDataPool(this->mu32_DataPoolIndex, NULL, NULL, &this->mq_ErrorInList, NULL, NULL);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error state change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListErrorManager::OnErrorChange(void)
{
   const bool q_PreviousErrorState = this->mq_ErrorInList;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      pc_Node->CheckErrorDataPool(this->mu32_DataPoolIndex, NULL, NULL, &this->mq_ErrorInList, NULL, NULL);
   }
   if (q_PreviousErrorState != this->mq_ErrorInList)
   {
      Q_EMIT this->SigErrorChange(this->mq_ErrorInList);
   }
}

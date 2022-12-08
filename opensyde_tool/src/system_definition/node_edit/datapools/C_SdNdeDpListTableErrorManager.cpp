//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool table error manager (implementation)

   Node data pool table error manager.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeDpListTableErrorManager.hpp"
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
C_SdNdeDpListTableErrorManager::C_SdNdeDpListTableErrorManager(void) :
   QObject(),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0),
   mq_ElementsInvalid(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init error manager

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
   \param[in] oru32_ListIndex     List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableErrorManager::Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          const uint32_t & oru32_ListIndex)
{
   const C_OscNodeDataPool * pc_DataPool;

   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);
   if (pc_DataPool != NULL)
   {
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, NULL, NULL, NULL, NULL, NULL, &this->mq_ElementsInvalid, NULL,
                                  NULL);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error state change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableErrorManager::OnErrorChange(void)
{
   const bool q_PreviousErrorState = this->mq_ElementsInvalid;
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, NULL, NULL, NULL, NULL, NULL, &this->mq_ElementsInvalid, NULL,
                                  NULL);
   }
   if (q_PreviousErrorState != this->mq_ElementsInvalid)
   {
      Q_EMIT (this->SigErrorChange());
   }
}

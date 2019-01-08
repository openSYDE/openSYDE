//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node data pool table error manager (implementation)

   Node data pool table error manager.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeDataPoolListTableErrorManager.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListTableErrorManager::C_SdNdeDataPoolListTableErrorManager(void) :
   QObject(),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0),
   mq_ElementsInvalid(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init error manager

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Data pool index
   \param[in] oru32_ListIndex     List index

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableErrorManager::Init(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                                const uint32 & oru32_ListIndex)
{
   const C_OSCNodeDataPool * pc_DataPool;

   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);
   if (pc_DataPool != NULL)
   {
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, NULL, NULL, NULL, NULL, NULL, &this->mq_ElementsInvalid, NULL,
                                  NULL);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check error state change

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableErrorManager::OnErrorChange(void)
{
   const bool q_PreviousErrorState = this->mq_ElementsInvalid;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      pc_DataPool->CheckErrorList(this->mu32_ListIndex, NULL, NULL, NULL, NULL, NULL, &this->mq_ElementsInvalid, NULL,
                                  NULL);
   }
   if (q_PreviousErrorState != this->mq_ElementsInvalid)
   {
      Q_EMIT this->SigErrorChange();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo base command (implementation)

   Message and signal undo base command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_SdBueUnoMessageBaseCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orc_MessageId             Message identification indices
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      orc_Text                  Optional command text for informational display
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageBaseCommand::C_SdBueUnoMessageBaseCommand(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageId,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mpc_MessageSyncManager(opc_MessageSyncManager),
   mpc_MessageTreeWidget(opc_MessageTreeWidget)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      this->mc_UniqueId.reserve(orc_MessageId.size());
      for (uint32_t u32_It = 0UL; u32_It < orc_MessageId.size(); ++u32_It)
      {
         this->mc_UniqueId.push_back(this->mpc_MessageSyncManager->GetUniqueId(orc_MessageId[u32_It]));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check messages sorted

   \return
   Flags

   \retval   True    Sorted
   \retval   False   Not sorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueUnoMessageBaseCommand::m_CheckMessagesSortedAscending() const
{
   bool q_Sorted = true;

   tgl_assert(this->mpc_MessageSyncManager != NULL);
   if (this->mpc_MessageSyncManager != NULL)
   {
      C_OscCanMessageIdentificationIndices c_PrevId;
      bool q_PrevIdValid = false;
      for (uint32_t u32_It = 0UL; u32_It < this->mc_UniqueId.size(); ++u32_It)
      {
         const C_OscCanMessageIdentificationIndices c_Id = this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
            this->mc_UniqueId[u32_It]);
         if (q_PrevIdValid)
         {
            if ((((c_Id.u32_NodeIndex == c_PrevId.u32_NodeIndex) && (c_Id.e_ComProtocol == c_PrevId.e_ComProtocol)) &&
                 (c_Id.u32_DatapoolIndex == c_PrevId.u32_DatapoolIndex)) &&
                (c_Id.u32_InterfaceIndex == c_PrevId.u32_InterfaceIndex))
            {
               if (c_Id.u32_MessageIndex >= c_PrevId.u32_MessageIndex)
               {
                  //pass
               }
               else
               {
                  q_Sorted = false;
                  break;
               }
            }
         }
         //prep next iteration
         c_PrevId = c_Id;
         q_PrevIdValid = true;
      }
   }
   return q_Sorted;
}

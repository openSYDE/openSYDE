//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Storage container for all CAN message types UI part (implementation)

   Storage container for all CAN message types UI part

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdNodeCanMessageContainer.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdNodeCanMessageContainer::C_PuiSdNodeCanMessageContainer(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdNodeCanMessageContainer::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItMessage = 0; u32_ItMessage < this->c_TxMessages.size(); ++u32_ItMessage)
   {
      const C_PuiSdNodeCanMessage & rc_Message = this->c_TxMessages[u32_ItMessage];
      rc_Message.CalcHash(oru32_HashValue);
   }
   for (stw_types::uint32 u32_ItMessage = 0; u32_ItMessage < this->c_RxMessages.size(); ++u32_ItMessage)
   {
      const C_PuiSdNodeCanMessage & rc_Message = this->c_RxMessages[u32_ItMessage];
      rc_Message.CalcHash(oru32_HashValue);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request reference to either list

   \param[in] orq_IsTx Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const std::vector<C_PuiSdNodeCanMessage> & C_PuiSdNodeCanMessageContainer::GetMessagesConst(const bool & orq_IsTx) const
{
   if (orq_IsTx == true)
   {
      return this->c_TxMessages;
   }
   else
   {
      return this->c_RxMessages;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request reference to either list

   \param[in] orq_IsTx Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_PuiSdNodeCanMessage> & C_PuiSdNodeCanMessageContainer::GetMessages(const bool & orq_IsTx)
{
   if (orq_IsTx == true)
   {
      return this->c_TxMessages;
   }
   else
   {
      return this->c_RxMessages;
   }
}

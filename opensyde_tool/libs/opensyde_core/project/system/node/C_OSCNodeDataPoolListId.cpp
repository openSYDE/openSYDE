//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Datapool list ID (implementation)

   Datapool list ID

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCNodeDataPoolListId.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     17.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolListId::C_OSCNodeDataPoolListId(void) :
   u32_NodeIndex(0U),
   u32_DataPoolIndex(0),
   u32_ListIndex(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index

   \created     17.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolListId::C_OSCNodeDataPoolListId(const stw_types::uint32 ou32_NodeIndex,
                                                 const stw_types::uint32 ou32_DataPoolIndex,
                                                 const stw_types::uint32 ou32_ListIndex) :
   u32_NodeIndex(ou32_NodeIndex),
   u32_DataPoolIndex(ou32_DataPoolIndex),
   u32_ListIndex(ou32_ListIndex)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolListId::operator <(const C_OSCNodeDataPoolListId & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_NodeIndex > orc_Cmp.u32_NodeIndex)
   {
      q_Return = false;
   }
   else if (this->u32_NodeIndex == orc_Cmp.u32_NodeIndex)
   {
      if (this->u32_DataPoolIndex > orc_Cmp.u32_DataPoolIndex)
      {
         q_Return = false;
      }
      else if (this->u32_DataPoolIndex == orc_Cmp.u32_DataPoolIndex)
      {
         if (this->u32_ListIndex >= orc_Cmp.u32_ListIndex)
         {
            q_Return = false;
         }
         else
         {
            q_Return = true;
         }
      }
      else
      {
         q_Return = true;
      }
   }
   else
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else

   \created     20.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolListId::operator ==(const C_OSCNodeDataPoolListId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u32_DataPoolIndex == orc_Cmp.u32_DataPoolIndex) &&
       (this->u32_ListIndex == orc_Cmp.u32_ListIndex))
   {
      q_Return = true;
   }

   return q_Return;
}

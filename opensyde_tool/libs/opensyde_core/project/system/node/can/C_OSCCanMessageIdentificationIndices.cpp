//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class to store all necessary indices to identify a message stored inside a node (implementation)

   Utility class to store all necessary indices to identify a message stored inside a node

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   \created     22.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices::C_OSCCanMessageIdentificationIndices(void) :
   u32_NodeIndex(0),
   e_ComProtocol(C_OSCCanProtocol::eLAYER2),
   u32_InterfaceIndex(0),
   q_MessageIsTx(false),
   u32_MessageIndex(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor to initialize all members at once

   \param[in] ou32_NodeIndex      Node index
   \param[in] oe_ComProtocol      Com protocol
   \param[in] ou32_InterfaceIndex Interface index
   \param[in] oq_MessageIsTx      Flag if message is tx, false: rx assumed
   \param[in] ou32_MessageIndex   Message index

   \created     19.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCCanMessageIdentificationIndices::C_OSCCanMessageIdentificationIndices(const uint32 ou32_NodeIndex,
                                                                           const C_OSCCanProtocol::E_Type oe_ComProtocol, const uint32 ou32_InterfaceIndex, const bool oq_MessageIsTx,
                                                                           const uint32 ou32_MessageIndex) :
   u32_NodeIndex(ou32_NodeIndex),
   e_ComProtocol(oe_ComProtocol),
   u32_InterfaceIndex(ou32_InterfaceIndex),
   q_MessageIsTx(oq_MessageIsTx),
   u32_MessageIndex(ou32_MessageIndex)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Compare two indices

   \param[in] orc_Cmp Message identification indices to compare with

   \return
   Current equal to orc_Cmp
   Else false

   \created     19.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCCanMessageIdentificationIndices::operator ==(const C_OSCCanMessageIdentificationIndices & orc_Cmp) const
{
   return (((((orc_Cmp.u32_NodeIndex == this->u32_NodeIndex) && (orc_Cmp.e_ComProtocol == this->e_ComProtocol)) &&
             (orc_Cmp.u32_InterfaceIndex == this->u32_InterfaceIndex)) &&
            (orc_Cmp.q_MessageIsTx == this->q_MessageIsTx)) && (orc_Cmp.u32_MessageIndex == this->u32_MessageIndex));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add readable stream output

   \param[in,out] orc_Stream Stream
   \param[in]     orc_Id     ID to add

   \return
   Stream including ID

   \created     11.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::ostream & operator <<(std::ostream & orc_Stream, const C_OSCCanMessageIdentificationIndices & orc_Id)
{
   std::string c_Tx;
   std::string c_Type;
   switch (orc_Id.e_ComProtocol)
   {
   case C_OSCCanProtocol::eLAYER2:
      c_Type = "L2";
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      c_Type = "COPS";
      break;
   case C_OSCCanProtocol::eECES:
      c_Type = "ECES";
      break;
   }
   if (orc_Id.q_MessageIsTx == true)
   {
      c_Tx = "Tx";
   }
   else
   {
      c_Tx = "Rx";
   }
   orc_Stream << "(" << orc_Id.u32_NodeIndex << "," << c_Type << "," << orc_Id.u32_InterfaceIndex << "," << c_Tx <<
      "," << orc_Id.u32_MessageIndex << ")";
   return orc_Stream;
}

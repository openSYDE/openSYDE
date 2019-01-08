//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Store bus (header)

   Store bus (See .cpp file for full description)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCSYSTEMBUSH
#define C_OSCSYSTEMBUSH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemBus
{
public:
   C_OSCSystemBus(void);
   virtual ~C_OSCSystemBus(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   enum E_Type ///< type of bus
   {
      eCAN,     ///< Data element is storing an CAN element
      eETHERNET ///< Data element is storing an ethernet element
   };

   static bool h_CompareNameGreater(const C_OSCSystemBus & orc_Bus1, const C_OSCSystemBus & orc_Bus2);
   bool CheckErrorBusId(void) const;

   E_Type e_Type;                           ///< type of bus (e.g. CAN / Ethernet)
   stw_scl::C_SCLString c_Name;             ///< unique name of bus
   stw_scl::C_SCLString c_Comment;          ///< user comment
   stw_types::uint64 u64_BitRate;           ///< bitrate of bus in bits / second
   stw_types::uint8 u8_BusID;               ///< unique bus id
   stw_types::uint16 u16_RxTimeoutOffsetMs; ///< Delta time to wait for receive before timeout in ms
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif

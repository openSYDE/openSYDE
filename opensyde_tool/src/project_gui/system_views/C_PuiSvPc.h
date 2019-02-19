//-----------------------------------------------------------------------------
/*!
   \file
   \brief       System view PC item data element (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVPC_H
#define C_PUISVPC_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_PuiBsBox.h"
#include "C_PuiBsLineBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvPc :
   public C_PuiBsBox
{
public:
   enum E_CANDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   C_PuiSvPc(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool GetConnected(void) const;
   stw_types::uint32 GetBusIndex(void) const;
   const C_PuiBsLineBase & GetConnectionData(void) const;
   void SetConnectionData(const C_PuiBsLineBase & orc_Value);
   void SetConnected(const bool oq_Connected, const stw_types::uint32 ou32_BusIndex,
                     const bool & orq_ForceSimpleSet = false);
   E_CANDllType GetCANDllType(void) const;
   QString GetCANDll(void) const;
   QString GetCustomCANDllPath(void) const;
   QString GetCANDllAbsolute(void) const;
   void SetCANDllType(const E_CANDllType oe_Type);
   void SetCustomCANDllPath(const QString & orc_Path);

   //Specific setter
   void SetBox(const C_PuiBsBox & orc_Box);

   //Sync to system definition
   void OnSyncBusAdded(const stw_types::uint32 ou32_Index);
   void OnSyncBusDeleted(const stw_types::uint32 ou32_Index);
   bool CheckIndirectDisconnection(void) const;

private:
   bool mq_Connected;               ///< Flag if there is a valid bus connected to this PC
   stw_types::uint32 mu32_BusIndex; ///< Target bus index this PC connects to
   ///< Only valid if bus flag q_IsBusConnected is true
   C_PuiBsLineBase mc_ConnectionData; ///< Relevant connection UI data
   E_CANDllType me_CANDllType;        ///< Type of CAN DLL
   QString mc_CustomCANDllPath;       ///< Path to the custom CAN DLL
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

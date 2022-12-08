//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view PC item data element (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVPC_HPP
#define C_PUISVPC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_PuiBsBox.hpp"
#include "C_PuiBsLineBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvPc :
   public C_PuiBsBox
{
public:
   enum E_CanDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   C_PuiSvPc(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;

   bool GetConnected(void) const;
   uint32_t GetBusIndex(void) const;
   const C_PuiBsLineBase & GetConnectionData(void) const;
   void SetConnectionData(const C_PuiBsLineBase & orc_Value);
   void SetConnected(const bool oq_Connected, const uint32_t ou32_BusIndex, const bool & orq_ForceSimpleSet = false);
   E_CanDllType GetCanDllType(void) const;
   QString GetCanDll(void) const;
   QString GetCustomCanDllPath(void) const;
   QString GetCanDllAbsolute(void) const;
   void SetCanDllType(const E_CanDllType oe_Type);
   void SetCustomCanDllPath(const QString & orc_Path);

   //Specific setter
   void SetBox(const C_PuiBsBox & orc_Box);

   //Sync to system definition
   void OnSyncBusAdded(const uint32_t ou32_Index);
   void OnSyncBusDeleted(const uint32_t ou32_Index);
   bool CheckIndirectDisconnection(void) const;

private:
   bool mq_Connected;      ///< Flag if there is a valid bus connected to this PC
   uint32_t mu32_BusIndex; ///< Target bus index this PC connects to
   ///< Only valid if bus flag q_IsBusConnected is true
   C_PuiBsLineBase mc_ConnectionData; ///< Relevant connection UI data
   E_CanDllType me_CanDllType;        ///< Type of CAN DLL
   QString mc_CustomCanDllPath;       ///< Path to the custom CAN DLL
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

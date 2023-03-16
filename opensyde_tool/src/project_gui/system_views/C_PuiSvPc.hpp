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

   const C_PuiBsLineBase & GetConnectionData(void) const;
   void SetConnectionData(const C_PuiBsLineBase & orc_Value);
   E_CanDllType GetCanDllType(void) const;
   QString GetCanDll(void) const;
   QString GetCustomCanDllPath(void) const;
   QString GetCanDllAbsolute(void) const;
   void SetCanDllType(const E_CanDllType oe_Type);
   void SetCustomCanDllPath(const QString & orc_Path);

   //Specific setter
   void SetBox(const C_PuiBsBox & orc_Box);

private:
   C_PuiBsLineBase mc_ConnectionData; ///< Relevant connection UI data
   E_CanDllType me_CanDllType;        ///< Type of CAN DLL
   QString mc_CustomCanDllPath;       ///< Path to the custom CAN DLL
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

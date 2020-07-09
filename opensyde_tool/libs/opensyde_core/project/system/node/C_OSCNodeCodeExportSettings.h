//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for code export settings (implementation)

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODECODEEXPORTSETTINGS_H
#define C_OSCNODECODEEXPORTSETTINGS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeCodeExportSettings
{
public:
   C_OSCNodeCodeExportSettings(void);

   //lint -sem(stw_opensyde_core::C_OSCNodeCodeExportSettings::Initialize,initializer)
   void Initialize(void);

   enum E_Scaling ///< Scaling settings
   {
      eFLOAT32, ///< Generate factor and offset as float 32
      eFLOAT64, ///< Generate factor and offset as float 64
      eNONE     ///< Do not add factor, offset and scaling macros at all
   };

   E_Scaling e_ScalingSupport;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

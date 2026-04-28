//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package definition

   Node update package definition

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscSupNodeDefinition.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSupNodeDefinition::C_OscSupNodeDefinition(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinition::Initialize(void)
{
   u8_Active = 0U;
   u32_Position = 0UL;
   q_SendSecureAuthenticationEnabledState = false;
   q_SecureAuthenticationEnabled = false;
   q_SendTrafficEncryptionEnabledState = false;
   q_TrafficEncryptionEnabled = false;
   q_SendDebuggerEnabledState = false;
   q_DebuggerEnabled = false;
   u8_SignaturePresent = 0U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check update necessary

   \return
   Flags

   \retval   True    Update necessary
   \retval   False   Update not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSupNodeDefinition::CheckUpdateNecessary() const
{
   return ((this->c_ApplicationFileNames.size() > 0) ||
           (this->c_NvmFileNames.size() > 0) ||
           (this->c_PemFile != "") ||
           (this->q_SendSecureAuthenticationEnabledState) ||
           (this->q_SendTrafficEncryptionEnabledState) ||
           (this->q_SendDebuggerEnabledState));
}

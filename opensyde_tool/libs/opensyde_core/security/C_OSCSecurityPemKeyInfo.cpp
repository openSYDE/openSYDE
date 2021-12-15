//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PEM key info part

   PEM key info part

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCSecurityPemKeyInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

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
C_OSCSecurityPemKeyInfo::C_OSCSecurityPemKeyInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityPemKeyInfo::Clear()
{
   this->mc_PrivKeyTextDecoded.clear();
   this->mc_PubKeySerialNumber.clear();
   this->mc_PubKeySerialNumber.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get private key text decoded

   \return
   Priv key text decoded
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8> & C_OSCSecurityPemKeyInfo::GetPrivKeyTextDecoded() const
{
   return this->mc_PrivKeyTextDecoded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pub key text decoded

   \return
   Pub key text decoded
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8> & C_OSCSecurityPemKeyInfo::GetPubKeyTextDecoded() const
{
   return this->mc_PubKeyTextDecoded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get public key serial number

   \return
   Pub key serial number
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8> & C_OSCSecurityPemKeyInfo::GetPubKeySerialNumber() const
{
   return this->mc_PubKeySerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set private key text decoded

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityPemKeyInfo::SetPrivKeyTextDecoded(const std::vector<uint8> & orc_Value)
{
   this->mc_PrivKeyTextDecoded = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set public key text decoded

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityPemKeyInfo::SetPubKeyTextDecoded(const std::vector<uint8> & orc_Value)
{
   this->mc_PubKeyTextDecoded = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set public key serial number

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityPemKeyInfo::SetPubKeySerialNumber(const std::vector<uint8> & orc_Value)
{
   this->mc_PubKeySerialNumber = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PEM key info part

   PEM key info part

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscSecurityPemKeyInfo.hpp"

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
C_OscSecurityPemKeyInfo::C_OscSecurityPemKeyInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::Clear()
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
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetPrivKeyTextDecoded() const
{
   return this->mc_PrivKeyTextDecoded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pub key text decoded

   \return
   Pub key text decoded
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetPubKeyTextDecoded() const
{
   return this->mc_PubKeyTextDecoded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get public key serial number

   \return
   Pub key serial number
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetPubKeySerialNumber() const
{
   return this->mc_PubKeySerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check valid key

   \param[in,out]  orc_ErrorMessage       Error message
   \param[in]      oq_CheckSerialNumber   Check serial number

   \return
   Flags

   \retval   True    Is valid key
   \retval   False   Is not valid key
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSecurityPemKeyInfo::CheckValidKey(std::string & orc_ErrorMessage, const bool oq_CheckSerialNumber) const
{
   bool q_Retval = false;

   if (this->GetPubKeyTextDecoded().size() > 0UL)
   {
      if (this->GetPrivKeyTextDecoded().size() > 0UL)
      {
         if (oq_CheckSerialNumber)
         {
            if (this->GetPubKeySerialNumber().size() > 0UL)
            {
               q_Retval = true;
            }
            else
            {
               orc_ErrorMessage = "could not find serial number in public key";
            }
         }
         else
         {
            q_Retval = true;
         }
      }
      else
      {
         orc_ErrorMessage = "could not find private key";
      }
   }
   else
   {
      orc_ErrorMessage = "could not find public key";
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set private key text decoded

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetPrivKeyTextDecoded(const std::vector<uint8_t> & orc_Value)
{
   this->mc_PrivKeyTextDecoded = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set public key text decoded

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetPubKeyTextDecoded(const std::vector<uint8_t> & orc_Value)
{
   this->mc_PubKeyTextDecoded = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set public key serial number

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetPubKeySerialNumber(const std::vector<uint8_t> & orc_Value)
{
   this->mc_PubKeySerialNumber = orc_Value;
}

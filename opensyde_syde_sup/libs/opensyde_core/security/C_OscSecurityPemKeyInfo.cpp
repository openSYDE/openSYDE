//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PEM key info part

   Container class for storing key information from a .pem file:
   * private key
   * certificate information:
   ** public key
   ** "key usage" and "extended key usage" flags


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
/*! \brief  Default constructor of utility class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation::C_CertificateKeyUsageInformation()
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Preset all flags to false
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation::Clear()
{
   q_KeyUsageDefined = false;
   q_KeyUsageDigitalSignature = false;
   q_KeyUsageNonRepudiation = false;
   q_KeyUsageKeyEncipherment = false;
   q_KeyUsageDataEncipherment = false;
   q_KeyUsageKeyAgreement = false;
   q_KeyUsageKeyCertSign = false;
   q_KeyUsageCrlSign = false;
   q_KeyUsageEncipherOnly = false;
   q_KeyUsageDecipherOnly = false;

   q_ExtendedKeyUsageDefined = false;
   q_ExtendedKeyUsageServerAuth = false;
   q_ExtendedKeyUsageClientAuth = false;
   q_ExtendedKeyUsageEmailProtection = false;
   q_ExtendedKeyUsageCodeSigning = false;
   q_ExtendedKeyUsageOcspSigning = false;
   q_ExtendedKeyUsageTimeStamping = false;
   q_ExtendedKeyUsageDvcs = false;
   q_ExtendedKeyUsageAnyExtendedKeyUsage = false;
}

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
   this->mc_KeyUsageInformation.Clear();
   this->mc_PrivateKey.clear();
   this->mc_X509CertificateData.clear();
   this->mc_CertificateSerialNumber.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get private key text decoded

   Get private key value.
   Format: binary array

   \return
   Priv key text decoded
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetPrivateKey() const
{
   return this->mc_PrivateKey;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get X.509 certificate data

   Return the binary representation of the whole X.509 "Certificate" section of a pem file.
   i.e. the base64 data converted to binary form.

   \return
   Certificate data
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetX509CertificateData() const
{
   return this->mc_X509CertificateData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get certificate serial number

   Format: binary array

   \return
   Certificate serial number
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> & C_OscSecurityPemKeyInfo::GetCertificateSerialNumber() const
{
   return this->mc_CertificateSerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get key usage information

   \return
   Key usage information
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation & C_OscSecurityPemKeyInfo::GetKeyUsageInformation()
const
{
   return mc_KeyUsageInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check for presence of key information

   Check whether the instance contains non-zero length certificate and private key information.
   If information is missing the function will fill a text string with details.

   \param[out]     orc_ErrorMessage       Details about what information is missing
   \param[in]      oq_CheckSerialNumber   true: also check for non-zero length certificate serial number

   \retval   true    All requested information present
   \retval   false   At least one piece of information (certificate, private key, serial number) not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSecurityPemKeyInfo::AreKeysAvailable(std::string & orc_ErrorMessage, const bool oq_CheckSerialNumber) const
{
   bool q_Retval = false;

   if (this->GetX509CertificateData().size() > 0UL)
   {
      if (this->GetPrivateKey().size() > 0UL)
      {
         if (oq_CheckSerialNumber)
         {
            if (this->GetCertificateSerialNumber().size() > 0UL)
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

   Set private key value.
   Format: binary array

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetPrivateKey(const std::vector<uint8_t> & orc_Value)
{
   this->mc_PrivateKey = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set X.509 certificate data

   Set X.509 certificate value.
   Set the binary representation of the whole X.509 "Certificate" section of a pem file.

   Technically:
   Everything between the "BEGIN CERTIFICATE" and "END CERTIFICATE" lines converted from base64 to binary.

   This will typically include the public key, certificate information (e.g. serial number, flags).

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetX509CertificateData(const std::vector<uint8_t> & orc_Value)
{
   this->mc_X509CertificateData = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set certificate serial number

   Format: binary array

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetCertificateSerialNumber(const std::vector<uint8_t> & orc_Value)
{
   this->mc_CertificateSerialNumber = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set key usage information

   \param[in]  orc_KeyUsage   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSecurityPemKeyInfo::SetKeyUsageInformation(
   const C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation & orc_KeyUsage)
{
   this->mc_KeyUsageInformation = orc_KeyUsage;
}

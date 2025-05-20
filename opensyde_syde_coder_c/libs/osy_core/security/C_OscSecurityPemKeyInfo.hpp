//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PEM key info part
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMKEYINFO_HPP
#define C_OSCSECURITYPEMKEYINFO_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <string>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityPemKeyInfo
{
public:
   class C_CertificateKeyUsageInformation
   {
   public:
      C_CertificateKeyUsageInformation();

      void Clear();

      //see rfc5280 for meaning of values
      bool q_KeyUsageDefined; //information available in certificate ?
      //bits set in "key usage" field of certificate:
      bool q_KeyUsageDigitalSignature;
      bool q_KeyUsageNonRepudiation;
      bool q_KeyUsageKeyEncipherment;
      bool q_KeyUsageDataEncipherment;
      bool q_KeyUsageKeyAgreement;
      bool q_KeyUsageKeyCertSign;
      bool q_KeyUsageCrlSign;
      bool q_KeyUsageEncipherOnly;
      bool q_KeyUsageDecipherOnly;

      bool q_ExtendedKeyUsageDefined; //information available in certificate ?
      //bits set in "key usage" field of certificate:
      bool q_ExtendedKeyUsageServerAuth;
      bool q_ExtendedKeyUsageClientAuth;
      bool q_ExtendedKeyUsageEmailProtection;
      bool q_ExtendedKeyUsageCodeSigning;
      bool q_ExtendedKeyUsageOcspSigning;
      bool q_ExtendedKeyUsageTimeStamping;
      bool q_ExtendedKeyUsageDvcs;
      bool q_ExtendedKeyUsageAnyExtendedKeyUsage;
   };

   C_OscSecurityPemKeyInfo();

   void Clear();

   const std::vector<uint8_t> & GetPrivateKey(void) const;
   const std::vector<uint8_t> & GetX509CertificateData(void) const;
   const std::vector<uint8_t> & GetCertificateSerialNumber(void) const;
   const C_CertificateKeyUsageInformation & GetKeyUsageInformation(void) const;

   bool AreKeysAvailable(std::string & orc_ErrorMessage, const bool oq_CheckSerialNumber) const;
   void SetPrivateKey(const std::vector<uint8_t> & orc_Value);
   void SetX509CertificateData(const std::vector<uint8_t> & orc_Value);
   void SetCertificateSerialNumber(const std::vector<uint8_t> & orc_Value);
   void SetKeyUsageInformation(const C_CertificateKeyUsageInformation & orc_KeyUsage);

private:
   //information flags from certificate
   C_CertificateKeyUsageInformation mc_KeyUsageInformation;

   std::vector<uint8_t> mc_PrivateKey;

   //certificate information (includes public key, certificate data, flags, ...)
   std::vector<uint8_t> mc_X509CertificateData;

   //serial number of certificate
   std::vector<uint8_t> mc_CertificateSerialNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle PEM content
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEM_H
#define C_OSCSECURITYPEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"
#include "C_OscSecurityPemKeyInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSecurityPem
{
public:
   C_OscSecurityPem();

   void Clear();

   const C_OscSecurityPemKeyInfo & GetKeyInfo(void) const;
   const std::string & GetMetaInfos(void) const;

   int32_t LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage);

   static int32_t h_ExtractModulusAndExponentFromFile(const std::string & orc_FileName,
                                                      std::vector<uint8_t> & orc_Modulus,
                                                      std::vector<uint8_t> & orc_Exponent,
                                                      std::string & orc_ErrorMessage);
   static int32_t h_ExtractModulusAndExponent(const std::vector<uint8_t> & orc_PubKeyTextDecoded,
                                              std::vector<uint8_t> & orc_Modulus, std::vector<uint8_t> & orc_Exponent,
                                              std::string & orc_ErrorMessage);

private:
   static const uint32_t mhu32_DEFAULT_BUFFER_SIZE;

   std::string mc_MetaInfo;
   C_OscSecurityPemKeyInfo mc_KeyInfo;

   int32_t m_ReadPublicKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);
   int32_t m_ReadMetaInfos(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);
   int32_t m_ReadPrivateKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

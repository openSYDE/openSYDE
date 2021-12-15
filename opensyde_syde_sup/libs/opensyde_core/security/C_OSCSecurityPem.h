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

#include "stwtypes.h"
#include "C_OSCSecurityPemKeyInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSecurityPem
{
public:
   C_OSCSecurityPem();

   void Clear();

   const C_OSCSecurityPemKeyInfo & GetKeyInfo(void) const;
   const std::string & GetMetaInfos(void) const;

   stw_types::sint32 LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage);

   static stw_types::sint32 h_ExtractModulusAndExponentFromFile(const std::string & orc_FileName,
                                                                std::vector<stw_types::uint8> & orc_Modulus,
                                                                std::vector<stw_types::uint8> & orc_Exponent,
                                                                std::string & orc_ErrorMessage);
   static stw_types::sint32 h_ExtractModulusAndExponent(const std::vector<stw_types::uint8> & orc_PubKeyTextDecoded,
                                                        std::vector<stw_types::uint8> & orc_Modulus,
                                                        std::vector<stw_types::uint8> & orc_Exponent,
                                                        std::string & orc_ErrorMessage);

private:
   static const stw_types::uint32 mhu32_DEFAULT_BUFFER_SIZE;

   std::string mc_MetaInfo;
   C_OSCSecurityPemKeyInfo mc_KeyInfo;

   stw_types::sint32 m_ReadPublicKey(const std::vector<stw_types::uint8> & orc_FileContent,
                                     std::string & orc_ErrorMessage);
   stw_types::sint32 m_ReadMetaInfos(const std::vector<stw_types::uint8> & orc_FileContent,
                                     std::string & orc_ErrorMessage);
   stw_types::sint32 m_ReadPrivateKey(const std::vector<stw_types::uint8> & orc_FileContent,
                                      std::string & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

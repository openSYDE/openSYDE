//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for handling PEM file content
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSECURITYPEMBASE_HPP
#define C_OSCSECURITYPEMBASE_HPP

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

class C_OscSecurityPemBase
{
public:
   C_OscSecurityPemBase();
   virtual ~C_OscSecurityPemBase();

   void Clear();

   const C_OscSecurityPemKeyInfo & GetKeyInfo(void) const;
   const std::string & GetMetaInfos(void) const;

   virtual int32_t LoadFromFile(const std::string & orc_FileName, std::string & orc_ErrorMessage);

protected:
   static const uint32_t mhu32_DEFAULT_BUFFER_SIZE;

   std::string mc_MetaInfo;
   C_OscSecurityPemKeyInfo mc_KeyInfo;

   virtual int32_t m_ReadPublicKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);
   virtual int32_t m_ReadMetaInfos(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage);

   //Pure virtual function. Implementation will differ depending on whether a regular key or an elliptic curve key is
   // expected.
   virtual int32_t m_ReadPrivateKey(const std::vector<uint8_t> & orc_FileContent, std::string & orc_ErrorMessage) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

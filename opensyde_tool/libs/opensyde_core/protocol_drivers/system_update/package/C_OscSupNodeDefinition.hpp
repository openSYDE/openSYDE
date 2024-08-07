//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package definition
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUPNODEDEFINITION_HPP
#define C_OSCSUPNODEDEFINITION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSupNodeDefinition
{
public:
   C_OscSupNodeDefinition(void);

   //lint -sem(stw::opensyde_core::C_OscSupNodeDefinition::Initialize,initializer)
   void Initialize(void);

   uint8_t u8_Active;
   uint32_t u32_Position;
   std::vector<stw::scl::C_SclString> c_ApplicationFileNames; // with relative path
   std::vector<stw::scl::C_SclString> c_NvmFileNames;         // with relative path
   stw::scl::C_SclString c_PemFile;
   uint8_t u8_SignaturePresent;
   stw::scl::C_SclString c_SignatureFile;

   /// Node configuration flags for security state
   bool q_SendSecurityEnabledState;
   bool q_SecurityEnabled;

   /// Node configuration flags for debugger state
   bool q_SendDebuggerEnabledState;
   bool q_DebuggerEnabled;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

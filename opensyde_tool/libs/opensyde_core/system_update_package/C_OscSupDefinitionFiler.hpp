//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System update package definition filer
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUPDEFINITIONFILER_HPP
#define C_OSCSUPDEFINITIONFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscSupDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSupDefinitionFiler
{
public:
   static int32_t h_CreateUpdatePackageDefFile(const stw::scl::C_SclString & orc_Path,
                                               const C_OscSupDefinition & orc_SupDefContent,
                                               const std::vector<stw::scl::C_SclString> & orc_Files);
   static int32_t h_LoadUpdatePackageDefFile(const stw::scl::C_SclString & orc_TargetUnzipPath, const bool oq_IsZip,
                                             const stw::scl::C_SclString & orc_PackagePath,
                                             uint32_t & oru32_FileVersion, stw::scl::C_SclString & orc_FilePackagePath,
                                             uint32_t & oru32_ActiveBusIndex, std::vector<uint8_t> & orc_ActiveNodes,
                                             std::vector<uint32_t> & orc_UpdatePosition,
                                             std::vector<stw::scl::C_SclString> & orc_PackageFiles);

   static const stw::scl::C_SclString hc_PACKAGE_UPDATE_DEF;

private:
   static void mh_SaveNodes(C_OscXmlParserBase & orc_XmlParser, const std::vector<C_OscSupNodeDefinition> & orc_Nodes,
                            const std::vector<stw::scl::C_SclString> & orc_Files);
   static void mh_LoadNodes(C_OscXmlParserBase & orc_XmlParser, std::vector<uint8_t> & orc_ActiveNodes,
                            std::vector<uint32_t> & orc_UpdatePosition,
                            std::vector<stw::scl::C_SclString> & orc_PackageFiles);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

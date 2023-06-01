//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SUPCREATEPACKAGE_H
#define C_SUPCREATEPACKAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SydeSup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SupCreatePackage
{
public:
   C_SupCreatePackage(const bool oq_Quiet, const stw::scl::C_SclString & orc_SupFilePath,
                      const stw::scl::C_SclString & orc_OsyProjectPath, const stw::scl::C_SclString & orc_ViewName,
                      const stw::scl::C_SclString & orc_DeviceDefPath, const stw::scl::C_SclString & orc_TempDir);

   C_SydeSup::E_Result Create(void);

private:
   bool mq_Quiet;
   stw::scl::C_SclString mc_SupFilePath;
   stw::scl::C_SclString mc_OsyProjectPath;
   stw::scl::C_SclString mc_ViewName;
   stw::scl::C_SclString mc_DeviceDefPath;
   stw::scl::C_SclString mc_TempDir;
   const stw::scl::C_SclString mc_LoggingCategory = "Create Update Package";

   C_SydeSup::E_Result m_FindView(const std::vector<stw::opensyde_core::C_OscViewData> & orc_Views,
                                  stw::opensyde_core::C_OscViewData & orc_View);
   void m_GetUpdatePackage(const stw::opensyde_core::C_OscViewData & orc_View,
                           const stw::opensyde_core::C_OscSystemDefinition & orc_SystemDefinition,
                           std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite) const;
   void m_GetDataBlocksToWrite(const stw::opensyde_core::C_OscViewNodeUpdate & orc_NodeUpdate,
                               const stw::opensyde_core::C_OscNode & orc_Node,
                               stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const;
   void m_GetFileBasedFilesToWrite(const stw::opensyde_core::C_OscViewNodeUpdate & orc_NodeUpdate,
                                   const stw::opensyde_core::C_OscNode & orc_Node,
                                   stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const;
   void m_GetParamSetsToWrite(const stw::opensyde_core::C_OscViewNodeUpdate & orc_NodeUpdate,
                              const stw::opensyde_core::C_OscNode & orc_Node,
                              stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_ApplicationToWrite) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif

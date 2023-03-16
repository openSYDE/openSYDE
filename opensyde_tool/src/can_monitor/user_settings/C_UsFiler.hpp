//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (header)

   Handle save'n load for user settings (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USFILER_H
#define C_USFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_UsHandler.hpp"
#include "stwtypes.hpp"
#include "C_SclIniFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsFiler
{
public:
   static int32_t h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path,
                         const QString & orc_ActiveProject);
   static int32_t h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path, const QString & orc_ActiveProject);

private:
   C_UsFiler(void);

   static void mh_SaveRecentProjects(const C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);
   static void mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);
   static void mh_SaveProjectDependentSection(const C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
   static void mh_LoadRecentProjects(C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);
   static void mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini);
   static void mh_LoadProjectDependentSection(C_UsHandler & orc_UserSettings, stw::scl::C_SclIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
   static void mh_SaveColumns(stw::scl::C_SclIniFile & orc_Ini, const stw::scl::C_SclString & orc_SectionName,
                              const std::string & orc_IdentifierBaseName, const std::vector<int32_t> & orc_Columns);
   static void mh_LoadColumns(stw::scl::C_SclIniFile & orc_Ini, const stw::scl::C_SclString & orc_SectionName,
                              const std::string & orc_IdentifierBaseName, std::vector<int32_t> & orc_Columns);

   static const std::string mhc_TRACE_COL_WIDTH_IDENTIFIER;
   static const std::string mhc_TRACE_COL_POSITION_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_SIGNALS_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_SELECTION_IDENTIFIER;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

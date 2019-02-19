//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (header)

   Handle save'n load for user settings (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_USFILER_H
#define C_USFILER_H

/* -- Includes ------------------------------------------------------------- */

#include "C_UsHandler.h"
#include "stwtypes.h"
#include "CSCLIniFile.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UsFiler
{
public:
   static stw_types::sint32 h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path,
                                   const QString & orc_ActiveProject);
   static stw_types::sint32 h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path,
                                   const QString & orc_ActiveProject);

private:
   C_UsFiler(void);

   static void mh_SaveRecentProjects(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_SaveProjectDependentSection(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
   static void mh_LoadRecentProjects(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_LoadProjectDependentSection(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
   static void mh_SaveColumns(stw_scl::C_SCLIniFile & orc_Ini, const stw_scl::C_SCLString & orc_SectionName,
                              const std::string & orc_IdentifierBaseName,
                              const std::vector<stw_types::sint32> & orc_Columns);
   static void mh_LoadColumns(stw_scl::C_SCLIniFile & orc_Ini, const stw_scl::C_SCLString & orc_SectionName,
                              const std::string & orc_IdentifierBaseName, std::vector<stw_types::sint32> & orc_Columns);

   static const std::string mhc_TRACE_COL_WIDTH_IDENTIFIER;
   static const std::string mhc_TRACE_COL_POSITION_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_SIGNALS_IDENTIFIER;
   static const std::string mhc_MESSAGE_GENERATOR_SELECTION_IDENTIFIER;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

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

   static void mh_SaveNode(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_NodeIdBase, const QString & orc_NodeName, const C_UsNode & orc_Node);
   static void mh_SaveBus(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                          const QString & orc_BusIdBase, const QString & orc_BusName,
                          const C_UsCommunication & orc_Bus);
   static void mh_SaveDatapool(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                               const QString & orc_DatapoolIdBase, const QString & orc_DatapoolName,
                               const C_UsNodeDatapool & orc_Datapool);
   static void mh_SaveList(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_ListIdBase, const QString & orc_ListName,
                           const C_UsNodeDatapoolList & orc_List);
   static void mh_SaveView(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_ViewIdBase, const QString & orc_ViewName,
                           const C_UsSystemView & orc_View);
   static void mh_SaveDataRates(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DataRateIdBase, const C_UsSystemView & orc_View);
   static void mh_SaveDashboard(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DashboardIdBase, const QString & orc_DashboardName,
                                const C_UsSystemViewDashboard & orc_Dashboard);
   static void mh_SaveLanguages(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_SaveRecentProjects(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_SaveProjectDependentSection(const C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
   static void mh_LoadNode(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_NodeIdBase, const QString & orc_NodeName,
                           C_UsHandler & orc_UserSettings);
   static void mh_LoadBus(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                          const QString & orc_BusIdBase, const QString & orc_BusName, C_UsHandler & orc_UserSettings,
                          const bool oq_IsBus, const QString & orc_NodeName, const QString & orc_DataPoolName);
   static void mh_LoadDatapool(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                               const QString & orc_DatapoolIdBase, const QString & orc_NodeName,
                               C_UsHandler & orc_UserSettings);
   static void mh_LoadList(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_ListIdBase, const QString & orc_NodeName,
                           const QString & orc_DataPoolName, C_UsHandler & orc_UserSettings);
   static void mh_LoadView(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                           const QString & orc_ViewIdBase, const QString & orc_ViewName,
                           C_UsHandler & orc_UserSettings);
   static void mh_LoadDataRates(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DataRateIdBase, const QString & orc_ViewName,
                                C_UsHandler & orc_UserSettings);
   static void mh_LoadDashboard(stw_scl::C_SCLIniFile & orc_Ini, const QString & orc_SectionName,
                                const QString & orc_DashboardIdBase, const QString & orc_ViewName,
                                C_UsHandler & orc_UserSettings);
   static void mh_LoadLanguages(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_LoadRecentProjects(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini);
   static void mh_LoadProjectDependentSection(C_UsHandler & orc_UserSettings, stw_scl::C_SCLIniFile & orc_Ini,
                                              const QString & orc_ActiveProject);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

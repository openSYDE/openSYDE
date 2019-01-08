//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user settings data (header)

   This class handles all actions concerning user settings.
   (note: main module description should be in .cpp file)

   \implementation
   project     STW Qt user settings
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.06.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_USHANDLER_H
#define C_USHANDLER_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPoint>
#include <QSize>
#include <QMap>
#include "stwtypes.h"
#include "C_UsCommunication.h"
#include "C_UsNode.h"
#include "C_UsSystemView.h"

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UsHandler
{
public:
   static C_UsHandler * h_GetInstance(void);
   static void h_Destroy(void);

   void ChangeActiveProjectName(const QString & orc_ActiveProject);
   void LoadActiveProject(const QString & orc_ActiveProject);

   void SetDefault(void);

   //Get
   QString GetLanguage(void) const;
   QStringList GetRecentProjects(void) const;
   QPoint GetScreenPos(void) const;
   QSize GetAppSize(void) const;
   bool GetAppMaximized(void) const;
   QPoint GetSdTopologyToolboxPos(void) const;
   QSize GetSdTopologyToolboxSize(void) const;
   bool GetSdTopologyToolboxMaximized(void) const;
   stw_types::sint32 GetNaviBarSize(void) const;
   stw_types::sint32 GetNaviBarNodeSectionSize(void) const;
   stw_types::sint32 GetSdNodeEditSplitterX(void) const;
   stw_types::sint32 GetSdBusEditTreeSplitterX(void) const;
   stw_types::sint32 GetSdBusEditLayoutSplitterX(void) const;
   static stw_types::uint8 GetMaxRecentProjects(void);
   QString GetCurrentSaveAsPath(void) const;

   void GetMostRecentFolder(QString & orc_Str) const;
   void GetRecentFolders(QStringList & orc_Folders) const;
   static void h_GetLanguages(QStringList & orc_List);

   // Project specific get
   stw_types::sint32 GetProjLastMode(void) const;
   stw_types::sintn GetProjSdTopologyViewZoom(void) const;
   QPoint GetProjSdTopologyViewPos(void) const;
   QString GetProjSdTopologyLastKnownTSPPath(void) const;
   QString GetProjSdTopologyLastKnownCodeExportPath(void) const;
   QString GetProjSdTopologyLastKnownExportPath(void) const;
   QString GetProjSdTopologyLastKnownImportPath(void) const;
   QString GetProjSdTopologyLastKnownRtfPath(void) const;
   QString GetProjSdTopologyLastKnownRtfCompanyName(void) const;
   QString GetProjSdTopologyLastKnownRtfCompanyLogoPath(void) const;
   C_UsNode GetProjSdNode(const QString & orc_NodeName) const;
   C_UsCommunication GetProjSdBus(const QString & orc_BusName) const;
   C_UsSystemView GetProjSvSetupView(const QString & orc_ViewName) const;
   const QList<QString> GetProjSdNodeKeysInternal(void) const;
   const QList<QString> GetProjSdBusKeysInternal(void) const;
   const QList<QString> GetProjSvSetupViewKeysInternal(void) const;
   void GetProjSvSetupNodeInfoSizePos(const QString & orc_ViewName, QPoint & orc_Pos, QSize & orc_Size,
                                      bool & orq_Maximized) const;
   C_UsSystemViewDashboard GetProjSvDashboardSettings(const QString & orc_ViewName,
                                                      const QString & orc_DashboardName) const;
   void GetProjLastScreenMode(stw_types::sint32 & ors32_SysDefSubMode, stw_types::uint32 & oru32_SysDefIndex,
                              stw_types::uint32 & oru32_SysDefFlag, stw_types::sint32 & ors32_SysViewSubMode,
                              stw_types::uint32 & oru32_SysViewIndex, stw_types::uint32 & oru32_SysViewFlag) const;
   void GetProjLastSysDefTabIndex(stw_types::sintn & orsn_SysDefNodeEditTabIndex,
                                  stw_types::sintn & orsn_SysDefBusEditTabIndex) const;

   //Set
   stw_types::sint32 SetLanguage(const QString & orc_Lang);
   void SetRecentProjects(const QStringList & orc_New);
   void SetScreenPos(const QPoint & orc_New);
   void SetAppSize(const QSize & orc_New);
   void SetAppMaximized(const bool & orq_New);
   void SetSdTopologyToolboxPos(const QPoint & orc_New);
   void SetSdTopologyToolboxSize(const QSize & orc_New);
   void SetNaviBarSize(const stw_types::sint32 os32_Value);
   void SetNaviBarNodeSectionSize(const stw_types::sint32 os32_Value);
   void SetSdNodeEditSplitterX(const stw_types::sint32 os32_Value);
   void SetSdBusEditTreeSplitterX(const stw_types::sint32 os32_Value);
   void SetSdBusEditLayoutSplitterX(const stw_types::sint32 os32_Value);
   void SetSdTopologyToolboxMaximized(const bool & orq_New);
   void SetCurrentSaveAsPath(const QString & orc_Value);

   void AddToRecentProjects(const QString & orc_Str);
   void RemoveOfRecentProjects(const QString & orc_Str);
   void ClearRecentProjects(void);

   // Project specific set
   void SetProjLastMode(const stw_types::sint32 os32_New);
   void SetProjSdTopologyViewZoom(const stw_types::sintn osn_New);
   void SetProjSdTopologyViewPos(const QPoint & orc_New);
   void SetProjSdTopologyLastKnownTSPPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownCodeExportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownExportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownImportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfCompanyName(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfCompanyLogoPath(const QString & orc_New);
   void SetProjSdNodeSelectedDatapoolName(const QString & orc_NodeName, const QString & orc_DatapoolName);
   void SetProjSdNodeDatapoolOpenListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                           const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolSelectedListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                               const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolSelectedVariableNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                   const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolListSelectedMessage(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                 const QString & orc_ListName,
                                                 const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                                 const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                                 const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetProjSdNodeDatapoolListColumnSizes(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                             const QString & orc_ListName,
                                             const std::vector<stw_types::sint32> & orc_ColumnWidths);
   void SetProjSdBusSelectedMessage(const QString & orc_BusName,
                                    const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                    const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                    const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetProjSvNavigationExpandedStatus(const QString & orc_ViewName, const bool oq_NavigationExpandedStatus);
   void SetProjSvSetupViewZoom(const QString & orc_ViewName, const stw_types::sintn osn_New);
   void SetProjSvSetupViewPos(const QString & orc_ViewName, const QPoint & orc_New);
   void SetProjSvUpdateViewZoom(const QString & orc_ViewName, const stw_types::sintn osn_New);
   void SetProjSvUpdateViewPos(const QString & orc_ViewName, const QPoint & orc_New);
   void SetProjSvParamExport(const QString & orc_ViewName, const QString & orc_Path);
   void SetProjSvParamImport(const QString & orc_ViewName, const QString & orc_Path);
   void SetProjSvParamRecord(const QString & orc_ViewName, const QString & orc_Path, const QString & orc_FileName);
   void SetProjSvSetupNodeInfoSizePos(const QString & orc_ViewName, const QPoint & orc_Pos, const QSize & orc_Size,
                                      const bool & orq_Maximized);
   void AddProjSvUpdateDataRate(const QString & orc_ViewName, const stw_types::uint32 ou32_Checksum,
                                const stw_types::uint64 ou64_DataRateBytesPerS);
   void SetProjSvUpdateSplitterX(const QString & orc_ViewName, const stw_types::sint32 os32_Value);
   void SetProjSvUpdateProgressLog(const QString & orc_ViewName, const QPoint & orc_Position, const QSize & orc_Size,
                                   const bool & orq_Maximized);
   void SetProjSvDashboardToolbox(const QString & orc_ViewName, const QPoint & orc_Position, const QSize & orc_Size,
                                  const bool & orq_Maximized);
   void SetProjSvDashboardSelectedTabIndex(const QString & orc_ViewName, const stw_types::sint32 os32_Index);
   void SetProjSvDashboardMainTab(const QString & orc_ViewName, const QString & orc_DashboardName,
                                  const stw_types::sint32 os32_TabIndex);
   void SetProjSvDashboardTearOffPosition(const QString & orc_ViewName, const QString & orc_DashboardName,
                                          const QPoint & orc_Position, const QSize & orc_Size,
                                          const bool oq_TornOffWindowMinimized, const bool oq_TornOffWindowMaximized);
   void SetProjSvDashboardScenePositionAndZoom(const QString & orc_ViewName, const QString & orc_DashboardName,
                                               const QPoint & orc_Position, const stw_types::sintn osn_Zoom);
   void SetProjLastScreenMode(const stw_types::sint32 os32_SysDefSubMode, const stw_types::uint32 ou32_SysDefIndex,
                              const stw_types::uint32 ou32_SysDefFlag, const stw_types::sint32 os32_SysViewSubMode,
                              const stw_types::uint32 ou32_SysViewIndex, const stw_types::uint32 ou32_SysViewFlag);
   void SetProjLastSysDefTabIndex(const stw_types::sintn osn_SysDefNodeEditTabIndex,
                                  const stw_types::sintn osn_SysDefBusEditTabIndex);

   void ClearMaps(void);

   void Save(void) const;

   static stw_types::sint32 h_CheckLanguageExists(const QString & orc_Str);
   static stw_types::sint32 h_GetParentFolder(const QString & orc_CompletePath, QString & orc_Parent,
                                              const bool & orq_CompletePathContainsFile);

private:
   static C_UsHandler * mhpc_Singleton;

   //Avoid call
   C_UsHandler(void);
   virtual ~C_UsHandler();
   C_UsHandler(const C_UsHandler &);
   C_UsHandler & operator =(const C_UsHandler &);

   const QString mc_IniPathAndName;                      ///< Fix location of user settings
   QString mc_ActualProject;                             ///< Project specific settings for this concrete project
   static const stw_types::uint8 mhu8_MaxRecentProjects; ///< Maximum number of entries in unique history of recent
                                                         // projects
   static const QPoint mhc_DefaultViewPos;               ///< Default view position
   static const stw_types::sintn mhsn_DefaultZoomLevel;  ///< Default zoom level
   const QString mc_DefaultProjectsFolder;               ///< Default openSYDE projects location if no recent projects
                                                         // are available
   QString mc_Lang;                                      ///< Current language
   QString mc_CurrentSaveAsPath;                         ///< Current save as base path
   QStringList mc_RecentProjects;                        ///< Unique history of recent projects
   QPoint mc_ScreenPos;                                  ///< History of last known screen position
   QSize mc_AppSize;                                     ///< History of last known openSyde window size
   bool mq_AppMaximized;                                 ///< History of openSyde window state
   QPoint mc_SdTopologyToolboxPos;                       ///< History of last known sys def toolbox position
   QSize mc_SdTopologyToolboxSize;                       ///< History of last known sys def toolbox size
   stw_types::sint32 ms32_NaviBarSize;                   ///< Last known navi bar size
   stw_types::sint32 ms32_NaviBarNodeSectionSize;        ///< Last known navi bar node section size
   stw_types::sint32 ms32_SdNodeEditSplitterX;           ///< History of last known sys def node edit splitter position
                                                         // x value
   stw_types::sint32 ms32_SdBusEditTreeSplitterX;        ///< History of last known sys def bus edit tree splitter
                                                         // position x value
   stw_types::sint32 ms32_SdBusEditLayoutSplitterX;      ///< History of last known sys def bus edit layout splitter
                                                         // position x value
   bool mq_SdTopologyToolboxMaximized;                   ///< History of sys def toolbox state
   stw_types::sint32 ms32_ProjLastKnownMode;             ///< History of last known project mode (SD/SC/Main)
   stw_types::sintn msn_ProjSdTopologyViewZoom;          ///< History of last known zoom value for specific project
   QPoint mc_ProjSdTopologyViewPos;                      ///< History of last known viewport pos for specific project
   QString mc_ProjSdTopologyLastKnownTSPPath;            ///< History of last known TSP path
   QString mc_ProjSdTopologyLastKnownCodeExportPath;     ///< History of last known code export path
   QString mc_ProjSdTopologyLastKnownExportPath;         ///< History of last known export path
   QString mc_ProjSdTopologyLastKnownImportPath;         ///< History of last known import path
   QString mc_ProjSdTopologyLastKnownRtfPath;            ///< History of last known rtf file export path
   QString mc_ProjSdTopologyLastKnownRtfCompanyName;     ///< History of last known rtf file export company name
   QString mc_ProjSdTopologyLastKnownRtfCompanyLogoPath; ///< History of last known rtf file export company logo path
   QMap<QString, C_UsSystemView> mc_ProjSvSetupView;     ///< History of last known view user settings
   QMap<QString, C_UsNode> mc_ProjSdNode;                ///< History of last known node user settings
   QMap<QString, C_UsCommunication> mc_ProjSdBus;        ///< History of last known bus user settings
   stw_types::sint32 ms32_SysDefSubMode;                 ///< History of last known system definition sub mode
   stw_types::uint32 mu32_SysDefIndex;                   ///< History of last known system definition index
   stw_types::uint32 mu32_SysDefFlag;                    ///< History of last known system definition flag
   stw_types::sint32 ms32_SysViewSubMode;                ///< History of last known system view sub mode
   stw_types::uint32 mu32_SysViewIndex;                  ///< History of last known system view index
   stw_types::uint32 mu32_SysViewFlag;                   ///< History of last known system view flag
   stw_types::sintn msn_SysDefNodeEditTabIndex;          ///< History of last known tab index in node edit
   stw_types::sintn msn_SysDefBusEditTabIndex;           ///< History of last known tab index in bus edit
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user settings data (header)

   This class handles all actions concerning user settings.
   (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_USHANDLER_HPP
#define C_USHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPoint>
#include <QSize>
#include <QMap>
#include <QVector>
#include <QColor>
#include "stwtypes.hpp"
#include "C_UsCommunication.hpp"
#include "C_UsNode.hpp"
#include "C_UsSystemView.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
   bool GetPerformanceActive(void) const;
   QStringList GetRecentProjects(void) const;
   QPoint GetScreenPos(void) const;
   QSize GetAppSize(void) const;
   bool GetAppMaximized(void) const;
   QPoint GetSdTopologyToolboxPos(void) const;
   QSize GetSdTopologyToolboxSize(void) const;
   bool GetSdTopologyToolboxMaximized(void) const;
   int32_t GetNaviBarSize(void) const;
   int32_t GetNaviBarNodeSectionSize(void) const;
   int32_t GetSdNodeEditSplitterHorizontal(void) const;
   int32_t GetSdNodeEditHalcSplitterHorizontal(void) const;
   int32_t GetSdNodeEditCoManagerSplitterHorizontal(void) const;
   int32_t GetSdBusEditTreeSplitterHorizontal(void) const;
   int32_t GetSdBusEditTreeSplitterHorizontal2(void) const;
   int32_t GetSdBusEditLayoutSplitterHorizontal(void) const;
   static uint8_t h_GetMaxRecentProjects(void);
   QString GetCurrentSaveAsPath(void) const;
   QVector<QColor> GetRecentColors(void) const;
   int32_t GetNextRecentColorButtonNumber(void) const;
   int32_t GetScreenshotGifSucessTimeout(void) const;

   void GetMostRecentFolder(QString & orc_Str) const;
   void GetRecentFolders(QStringList & orc_Folders) const;
   static void h_GetLanguages(QStringList & orc_List);

   // Project specific get
   int32_t GetProjLastMode(void) const;
   int32_t GetProjSdTopologyViewZoom(void) const;
   QPoint GetProjSdTopologyViewPos(void) const;
   QString GetProjSdTopologyLastKnownTspPath(void) const;
   QString GetProjSdTopologyLastKnownCodeExportPath(void) const;
   QString GetProjSdTopologyLastKnownExportPath(void) const;
   QString GetProjSdTopologyLastKnownImportPath(void) const;
   QString GetProjSdTopologyLastKnownCanOpenEdsPath(void) const;
   QString GetProjSdTopologyLastKnownDeviceDefPath(void) const;
   QString GetProjSdTopologyLastKnownRtfPath(void) const;
   QString GetProjSdTopologyLastKnownRtfCompanyName(void) const;
   QString GetProjSdTopologyLastKnownRtfCompanyLogoPath(void) const;
   QString GetLastKnownHalcDefPath(void) const;
   QString GetLastKnownHalcImportPath(void) const;
   QString GetLastKnownHalcExportPath(void) const;
   QString GetLastKnownServiceProjectPath(void) const;
   QString GetLastKnownRamViewProjectPath(void) const;
   QString GetLastKnownJ1939CatalogPath(void) const;
   C_UsNode GetProjSdNode(const QString & orc_NodeName) const;
   C_UsCommunication GetProjSdBus(const QString & orc_BusName) const;
   C_UsSystemView GetProjSvSetupView(const QString & orc_ViewName) const;
   const QList<QString> GetProjSdNodeKeysInternal(void) const;
   const QList<QString> GetProjSdBusKeysInternal(void) const;
   const QList<QString> GetProjSvSetupViewKeysInternal(void) const;
   void GetProjLastScreenMode(int32_t & ors32_SysDefSubMode, uint32_t & oru32_SysDefIndex, uint32_t & oru32_SysDefFlag,
                              int32_t & ors32_SysViewSubMode, uint32_t & oru32_SysViewIndex,
                              uint32_t & oru32_SysViewFlag) const;
   int32_t GetProjLastSysDefNodeTabIndex(void) const;
   int32_t GetProjLastSysDefBusTabIndex(void) const;
   std::array<bool, 3> GetViewPermissions(const QString & orc_ViewName);

   //Set
   int32_t SetLanguage(const QString & orc_Lang);
   void SetPerformanceActive(const bool oq_Active);
   void SetRecentProjects(const QStringList & orc_New);
   void SetScreenPos(const QPoint & orc_New);
   void SetAppSize(const QSize & orc_New);
   void SetAppMaximized(const bool oq_New);
   void SetSdTopologyToolboxPos(const QPoint & orc_New);
   void SetSdTopologyToolboxSize(const QSize & orc_New);
   void SetNaviBarSize(const int32_t os32_Value);
   void SetNaviBarNodeSectionSize(const int32_t os32_Value);
   void SetSdNodeEditSplitterHorizontal(const int32_t os32_Value);
   void SetSdNodeEditHalcSplitterHorizontal(const int32_t os32_Value);
   void SetSdNodeEditCoManagerSplitterHorizontal(const int32_t os32_Value);
   void SetSdBusEditTreeSplitterHorizontal(const int32_t os32_Value);
   void SetSdBusEditTreeSplitterHorizontal2(const int32_t os32_Value);
   void SetSdBusEditLayoutSplitterHorizontal(const int32_t os32_Value);
   void SetSdTopologyToolboxMaximized(const bool & orq_New);
   void SetCurrentSaveAsPath(const QString & orc_Value);
   void SetRecentColors(const QVector<QColor> & orc_RecentColorsVector);
   void SetNextRecentColorButtonNumber(const int32_t os32_NextRecentColorButtonNumber);

   void AddToRecentProjects(const QString & orc_Str);
   void RemoveOfRecentProjects(const QString & orc_Str);
   void ClearRecentProjects(void);
   void SetScreenshotGifSucessTimeout(const int32_t os32_ScreenshotGifSucessTimer);

   // Project specific set
   void SetProjLastMode(const int32_t os32_New);
   void SetProjSdTopologyViewZoom(const int32_t os32_New);
   void SetProjSdTopologyViewPos(const QPoint & orc_New);
   void SetProjSdTopologyLastKnownTspPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownCodeExportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownExportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownImportPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownCanOpenEdsPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownDeviceDefPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfPath(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfCompanyName(const QString & orc_New);
   void SetProjSdTopologyLastKnownRtfCompanyLogoPath(const QString & orc_New);
   void SetLastKnownHalcDefPath(const QString & orc_NewPath);
   void SetLastKnownHalcImportPath(const QString & orc_NewPath);
   void SetLastKnownHalcExportPath(const QString & orc_NewPath);
   void SetLastKnownServiceProjectPath(const QString & orc_NewPath);
   void SetLastKnownRamViewProjectPath(const QString & orc_NewPath);
   void SetLastKnownJ1939CatalogPath(const QString & orc_NewPath);
   void SetProjSdNodeSelectedDatapoolName(const QString & orc_NodeName, const QString & orc_DatapoolName);
   void SetProjSdNodeSelectedProtocol(const QString & orc_NodeName,
                                      const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol);
   void SetProjSdNodeSelectedInterface(const QString & orc_NodeName, const uint32_t ou32_SelectedInterface);
   void SetProjSdNodeDatapoolOpenListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                           const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolSelectedListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                               const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolSelectedVariableNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                   const std::vector<QString> & orc_New);
   void SetProjSdNodeDatapoolCommMessageOverviewColumnWidth(const QString & orc_NodeName,
                                                            const QString & orc_DatapoolName,
                                                            const QString & orc_ListName,
                                                            const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeDatapoolCommSignalOverviewColumnWidth(const QString & orc_NodeName,
                                                           const QString & orc_DatapoolName,
                                                           const QString & orc_ListName,
                                                           const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeDatapoolListSelectedMessage(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                 const QString & orc_ListName,
                                                 const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                                 const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                                 const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetProjSdNodeDatapoolListColumnSizes(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                             const QString & orc_ListName,
                                             const std::vector<int32_t> & orc_ColumnWidths);
   void SetProjSdNodeCanOpenOverviewColumnWidth(const QString & orc_NodeName, const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeCanOpenPdoOverviewColumnWidth(const QString & orc_NodeName,
                                                   const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeSelectedCanOpenManager(const QString & orc_NodeName, const uint8_t & oru8_Value);
   void SetProjSdNodeSelectedCanOpenDevice(const QString & orc_NodeName, const uint8_t & oru8_Number,
                                           const QString & orc_Value);
   void SetProjSdNodeSelectedCanOpenDeviceUseCaseIndex(const QString & orc_NodeName, const uint32_t & oru32_Value);
   void SetProjSdNodeExpandedCanOpenTree(const QString & orc_NodeName, const std::map<uint8_t,
                                                                                      bool> & orc_Interfaces,
                                         const std::map<uint8_t, bool> & orc_Devices, const std::map<std::pair<uint8_t,
                                                                                                               std::pair<uint8_t,
                                                                                                                         stw::scl::C_SclString> >,
                                                                                                     bool> & orc_Device);
   void SetProjSdNodeCanOpenSelectedUseCaseOrInterface(const QString & orc_NodeName,
                                                       const bool & orq_IsUseCaseSelected);
   void SetProjSdNodeHalcOverviewColumnWidth(const QString & orc_NodeName, const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeHalcConfigColumnWidth(const QString & orc_NodeName, const std::vector<int32_t> & orc_Value);
   void SetProjSdNodeSelectedHalcDomain(const QString & orc_NodeName, const QString & orc_Value);
   void SetProjSdNodeSelectedHalcChannel(const QString & orc_NodeName, const QString & orc_Value);
   void SetProjSdBusSelectedMessage(const QString & orc_BusName,
                                    const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                    const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                    const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetProjSdBusCommMessageOverviewColumnWidth(const QString & orc_BusName, const std::vector<int32_t> & orc_Value);
   void SetProjSdBusCommSignalOverviewColumnWidth(const QString & orc_BusName, const std::vector<int32_t> & orc_Value);
   void SetProjSvNavigationExpandedStatus(const QString & orc_ViewName, const bool oq_NavigationExpandedStatus);
   void SetProjSvSetupViewZoom(const QString & orc_ViewName, const int32_t os32_New);
   void SetProjSvSetupViewPos(const QString & orc_ViewName, const QPoint & orc_New);
   void SetProjSvUpdateViewZoom(const QString & orc_ViewName, const int32_t os32_New);
   void SetProjSvUpdateViewPos(const QString & orc_ViewName, const QPoint & orc_New);
   void SetProjSvParamExport(const QString & orc_ViewName, const QString & orc_Path);
   void SetProjSvParamImport(const QString & orc_ViewName, const QString & orc_Path);
   void SetProjSvParamRecord(const QString & orc_ViewName, const QString & orc_Path, const QString & orc_FileName);
   void AddProjSvNodeUpdateDataRate(const QString & orc_ViewName, const QString & orc_NodeName,
                                    const uint32_t ou32_Checksum, const float64_t of64_DataRateBytesPerMs);
   void SetProjSvUpdateSplitterHorizontal(const QString & orc_ViewName, const int32_t os32_Value);
   void SetProjSvUpdateHorizontalSplitterVertical(const QString & orc_ViewName, const int32_t os32_Value);
   void SetProjSvUpdateProgressLog(const QString & orc_ViewName, const QPoint & orc_Position, const QSize & orc_Size,
                                   const bool & orq_Maximized);
   void SetProjSvUpdateSummaryBig(const QString & orc_ViewName, const bool oq_BigVisible);
   void SetProjSvUpdateEmptyOptionalSectionsVisible(const QString & orc_ViewName, const bool oq_Visible);
   void SetProjSvUpdateSectionsExpandedFlags(const QString & orc_ViewName, const QString & orc_NodeName,
                                             const QVector<bool> & orc_SectionsExpanded);
   void SetProjSvDashboardToolbox(const QString & orc_ViewName, const QPoint & orc_Position, const QSize & orc_Size,
                                  const bool & orq_Maximized);
   void SetProjSvDashboardSelectedTabIndex(const QString & orc_ViewName, const int32_t os32_Index);
   void SetProjSvDashboardMainTab(const QString & orc_ViewName, const QString & orc_DashboardName);
   void SetProjSvDashboardTearOffPosition(const QString & orc_ViewName, const QString & orc_DashboardName,
                                          const QPoint & orc_Position, const QSize & orc_Size,
                                          const bool oq_TornOffWindowMinimized, const bool oq_TornOffWindowMaximized);
   void SetProjSvDashboardScenePositionAndZoom(const QString & orc_ViewName, const QString & orc_DashboardName,
                                               const QPoint & orc_Position, const int32_t os32_Zoom);
   void SetProjLastScreenMode(const int32_t os32_SysDefSubMode, const uint32_t ou32_SysDefIndex,
                              const uint32_t ou32_SysDefFlag, const int32_t os32_SysViewSubMode,
                              const uint32_t ou32_SysViewIndex, const uint32_t ou32_SysViewFlag);
   void SetProjLastSysDefNodeTabIndex(const int32_t os32_SysDefNodeEditTabIndex);
   void SetProjLastSysDefBusTabIndex(const int32_t os32_SysDefBusEditTabIndex);
   void CopyProjSvSettings(const QString & orc_SourceViewName, const QString & orc_TargetViewName);
   void SetViewPermission(const QString & orc_ViewName, std::array<bool, 3 > & orc_Permissions);

   void ClearMaps(void);

   void Save(void) const;

   static int32_t h_CheckLanguageExists(const QString & orc_Str);
   static int32_t h_GetParentFolder(const QString & orc_CompletePath, QString & orc_Parent,
                                    const bool & orq_CompletePathContainsFile);

private:
   static C_UsHandler * mhpc_Singleton;

   //Avoid call
   C_UsHandler(void);
   virtual ~C_UsHandler();
   C_UsHandler(const C_UsHandler &);
   C_UsHandler & operator =(const C_UsHandler &) &;

   const QString mc_IniPathAndName;               ///< Fix location of user settings
   QString mc_ActualProject;                      ///< Project specific settings for this concrete project
   static const uint8_t mhu8_MAX_RECENT_PROJECTS; ///< Maximum number of entries in unique history of recent
   // projects
   static const QPoint mhc_DEFAULT_VIEW_POS;      ///< Default view position
   static const int32_t mhs32_DEFAULT_ZOOM_LEVEL; ///< Default zoom level
   const QString mc_DefaultProjectsFolder;        ///< Default openSYDE projects location if no recent projects
                                                  // are available
   QString mc_Lang;                               ///< Current language
   bool mq_PerformanceMeasurementActive;          ///< Flag if performance measurement is active (log entries)
   QString mc_CurrentSaveAsPath;                  ///< Current save as base path
   QVector<QColor> mc_RecentColors;               ///< Recent colors from color picker
   int32_t ms32_NextRecentColorButtonNumber;      ///< Next recent color button for color from color picker
   QStringList mc_RecentProjects;                 ///< Unique history of recent projects
   QPoint mc_ScreenPos;                           ///< History of last known screen position
   QSize mc_AppSize;                              ///< History of last known openSyde window size
   bool mq_AppMaximized;                          ///< History of openSyde window state
   QPoint mc_SdTopologyToolboxPos;                ///< History of last known sys def toolbox position
   QSize mc_SdTopologyToolboxSize;                ///< History of last known sys def toolbox size
   int32_t ms32_NaviBarSize;                      ///< Last known navi bar size
   int32_t ms32_NaviBarNodeSectionSize;           ///< Last known navi bar node section size
   int32_t ms32_SdNodeEditSplitterHorizontal;     ///< History of last known node edit splitter position x
   // value
   int32_t ms32_SdNodEditHalcSplitterHorizontal;      ///< History of last known halc splitter position x value
   int32_t ms32_SdNodEditCoManagerSplitterHorizontal; ///< History of last known CANopen Manager splitter position
   // x value
   int32_t ms32_SdBusEditTreeSplitterHorizontal; ///< History of last known bus edit tree splitter
   // position x value
   int32_t ms32_SdBusEditTreeSplitterHorizontal2; ///< History of last known sys def bus edit tree splitter
   // position 2 x value
   int32_t ms32_SdBusEditLayoutSplitterHorizontal; ///< History of last known sys def bus edit layout splitter
   // position x value
   bool mq_SdTopologyToolboxMaximized;                   ///< History of sys def toolbox state
   int32_t ms32_ProjLastKnownMode;                       ///< History of last known project mode (SD/SC/Main)
   int32_t ms32_ProjSdTopologyViewZoom;                  ///< History of last known zoom value for specific project
   QPoint mc_ProjSdTopologyViewPos;                      ///< History of last known viewport pos for specific project
   QString mc_ProjSdTopologyLastKnownTspPath;            ///< History of last known TSP path
   QString mc_ProjSdTopologyLastKnownCodeExportPath;     ///< History of last known code export path
   QString mc_ProjSdTopologyLastKnownExportPath;         ///< History of last known export path
   QString mc_ProjSdTopologyLastKnownImportPath;         ///< History of last known import path
   QString mc_ProjSdTopologyLastKnownCanOpenEdsPath;     ///< History of last known CAN open path
   QString mc_ProjSdTopologyLastKnownDeviceDefPath;      ///< History of last known import path for a syde_devdef-file
   QString mc_ProjSdTopologyLastKnownRtfPath;            ///< History of last known rtf file export path
   QString mc_ProjSdTopologyLastKnownRtfCompanyName;     ///< History of last known rtf file export company name
   QString mc_ProjSdTopologyLastKnownRtfCompanyLogoPath; ///< History of last known rtf file export company logo path
   QString mc_LastKnownHalcDefPath;                      ///< History of last known HALC definition file path
   QString mc_LastKnownHalcImportPath;                   ///< History of last known HALC import file path
   QString mc_LastKnownHalcExportPath;                   ///< History of last known HALC export file path
   QString mc_LastKnownServiceProjectPath;               ///< History of last known service project path
   QString mc_LastKnownRamViewProjectPath;               ///< History of last known RAMView project import path
   QString mc_LastKnownJ1939CatalogPath;                 ///< History of last known J1939 catalog import path
   QMap<QString, C_UsSystemView> mc_ProjSvSetupView;     ///< History of last known view user settings
   QMap<QString, C_UsNode> mc_ProjSdNode;                ///< History of last known node user settings
   QMap<QString, C_UsCommunication> mc_ProjSdBus;        ///< History of last known bus user settings
   int32_t ms32_SysDefSubMode;                           ///< History of last known system definition sub mode
   uint32_t mu32_SysDefIndex;                            ///< History of last known system definition index
   uint32_t mu32_SysDefFlag;                             ///< History of last known system definition flag
   int32_t ms32_SysViewSubMode;                          ///< History of last known system view sub mode
   uint32_t mu32_SysViewIndex;                           ///< History of last known system view index
   uint32_t mu32_SysViewFlag;                            ///< History of last known system view flag
   int32_t ms32_SysDefNodeEditTabIndex;                  ///< History of last known tab index in node edit
   int32_t ms32_SysDefBusEditTabIndex;                   ///< History of last known tab index in bus edit
   int32_t ms32_ScreenshotGifSucessTimeout;              ///< Screenshot GIF Play timer
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

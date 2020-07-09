//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user settings data (header)

   This class handles all actions concerning user settings.
   (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_USHANDLER_H
#define C_USHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPoint>
#include <QSize>
#include <QMap>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsHandler
{
public:
   enum E_SettingsSubSection
   {
      eNONE = 0,
      eDATABASE,
      eDLLCONFIG,
      eFILTER,
      eLOGGING
   };

   static C_UsHandler * h_GetInstance(void);
   static void h_Destroy(void);

   void ChangeActiveProjectName(const QString & orc_ActiveProject);
   void LoadActiveProject(const QString & orc_ActiveProject);

   void SetDefault(void);

   //Get
   QStringList GetRecentProjects(void) const;
   QPoint GetScreenPos(void) const;
   QSize GetAppSize(void) const;
   bool GetAppMaximized(void) const;
   bool GetButtonHexActive(void) const;
   bool GetButtonRelativeTimeStampActive(void) const;
   bool GetButtonUniqueViewActive(void) const;
   bool GetTraceSettingDisplayTimestampAbsoluteTimeOfDay(void) const;
   stw_types::uint32 GetTraceSettingBufferSize(void) const;
   stw_types::sint32 GetSelectedProtocolIndex(void) const;
   static stw_types::uint8 h_GetMaxRecentProjects(void);
   QString GetLastKnownDatabasePath(void) const;
   stw_types::sint32 GetSplitterSettingsX(void) const;
   bool GetSettingsAreExpanded(void) const;
   stw_types::sint32 GetSplitterMessageGenY(void) const;
   bool GetMessageGenIsExpanded(void) const;
   stw_types::sint32 GetSplitterMesSigX(void) const;
   const std::vector<stw_types::sint32> & GetSelectedMessages(void) const;
   const std::vector<stw_types::sint32> & GetTraceColWidths(void) const;
   const std::vector<stw_types::sint32> & GetTraceColPositions(void) const;
   const std::vector<stw_types::sint32> & GetMessageColWidths(void) const;
   const std::vector<stw_types::sint32> & GetSignalsColWidths(void) const;
   bool GetWiDatabaseExpanded(void) const;
   bool GetWiDllConfigExpanded(void) const;
   bool GetWiFilterExpanded(void) const;
   bool GetWiLoggingExpanded(void) const;
   E_SettingsSubSection GetPopOpenSection(void) const;

   void GetMostRecentFolder(QString & orc_Str) const;
   void GetRecentFolders(QStringList & orc_Folders) const;

   //Set
   void SetRecentProjects(const QStringList & orc_New);
   void SetScreenPos(const QPoint & orc_New);
   void SetAppSize(const QSize & orc_New);
   void SetAppMaximized(const bool oq_New);
   void SetButtonHexActive(const bool oq_New);
   void SetButtonRelativeTimeStampActive(const bool oq_New);
   void SetButtonUniqueViewActive(const bool oq_New);
   void SetTraceSettingDisplayTimestampAbsoluteTimeOfDay(const bool oq_New);
   void SetTraceSettingBufferSize(const stw_types::uint32 ou32_New);
   void SetSelectedProtocolIndex(const stw_types::sint32 os32_New);
   void SetCurrentSaveAsPath(const QString & orc_Value);
   void SetLastKnownDatabasePath(const QString & orc_Value);
   void SetSplitterSettingsX(const stw_types::sint32 os32_New);
   void SetSettingsAreExpanded(const bool oq_New);
   void SetSplitterMessageGenY(const stw_types::sint32 os32_New);
   void SetMessageGenIsExpanded(const bool oq_New);
   void SetSplitterMesSigX(const stw_types::sint32 os32_New);
   void SetSelectedMessages(const std::vector<stw_types::sint32> & orc_Value);
   void SetTraceColWidths(const std::vector<stw_types::sint32> & orc_Value);
   void SetTraceColPositions(const std::vector<stw_types::sint32> & orc_Value);
   void SetMessageColWidths(const std::vector<stw_types::sint32> & orc_Value);
   void SetSignalsColWidths(const std::vector<stw_types::sint32> & orc_Value);
   void SetWiDatabaseExpanded(const bool oq_New);
   void SetWiDllConfigExpanded(const bool oq_New);
   void SetWiFilterExpanded(const bool oq_New);
   void SetWiLoggingExpanded(const bool oq_New);
   void SetPopOpenSection(const E_SettingsSubSection oe_PopOpenSection);

   void AddToRecentProjects(const QString & orc_Str);
   void RemoveOfRecentProjects(const QString & orc_Str);
   void ClearRecentProjects(void);

   void Save(void) const;

   static stw_types::sint32 h_GetParentFolder(const QString & orc_CompletePath, QString & orc_Parent,
                                              const bool & orq_CompletePathContainsFile);

private:
   static C_UsHandler * mhpc_Singleton;

   //Avoid call
   C_UsHandler(void);
   virtual ~C_UsHandler();
   C_UsHandler(const C_UsHandler &);
   C_UsHandler & operator =(const C_UsHandler &);

   const QString mc_IniPathAndName;                       ///< Fix location of user settings
   QString mc_ActualProject;                              ///< Project specific settings for this concrete project
   static const stw_types::uint8 mhu8_MaxRecentProjects;  ///< Maximum number of entries in unique history of recent
                                                          // projects
   const QString mc_DefaultProjectsFolder;                ///< Default openSYDE projects location if no recent
                                                          // projects are available
   QString mc_CurrentSaveAsPath;                          ///< Current save as base path
   QString mc_LastKnownDatabasePath;                      ///< History of last known database path
   QStringList mc_RecentProjects;                         ///< Unique history of recent projects
   QPoint mc_ScreenPos;                                   ///< History of last known screen position
   QSize mc_AppSize;                                      ///< History of last known openSyde window size
   bool mq_AppMaximized;                                  ///< History of openSyde window state
   bool mq_ButtonHexActive;                               ///< History of last known button hex state
   bool mq_ButtonRelativeTimeStampActive;                 ///< History of last known button time stamp state
   bool mq_ButtonUniqueViewActive;                        ///< History last known button unique state
   bool mq_TraceSettingDisplayTimestampAbsoluteTimeOfDay; ///< History last known timestamp absolute mode
   stw_types::uint32 mu32_TraceSettingBufferSize;         ///< History last known trace buffer size
   stw_types::sint32 ms32_SelectedProtocolIndex;          ///< History last known selected protocol
   stw_types::sint32 ms32_SplitterSettingsX;              ///< History of last known settings-splitter position x value
   stw_types::sint32 ms32_SplitterMessageGenY;            ///< History of last known message generator splitter
                                                          // position x value
   bool mq_SettingsAreExpanded;                           ///< History of last known settings state (collapsed/expanded)
   bool mq_MessageGenIsExpanded;                          ///< History of last known message generator state
                                                          // (collapsed/expanded)
   stw_types::sint32 ms32_SplitterMesSigX;                ///< History of last known settings-splitter position x value
   bool mq_WiDatabaseExpanded;                            ///< History of last known database widget expanded state
   bool mq_WiDllConfigExpanded;                           ///< History of last known DLL configuration widget expanded
                                                          // state
   bool mq_WiFilterExpanded;                              ///< History of last known filter widget expanded state
   bool mq_WiLoggingExpanded;                             ///< History of last known logging widget expanded state
   std::vector<stw_types::sint32> mc_SelectedMessages;    ///< History of last known selected message generator indices
   std::vector<stw_types::sint32> mc_TraceColWidth;       ///< History of last known trace column widths
   std::vector<stw_types::sint32> mc_TraceColPosition;    ///< History of last known trace column widths
   std::vector<stw_types::sint32> mc_MessageGenColWidth;  ///< History of last known message generator column widths
   std::vector<stw_types::sint32> mc_SignalsColWidth;     ///< History of last known message generator signals table
                                                          // column widths
   E_SettingsSubSection me_PopOpenSection;                ///< History of last known expanded settings subsection
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

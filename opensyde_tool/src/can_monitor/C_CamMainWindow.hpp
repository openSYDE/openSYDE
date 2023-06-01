//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window for project openSYDE CAN Monitor (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMAINWINDOW_H
#define C_CAMMAINWINDOW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMainWindow>
#include <QMimeData>

#include "C_CamComDriverBase.hpp"

#include "C_Can.hpp"
#include "C_SyvComDriverThread.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMainWindow;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMainWindow :
   public QMainWindow
{
   Q_OBJECT

public:
   explicit C_CamMainWindow(QWidget * const opc_Parent = NULL);
   ~C_CamMainWindow(void) override;
   void CanFilterMsgDropped(void);

protected:
   void closeEvent(QCloseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   Ui::C_CamMainWindow * mpc_Ui;

   static const int32_t mhs32_SETTINGS_SPLITTER_MAX;
   static const int32_t mhs32_MESSAGE_GEN_SPLITTER_MAX;

   stw::opensyde_gui_logic::C_CamComDriverBase mc_ComDriver;
   stw::can::C_Can * mpc_CanDllDispatcher;
   stw::opensyde_gui_logic::C_SyvComDriverThread * mpc_CanThread;
   bool mq_LoggingStarted;

   // For loading files
   QString mc_CurrentLoadedFile;
   QString mc_CurrentLoadedFileOrg;
   std::vector<stw::scl::C_SclString> mc_OsySysDefs;
   std::vector<uint32_t> mc_OsySysDefBuses;
   std::vector<stw::scl::C_SclString> mc_DbcFiles;

   // For custom splitter resizing
   int32_t ms32_SettingsSplitterPosition;
   int32_t ms32_MessageGenSplitterPosition;

   //Avoid call
   C_CamMainWindow(const C_CamMainWindow &);
   C_CamMainWindow & operator =(const C_CamMainWindow &) &;

   void m_StartLogging(void);
   void m_PauseLogging(void);
   void m_ContinueLogging(void);
   void m_StopLogging(void);

   int32_t m_InitCan(int32_t & ors32_Bitrate);
   void m_CloseCan(void);

   void m_LoadInitialProject(void);
   void m_LoadUserSettings(void);
   void m_SaveUserSettings(void);
   static bool mh_CheckMime(const QMimeData * const opc_Mime, QString * const opc_FilePath = NULL);

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   void m_OnExpandSettings(const bool oq_Expand);
   void m_OnSettingsSplitterHandleDoubleClick(void) const;
   void m_OnSettingsSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index);

   void m_OnExpandMessageGen(const bool oq_Expand);
   void m_OnMessageGenSplitterHandleDoubleClick(void) const;
   void m_OnMessageGenSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index);

   void m_RegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void m_SendMessage(const uint32_t ou32_MessageIndex, const uint32_t ou32_TimeToSend);
   void m_RemoveAllCyclicMessages(void);

   void m_OnClearOldConfiguration(void) const;
   void m_OnNewConfiguration(void);
   void m_OnDatabaseLoadStarted(const QString & orc_File, const QString & orc_OrgPath);
   void m_OnDatabaseLoadFinished(const int32_t os32_Result);
   void m_OnDatabaseRemove(const QString & orc_File, const QString & orc_OrgPath, const bool oq_IsUpdate);
   void m_OnActivateDatabase(const QString & orc_File, const QString & orc_OrgPath, const bool oq_Active);
   void m_OnOsyChangeBus(const QString & orc_File, const QString & orc_OrgPath, const uint32_t ou32_BusIndex);
   void m_OnCanDllConfigChange(void);
   void m_CheckMessagesForLoadedDatabase(const QString & orc_DatabasePath);
   void m_CheckForLastDatabaseLoaded(const QString & orc_DatabasePath);
   void m_DisplayCheckMessagesDialog(const QString & orc_DatabasePath, const std::vector<uint32_t> & orc_Indices);
   void m_AddFilterData(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd);

Q_SIGNALS:
   void SigEmitAddFilterToChildWidget(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd);
   void SigSendCanMsgDroppedToChildrenWidget(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

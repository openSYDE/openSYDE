//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Project system view user settings (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEW_H
#define C_USSYSTEMVIEW_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QMap>
#include <QPoint>
#include "stwtypes.h"
#include "C_UsSystemViewDashboard.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UsSystemView
{
public:
   C_UsSystemView(void);
   C_UsSystemView(const stw_types::sintn osn_SetupViewZoom, const QPoint & orc_SetupViewPos);

   void SetSetupNodeInfoSize(const QSize & orc_Size);
   QSize GetSetupNodeInfoSize(void) const;
   void SetSetupNodeInfoPos(const QPoint & orc_Pos);
   QPoint GetSetupNodeInfoPos(void) const;
   void SetSetupNodeInfoMaximized(const bool oq_Maximized);
   bool GetSetupNodeInfoMaximized(void) const;
   void SetDashboardMainTab(const QString & orc_DashboardName, const stw_types::sint32 os32_TabIndex);
   void SetDashboardTearOffPosition(const QString & orc_DashboardName, const QPoint & orc_Position,
                                    const QSize & orc_Size, const bool oq_TornOffWindowMinimized,
                                    const bool oq_TornOffWindowMaximized);
   void SetProjSvDashboardScenePositionAndZoom(const QString & orc_DashboardName, const QPoint & orc_Position,
                                               const stw_types::sintn osn_Zoom);
   C_UsSystemViewDashboard GetDashboardSettings(const QString & orc_DashboardName) const;
   const QList<QString> GetDashboardKeysInternal(void) const;

   const QMap<stw_types::uint32, stw_types::uint64> & GetUpdateDataRateHistory(void) const;
   void AddUpdateDataRate(const stw_types::uint32 ou32_Checksum, const stw_types::uint64 ou64_DataRateBytesPerS);
   stw_types::sint32 GetUpdateSplitterX(void) const;
   void SetUpdateSplitterX(const stw_types::sint32 os32_Value);
   QPoint GetUpdateProgressLogPos(void) const;
   void SetUpdateProgressLogPos(const QPoint & orc_Value);
   QSize GetUpdateProgressLogSize(void) const;
   void SetUpdateProgressLogSize(const QSize & orc_Value);
   bool GetUpdateProgressLogMaximized(void) const;
   void SetUpdateProgressLogMaximized(const bool oq_Value);

   void SetNavigationExpandedStatus(const bool oq_NavigationExpandedStatus);
   bool GetNavigationExpandedStatus(void) const;

   QPoint GetDashboardToolboxPos(void) const;
   void SetDashboardToolboxPos(const QPoint & orc_Value);
   QSize GetDashboardToolboxSize(void) const;
   void SetDashboardToolboxSize(const QSize & orc_Value);
   bool GetDashboardToolboxMaximized(void) const;
   void SetDashboardToolboxMaximized(const bool oq_Value);

   stw_types::sintn sn_SetupViewZoom;
   QPoint c_SetupViewPos;
   stw_types::sintn sn_UpdateViewZoom;
   QPoint c_UpdateViewPos;
   QString c_ParamExportPath;
   QString c_ParamImportPath;
   QString c_ParamRecordPath;
   QString c_ParamRecordFileName;

   // Node read info widget configuration
   QPoint c_NodeInfoPos;
   QSize c_NodeInfoSize;
   bool q_NodeInfoMaximized;

   stw_types::sint32 GetDashboardSelectedTabIndex(void) const;
   void SetDashboardSelectedTabIndex(const stw_types::sint32 & orc_Value);

private:
   QMap<QString, C_UsSystemViewDashboard> mc_Dashboards;                ///< History of last known view dashboard user
                                                                        // settings
   QMap<stw_types::uint32, stw_types::uint64> mc_UpdateDataRateHistory; ///< History of last known view update
                                                                        // effective data rates
   bool mq_NavigationExpanded;                                          ///< History of last known navigation status
   stw_types::sint32 ms32_UpdateSplitterX;                              ///< History of last known update splitter
                                                                        // position x value
   QPoint mc_UpdateProgressLogPos;                                      ///< History of last known update progress log
                                                                        // position
   QSize mc_UpdateProgressLogSize;                                      ///< History of last known update progress log
                                                                        // size
   bool mq_UpdateProgressLogMaximized;                                  ///< History of update progress log state
   QPoint mc_DashboardToolboxPos;                                       ///< History of last known dashboard toolbox
                                                                        // position
   QSize mc_DashboardToolboxSize;                                       ///< History of last known dashboard toolbox
                                                                        // size
   bool mq_DashboardToolboxMaximized;                                   ///< History of dashboard toolbox state
   stw_types::sint32 ms32_DashboardSelectedTabIndex;                    ///< History of dashboard selected tab index
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

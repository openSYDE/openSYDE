//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project system view user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEW_H
#define C_USSYSTEMVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QMap>
#include <QPoint>
#include "stwtypes.h"
#include "C_UsSystemViewDashboard.h"
#include "C_UsSystemViewNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemView
{
public:
   C_UsSystemView(void);
   C_UsSystemView(const stw_types::sintn osn_SetupViewZoom, const QPoint & orc_SetupViewPos);

   // Setup
   // put System Commissioning Setup user settings Setter & Getter here

   // Update
   void AddNodeUpdateDataRate(const QString & orc_NodeName, const stw_types::uint32 ou32_Checksum,
                              const stw_types::float64 of64_DataRateBytesPerMs);
   stw_types::sint32 GetUpdateSplitterX(void) const;
   stw_types::sint32 GetUpdateHorizontalSplitterY(void) const;
   void SetUpdateSplitterX(const stw_types::sint32 os32_Value);
   void SetUpdateHorizontalSplitterY(const stw_types::sint32 os32_Value);
   QPoint GetUpdateProgressLogPos(void) const;
   void SetUpdateProgressLogPos(const QPoint & orc_Value);
   QSize GetUpdateProgressLogSize(void) const;
   void SetUpdateProgressLogSize(const QSize & orc_Value);
   bool GetUpdateProgressLogMaximized(void) const;
   void SetUpdateProgressLogMaximized(const bool oq_Value);
   bool GetUpdateSummaryBig(void) const;
   void SetUpdateSummaryBig(const bool oq_Value);
   bool GetUpdatePackEmptyOptionalSectionsVisible(void) const;
   void SetUpdateEmptyOptionalSectionsVisible(const bool oq_Visible);
   C_UsSystemViewNode GetSvNode(const QString & orc_NodeName) const;
   void SetNodeSectionsExpanded(const QString & orc_NodeName, const QMap<stw_types::uint32, bool> & orc_ExpandedFlags);
   const QList<QString> GetViewNodesKeysInternal(void) const;

   // Navigation
   void SetNavigationExpandedStatus(const bool oq_NavigationExpandedStatus);
   bool GetNavigationExpandedStatus(void) const;

   // Dashboard
   void SetDashboardMainTab(const QString & orc_DashboardName);
   void SetDashboardTearOffPosition(const QString & orc_DashboardName, const QPoint & orc_Position,
                                    const QSize & orc_Size, const bool oq_TornOffWindowMinimized,
                                    const bool oq_TornOffWindowMaximized);
   void SetProjSvDashboardScenePositionAndZoom(const QString & orc_DashboardName, const QPoint & orc_Position,
                                               const stw_types::sintn osn_Zoom);
   C_UsSystemViewDashboard GetDashboardSettings(const QString & orc_DashboardName) const;
   const QList<QString> GetDashboardKeysInternal(void) const;
   QPoint GetDashboardToolboxPos(void) const;
   void SetDashboardToolboxPos(const QPoint & orc_Value);
   QSize GetDashboardToolboxSize(void) const;
   void SetDashboardToolboxSize(const QSize & orc_Value);
   bool GetDashboardToolboxMaximized(void) const;
   void SetDashboardToolboxMaximized(const bool oq_Value);
   stw_types::sint32 GetDashboardSelectedTabIndex(void) const;
   void SetDashboardSelectedTabIndex(const stw_types::sint32 os32_Value);

   stw_types::sintn sn_SetupViewZoom;
   QPoint c_SetupViewPos;
   stw_types::sintn sn_UpdateViewZoom;
   QPoint c_UpdateViewPos;
   QString c_ParamExportPath;
   QString c_ParamImportPath;
   QString c_ParamRecordPath;
   QString c_ParamRecordFileName;

private:
   QMap<QString, C_UsSystemViewDashboard> mc_Dashboards; ///< History of last known view dashboard user
                                                         // settings
   // effective data rates (per node)
   bool mq_NavigationExpanded;                       ///< History of last known navigation status
   stw_types::sint32 ms32_UpdateSplitterX;           ///< History of last known update splitter position x value
   stw_types::sint32 ms32_UpdateHorizontalSplitterY; ///< History of last known update splitter  position x value
   QPoint mc_UpdateProgressLogPos;                   ///< History of last known update progress log position
   QSize mc_UpdateProgressLogSize;                   ///< History of last known update progress log size
   bool mq_UpdateProgressLogMaximized;               ///< History of update progress log state
   bool mq_UpdateSummaryBig;                         ///< History of last known update summary state
   bool mq_UpdateEmptyOptionalSectionsVisible;       ///< History of visibility state of empty optional sections
   QMap<QString, C_UsSystemViewNode> mc_Nodes;       ///< History of last known view node user settings
   QPoint mc_DashboardToolboxPos;                    ///< History of last known dashboard toolbox position
   QSize mc_DashboardToolboxSize;                    ///< History of last known dashboard toolbox size
   bool mq_DashboardToolboxMaximized;                ///< History of dashboard toolbox state
   stw_types::sint32 ms32_DashboardSelectedTabIndex; ///< History of dashboard selected tab index
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

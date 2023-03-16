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
#include "stwtypes.hpp"
#include "C_UsSystemViewDashboard.hpp"
#include "C_UsSystemViewNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemView
{
public:
   C_UsSystemView(void);
   C_UsSystemView(const int32_t os32_SetupViewZoom, const QPoint & orc_SetupViewPos);

   // Setup
   // put System Commissioning Setup user settings Setter & Getter here

   // Update
   void AddNodeUpdateDataRate(const QString & orc_NodeName, const uint32_t ou32_Checksum,
                              const float64_t of64_DataRateBytesPerMs);
   int32_t GetUpdateSplitterHorizontal(void) const;
   int32_t GetUpdateHorizontalSplitterVertical(void) const;
   void SetUpdateSplitterHorizontal(const int32_t os32_Value);
   void SetUpdateHorizontalSplitterVertical(const int32_t os32_Value);
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
   void SetNodeSectionsExpanded(const QString & orc_NodeName, const QVector<bool> & orc_ExpandedFlags);
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
                                               const int32_t os32_Zoom);
   C_UsSystemViewDashboard GetDashboardSettings(const QString & orc_DashboardName) const;
   const QList<QString> GetDashboardKeysInternal(void) const;
   QPoint GetDashboardToolboxPos(void) const;
   void SetDashboardToolboxPos(const QPoint & orc_Value);
   QSize GetDashboardToolboxSize(void) const;
   void SetDashboardToolboxSize(const QSize & orc_Value);
   bool GetDashboardToolboxMaximized(void) const;
   void SetDashboardToolboxMaximized(const bool oq_Value);
   int32_t GetDashboardSelectedTabIndex(void) const;
   void SetDashboardSelectedTabIndex(const int32_t os32_Value);
   bool GetSetupPermission(void) const;
   void SetSetupPermission(const bool oq_Value);
   bool GetUpdatePermission(void) const;
   void SetUpdatePermission(const bool oq_Value);
   bool GetDashboardPermission(void) const;
   void SetDashboardPermission(const bool oq_Value);

   int32_t s32_SetupViewZoom;
   QPoint c_SetupViewPos;
   int32_t s32_UpdateViewZoom;
   QPoint c_UpdateViewPos;
   QString c_ParamExportPath;
   QString c_ParamImportPath;
   QString c_ParamRecordPath;
   QString c_ParamRecordFileName;

private:
   QMap<QString, C_UsSystemViewDashboard> mc_Dashboards; ///< History of last known view dashboard user
                                                         // settings
   // effective data rates (per node)
   bool mq_NavigationExpanded;                    ///< History of last known navigation status
   int32_t ms32_UpdateSplitterHorizontal;         ///< History of last known update splitter position x value
   int32_t ms32_UpdateHorizontalSplitterVertical; ///< History of last known update splitter  position x value
   QPoint mc_UpdateProgressLogPos;                ///< History of last known update progress log position
   QSize mc_UpdateProgressLogSize;                ///< History of last known update progress log size
   bool mq_UpdateProgressLogMaximized;            ///< History of update progress log state
   bool mq_UpdateSummaryBig;                      ///< History of last known update summary state
   bool mq_UpdateEmptyOptionalSectionsVisible;    ///< History of visibility state of empty optional sections
   QMap<QString, C_UsSystemViewNode> mc_Nodes;    ///< History of last known view node user settings
   QPoint mc_DashboardToolboxPos;                 ///< History of last known dashboard toolbox position
   QSize mc_DashboardToolboxSize;                 ///< History of last known dashboard toolbox size
   bool mq_DashboardToolboxMaximized;             ///< History of dashboard toolbox state
   int32_t ms32_DashboardSelectedTabIndex;        ///< History of dashboard selected tab index
   bool mq_SetupPermission;                       ///< History of last known setup permission
   bool mq_UpdatePermission;                      ///< History of last known update permission
   bool mq_DashboardPermission;                   ///< History of last known dashboard permission
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

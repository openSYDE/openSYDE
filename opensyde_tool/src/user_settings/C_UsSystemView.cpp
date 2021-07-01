//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project system view user settings (implementation)

   Project system view user settings

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_UsSystemView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemView::C_UsSystemView(void) :
   sn_SetupViewZoom(100),
   c_SetupViewPos(0, 0),
   sn_UpdateViewZoom(100),
   c_UpdateViewPos(0, 0),
   mq_NavigationExpanded(false),
   ms32_UpdateSplitterX(-1),
   ms32_UpdateHorizontalSplitterY(-1),
   mc_UpdateProgressLogPos(-1, -1),
   mc_UpdateProgressLogSize(600, 400),
   mq_UpdateProgressLogMaximized(true),
   mq_UpdateSummaryBig(true),
   mq_UpdateEmptyOptionalSectionsVisible(true),
   mc_DashboardToolboxPos(-1, -1),
   mc_DashboardToolboxSize(600, 400),
   mq_DashboardToolboxMaximized(true),
   ms32_DashboardSelectedTabIndex(-1),
   mq_SetupPermission(false),
   mq_UpdatePermission(false),
   mq_DashboardPermission(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  osn_SetupViewZoom    Zoom init value
   \param[in]  orc_SetupViewPos     Position init value
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemView::C_UsSystemView(const sintn osn_SetupViewZoom, const QPoint & orc_SetupViewPos) :
   sn_SetupViewZoom(osn_SetupViewZoom),
   c_SetupViewPos(orc_SetupViewPos),
   sn_UpdateViewZoom(100),
   c_UpdateViewPos(0, 0),
   mq_NavigationExpanded(false),
   ms32_UpdateSplitterX(-1),
   ms32_UpdateHorizontalSplitterY(-1),
   mc_UpdateProgressLogPos(-1, -1),
   mc_UpdateProgressLogSize(600, 400),
   mq_UpdateProgressLogMaximized(true),
   mq_UpdateSummaryBig(true),
   mq_UpdateEmptyOptionalSectionsVisible(true),
   mc_DashboardToolboxPos(-1, -1),
   mc_DashboardToolboxSize(600, 400),
   mq_DashboardToolboxMaximized(true),
   ms32_DashboardSelectedTabIndex(-1),
   mq_SetupPermission(false),
   mq_UpdatePermission(false),
   mq_DashboardPermission(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add update data rates per node

   \param[in]  orc_NodeName               Node name
   \param[in]  ou32_Checksum              Checksum
   \param[in]  of64_DataRateBytesPerMs    Data rate bytes per ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::AddNodeUpdateDataRate(const QString & orc_NodeName, const uint32 ou32_Checksum,
                                           const float64 of64_DataRateBytesPerMs)
{
   if (this->mc_Nodes.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewNode & rc_Node = this->mc_Nodes.operator [](orc_NodeName);
      rc_Node.AddUpdateDataRate(ou32_Checksum, of64_DataRateBytesPerMs);
   }
   else
   {
      C_UsSystemViewNode c_Node;
      c_Node.AddUpdateDataRate(ou32_Checksum, of64_DataRateBytesPerMs);
      this->mc_Nodes.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get update splitter X value

   \return
   Current update splitter X value
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_UsSystemView::GetUpdateSplitterX(void) const
{
   return this->ms32_UpdateSplitterX;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get horizontal update splitter Y value

   \return
   Current update splitter Y value
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_UsSystemView::GetUpdateHorizontalSplitterY() const
{
   return this->ms32_UpdateHorizontalSplitterY;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update splitter X value

   \param[in]  os32_Value  New update splitter X value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateSplitterX(const sint32 os32_Value)
{
   this->ms32_UpdateSplitterX = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set horizontal update splitter Y value

   \param[in]  os32_Value  New update splitter Y value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateHorizontalSplitterY(const sint32 os32_Value)
{
   this->ms32_UpdateHorizontalSplitterY = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get update progress log position

   \return
   Current update progress log position
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_UsSystemView::GetUpdateProgressLogPos(void) const
{
   return this->mc_UpdateProgressLogPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update progress log position

   \param[in]  orc_Value   New update progress log position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogPos(const QPoint & orc_Value)
{
   this->mc_UpdateProgressLogPos = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get update progress log size

   \return
   Current update progress log size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_UsSystemView::GetUpdateProgressLogSize(void) const
{
   return this->mc_UpdateProgressLogSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update progress log size

   \param[in]  orc_Value   New update progress log size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogSize(const QSize & orc_Value)
{
   this->mc_UpdateProgressLogSize = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get update progress log maximized flag

   \return
   Current update progress log maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetUpdateProgressLogMaximized(void) const
{
   return this->mq_UpdateProgressLogMaximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update progress log maximized flag

   \param[in]  oq_Value    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogMaximized(const bool oq_Value)
{
   this->mq_UpdateProgressLogMaximized = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get style type of project system view update summary widget

   \retval true   big summary widget
   \retval false  small summary widget
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetUpdateSummaryBig(void) const
{
   return this->mq_UpdateSummaryBig;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set style type of project system view update summary widget

   \param[in]  oq_Value    true: big summary widget; false: small summary widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateSummaryBig(const bool oq_Value)
{
   this->mq_UpdateSummaryBig = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get visibility of empty optional sections in update package

   \retval true   show empty optional sections
   \retval false  hide     -"-
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetUpdatePackEmptyOptionalSectionsVisible(void) const
{
   return this->mq_UpdateEmptyOptionalSectionsVisible;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set style type of project system view update summary widget

   \param[in]  oq_Visible  show/hide emtpy optional sections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdateEmptyOptionalSectionsVisible(const bool oq_Visible)
{
   this->mq_UpdateEmptyOptionalSectionsVisible = oq_Visible;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get restored system view node user settings.

   \param[in]  orc_NodeName   Node name (identifier)

   \return
   Restored user settings if dashboard name was found; else default values.
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemViewNode C_UsSystemView::GetSvNode(const QString & orc_NodeName) const
{
   return this->mc_Nodes.value(orc_NodeName, C_UsSystemViewNode());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node section expanded flags.

   \param[in]  orc_NodeName         Node name (identifier)
   \param[in]  orc_ExpandedFlags    Expanded flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetNodeSectionsExpanded(const QString & orc_NodeName, const QVector<bool> & orc_ExpandedFlags)
{
   if (this->mc_Nodes.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewNode & rc_Node = this->mc_Nodes.operator [](orc_NodeName);
      rc_Node.SetSectionsExpanded(orc_ExpandedFlags);
   }
   else
   {
      C_UsSystemViewNode c_Node;
      c_Node.SetSectionsExpanded(orc_ExpandedFlags);
      this->mc_Nodes.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get view nodes keys internal structure

   \return
   View nodes keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsSystemView::GetViewNodesKeysInternal(void) const
{
   return this->mc_Nodes.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the status for the navigation expansion state

   \param[in]  oq_NavigationExpandedStatus   New navigation expansion state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetNavigationExpandedStatus(const bool oq_NavigationExpandedStatus)
{
   this->mq_NavigationExpanded = oq_NavigationExpandedStatus;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the status for the navigation expansion state

   \return
   The status for the navigation expansion state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetNavigationExpandedStatus(void) const
{
   return this->mq_NavigationExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard integrated tab index

   \param[in]  orc_DashboardName    Dashboard name (identifier)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardMainTab(const QString & orc_DashboardName)
{
   if (this->mc_Dashboards.contains(orc_DashboardName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewDashboard & rc_Dashboard = this->mc_Dashboards.operator [](orc_DashboardName);
      rc_Dashboard.q_TornOff = false;
   }
   else
   {
      C_UsSystemViewDashboard c_DashboardSettings;
      c_DashboardSettings.q_TornOff = false;
      this->mc_Dashboards.insert(orc_DashboardName, c_DashboardSettings);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard torn off window data

   \param[in]  orc_DashboardName          Dashboard name (identifier)
   \param[in]  orc_Position               Torn off window position
   \param[in]  orc_Size                   Torn off window size
   \param[in]  oq_TornOffWindowMinimized  Torn off window minimized flag
   \param[in]  oq_TornOffWindowMaximized  Torn off window maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardTearOffPosition(const QString & orc_DashboardName, const QPoint & orc_Position,
                                                 const QSize & orc_Size, const bool oq_TornOffWindowMinimized,
                                                 const bool oq_TornOffWindowMaximized)
{
   if (this->mc_Dashboards.contains(orc_DashboardName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewDashboard & rc_Dashboard = this->mc_Dashboards.operator [](orc_DashboardName);
      rc_Dashboard.q_TornOff = true;
      rc_Dashboard.c_TornOffWindowPosition = orc_Position;
      rc_Dashboard.c_TornOffWindowSize = orc_Size;
      rc_Dashboard.q_TornOffWindowMaximized = oq_TornOffWindowMaximized;
      rc_Dashboard.q_TornOffWindowMinimized = oq_TornOffWindowMinimized;
   }
   else
   {
      C_UsSystemViewDashboard c_DashboardSettings;
      c_DashboardSettings.q_TornOff = true;
      c_DashboardSettings.c_TornOffWindowPosition = orc_Position;
      c_DashboardSettings.c_TornOffWindowSize = orc_Size;
      c_DashboardSettings.q_TornOffWindowMaximized = oq_TornOffWindowMaximized;
      c_DashboardSettings.q_TornOffWindowMinimized = oq_TornOffWindowMinimized;
      this->mc_Dashboards.insert(orc_DashboardName, c_DashboardSettings);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard scene position and zoom

   \param[in]  orc_DashboardName    Dashboard name (identifier)
   \param[in]  orc_Position         Scene position
   \param[in]  osn_Zoom             Scene zoom
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetProjSvDashboardScenePositionAndZoom(const QString & orc_DashboardName,
                                                            const QPoint & orc_Position, const sintn osn_Zoom)
{
   if (this->mc_Dashboards.contains(orc_DashboardName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewDashboard & rc_Dashboard = this->mc_Dashboards.operator [](orc_DashboardName);
      rc_Dashboard.c_ScenePos = orc_Position;
      rc_Dashboard.sn_SceneZoom = osn_Zoom;
   }
   else
   {
      C_UsSystemViewDashboard c_DashboardSettings;
      c_DashboardSettings.c_ScenePos = orc_Position;
      c_DashboardSettings.sn_SceneZoom = osn_Zoom;
      this->mc_Dashboards.insert(orc_DashboardName, c_DashboardSettings);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get restored dashboard user settings

   \param[in]  orc_DashboardName    Dashboard name (identifier)

   \return
   If not torn off and tab index smaller than 0: invalid
   Else valid restored dashboard user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemViewDashboard C_UsSystemView::GetDashboardSettings(const QString & orc_DashboardName) const
{
   return this->mc_Dashboards.value(orc_DashboardName, C_UsSystemViewDashboard());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard keys internal structure

   \return
   Dashboard keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsSystemView::GetDashboardKeysInternal(void) const
{
   return this->mc_Dashboards.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard toolbox position

   \return
   Current dashboard toolbox position
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_UsSystemView::GetDashboardToolboxPos(void) const
{
   return this->mc_DashboardToolboxPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard toolbox position

   \param[in]  orc_Value   New dashboard toolbox position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxPos(const QPoint & orc_Value)
{
   this->mc_DashboardToolboxPos = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard toolbox size

   \return
   Current dashboard toolbox size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_UsSystemView::GetDashboardToolboxSize(void) const
{
   return this->mc_DashboardToolboxSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard toolbox size

   \param[in]  orc_Value   New dashboard toolbox size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxSize(const QSize & orc_Value)
{
   this->mc_DashboardToolboxSize = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard toolbox maximized flag

   \return
   Current dashboard toolbox maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetDashboardToolboxMaximized(void) const
{
   return this->mq_DashboardToolboxMaximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard toolbox maximized flag

   \param[in]  oq_Value    New dashboard toolbox maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxMaximized(const bool oq_Value)
{
   this->mq_DashboardToolboxMaximized = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard selected tab index

   \return
   Current dashboard selected tab index
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_UsSystemView::GetDashboardSelectedTabIndex(void) const
{
   return this->ms32_DashboardSelectedTabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard selected tab index

   \param[in]  os32_Value   New dashboard selected tab index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardSelectedTabIndex(const stw_types::sint32 os32_Value)
{
   this->ms32_DashboardSelectedTabIndex = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get setup permission

   \return
   Current setup permission
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetSetupPermission() const
{
   return this->mq_SetupPermission;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Set setup permission

   \param[in]  oq_Value   New setup permission
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetSetupPermission(const bool oq_Value)
{
   this->mq_SetupPermission = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update permission

   \return
   Current update permission
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetUpdatePermission() const
{
   return this->mq_UpdatePermission;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Set update permission

   \param[in]  oq_Value   New update permission
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetUpdatePermission(const bool oq_Value)
{
   this->mq_UpdatePermission = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get dashboard permission

   \return
   Current dashboard permission
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsSystemView::GetDashboardPermission() const
{
   return this->mq_DashboardPermission;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief  Set dashboard permission

   \param[in]  oq_Value   New dashboard permission
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemView::SetDashboardPermission(const bool oq_Value)
{
   this->mq_DashboardPermission = oq_Value;
}

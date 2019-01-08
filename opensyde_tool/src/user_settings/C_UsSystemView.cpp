//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Project system view user settings (implementation)

   Project system view user settings

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_UsSystemView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsSystemView::C_UsSystemView(void) :
   sn_SetupViewZoom(100),
   c_SetupViewPos(0, 0),
   sn_UpdateViewZoom(100),
   c_UpdateViewPos(0, 0),
   c_NodeInfoPos(20, 30),
   c_NodeInfoSize(1300, 1000),
   q_NodeInfoMaximized(false),
   mq_NavigationExpanded(false),
   ms32_UpdateSplitterX(-1),
   mc_UpdateProgressLogPos(-1, -1),
   mc_UpdateProgressLogSize(600, 400),
   mq_UpdateProgressLogMaximized(true),
   mc_DashboardToolboxPos(-1, -1),
   mc_DashboardToolboxSize(600, 400),
   mq_DashboardToolboxMaximized(true),
   ms32_DashboardSelectedTabIndex(-1)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] osn_SetupViewZoom Zoom init value
   \param[in] orc_SetupViewPos  Position init value

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsSystemView::C_UsSystemView(const sintn osn_SetupViewZoom, const QPoint & orc_SetupViewPos) :
   sn_SetupViewZoom(osn_SetupViewZoom),
   c_SetupViewPos(orc_SetupViewPos),
   sn_UpdateViewZoom(100),
   c_UpdateViewPos(0, 0),
   c_NodeInfoPos(20, 30),
   c_NodeInfoSize(1300, 1000),
   q_NodeInfoMaximized(false),
   mq_NavigationExpanded(false),
   ms32_UpdateSplitterX(-1),
   mc_UpdateProgressLogPos(-1, -1),
   mc_UpdateProgressLogSize(600, 400),
   mq_UpdateProgressLogMaximized(true),
   mc_DashboardToolboxPos(-1, -1),
   mc_DashboardToolboxSize(600, 400),
   mq_DashboardToolboxMaximized(true),
   ms32_DashboardSelectedTabIndex(-1)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the size of the read only info widget of nodes

   \param[in]     orc_Size       Actual size of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetSetupNodeInfoSize(const QSize & orc_Size)
{
   this->c_NodeInfoSize = orc_Size;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the size of the read only info widget of nodes

   \return
   Actual size of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QSize C_UsSystemView::GetSetupNodeInfoSize(void) const
{
   return this->c_NodeInfoSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the position of the read only info widget of nodes

   \param[in]     orc_Pos        Actual pos of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetSetupNodeInfoPos(const QPoint & orc_Pos)
{
   this->c_NodeInfoPos = orc_Pos;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the position of the read only info widget of nodes

   \return
   Actual pos of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QPoint C_UsSystemView::GetSetupNodeInfoPos(void) const
{
   return this->c_NodeInfoPos;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the maximized flag of the read only info widget of nodes

   \param[in]     oq_Maximized        Actual maximized flag of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetSetupNodeInfoMaximized(const bool oq_Maximized)
{
   this->q_NodeInfoMaximized = oq_Maximized;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the maximized flag of the read only info widget of nodes

   \return
   Actual maximized flag of node info widget

   \created     22.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_UsSystemView::GetSetupNodeInfoMaximized(void) const
{
   return this->q_NodeInfoMaximized;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard integrated tab index

   \param[in] orc_DashboardName Dashboard name (identifier)
   \param[in] os32_TabIndex     Current tab index

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetDashboardMainTab(const QString & orc_DashboardName, const sint32 os32_TabIndex)
{
   if (this->mc_Dashboards.contains(orc_DashboardName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemViewDashboard & rc_Dashboard = this->mc_Dashboards.operator [](orc_DashboardName);
      rc_Dashboard.q_TornOff = false;
      rc_Dashboard.s32_MainWindowTabIndex = os32_TabIndex;
   }
   else
   {
      C_UsSystemViewDashboard c_DashboardSettings;
      c_DashboardSettings.q_TornOff = false;
      c_DashboardSettings.s32_MainWindowTabIndex = os32_TabIndex;
      this->mc_Dashboards.insert(orc_DashboardName, c_DashboardSettings);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard torn off window data

   \param[in] orc_DashboardName         Dashboard name (identifier)
   \param[in] orc_Position              Torn off window position
   \param[in] orc_Size                  Torn off window size
   \param[in] oq_TornOffWindowMinimized Torn off window minimized flag
   \param[in] oq_TornOffWindowMaximized Torn off window maximized flag

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard scene position and zoom

   \param[in] orc_DashboardName Dashboard name (identifier)
   \param[in] orc_Position      Scene position
   \param[in] osn_Zoom          Scene zoom

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get restored dashboard user settings

   \param[in] orc_DashboardName Dashboard name (identifier)

   \return
   If not torn off and tab index smaller than 0: invalid
   Else valid restored dashboard user settings

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsSystemViewDashboard C_UsSystemView::GetDashboardSettings(const QString & orc_DashboardName) const
{
   return this->mc_Dashboards.value(orc_DashboardName, C_UsSystemViewDashboard());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get dashboard keys internal structure

   \return
   Dashboard keys internal structure

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QList<QString> C_UsSystemView::GetDashboardKeysInternal(void) const
{
   return this->mc_Dashboards.keys();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get update data rate

   \return
   Current update data rate

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QMap<uint32, uint64> & C_UsSystemView::GetUpdateDataRateHistory(void) const
{
   return this->mc_UpdateDataRateHistory;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add update data rate

   \param[in] os32_Checksum          System setup checksum
   \param[in] ou64_DataRateBytesPerS Data bytes per milli seconds

   \created     26.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::AddUpdateDataRate(const uint32 ou32_Checksum, const uint64 ou64_DataRateBytesPerS)
{
   //Clean up last entries
   QMap<uint32, uint64>::iterator c_FoundEntry = this->mc_UpdateDataRateHistory.find(ou32_Checksum);
   if (c_FoundEntry != this->mc_UpdateDataRateHistory.end())
   {
      this->mc_UpdateDataRateHistory.erase(c_FoundEntry);
   }
   //Insert new one
   this->mc_UpdateDataRateHistory.insert(ou32_Checksum, ou64_DataRateBytesPerS);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get update splitter X value

   \return
   Current update splitter X value

   \created     28.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_UsSystemView::GetUpdateSplitterX(void) const
{
   return this->ms32_UpdateSplitterX;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set update splitter X value

   \param[in] os32_Value New update splitter X value

   \created     28.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetUpdateSplitterX(const sint32 os32_Value)
{
   this->ms32_UpdateSplitterX = os32_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get update progress log position

   \return
   Current update progress log position

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QPoint C_UsSystemView::GetUpdateProgressLogPos(void) const
{
   return this->mc_UpdateProgressLogPos;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set update progress log position

   \param[in] orc_Value New update progress log position

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogPos(const QPoint & orc_Value)
{
   this->mc_UpdateProgressLogPos = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get update progress log size

   \return
   Current update progress log size

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_UsSystemView::GetUpdateProgressLogSize(void) const
{
   return this->mc_UpdateProgressLogSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set update progress log size

   \param[in] orc_Value New update progress log size

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogSize(const QSize & orc_Value)
{
   this->mc_UpdateProgressLogSize = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get update progress log maximized flag

   \return
   Current update progress log maximized flag

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_UsSystemView::GetUpdateProgressLogMaximized(void) const
{
   return this->mq_UpdateProgressLogMaximized;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set update progress log maximized flag

   \param[in] orc_Value New update progress log maximized flag

   \created     15.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetUpdateProgressLogMaximized(const bool oq_Value)
{
   this->mq_UpdateProgressLogMaximized = oq_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the status for the navigation expansion state

   \param[in] oq_NavigationExpandedStatus New navigation expansion state

   \created     08.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetNavigationExpandedStatus(const bool oq_NavigationExpandedStatus)
{
   this->mq_NavigationExpanded = oq_NavigationExpandedStatus;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get the status for the navigation expansion state

   \return
   The status for the navigation expansion state

   \created     08.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_UsSystemView::GetNavigationExpandedStatus(void) const
{
   return this->mq_NavigationExpanded;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get dashboard toolbox position

   \return
   Current dashboard toolbox position

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QPoint C_UsSystemView::GetDashboardToolboxPos(void) const
{
   return this->mc_DashboardToolboxPos;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard toolbox position

   \param[in] orc_Value New dashboard toolbox position

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxPos(const QPoint & orc_Value)
{
   this->mc_DashboardToolboxPos = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get dashboard toolbox size

   \return
   Current dashboard toolbox size

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_UsSystemView::GetDashboardToolboxSize(void) const
{
   return this->mc_DashboardToolboxSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard toolbox size

   \param[in] orc_Value New dashboard toolbox size

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxSize(const QSize & orc_Value)
{
   this->mc_DashboardToolboxSize = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get dashboard toolbox maximized flag

   \return
   Current dashboard toolbox maximized flag

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_UsSystemView::GetDashboardToolboxMaximized(void) const
{
   return this->mq_DashboardToolboxMaximized;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard toolbox maximized flag

   \param[in] orc_Value New dashboard toolbox maximized flag

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetDashboardToolboxMaximized(const bool oq_Value)
{
   this->mq_DashboardToolboxMaximized = oq_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get dashboard selected tab index

   \return
   Current dashboard selected tab index

   \created     22.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::sint32 C_UsSystemView::GetDashboardSelectedTabIndex(void) const
{
   return this->ms32_DashboardSelectedTabIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard selected tab index

   \param[in] orc_Value New dashboard selected tab index

   \created     22.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsSystemView::SetDashboardSelectedTabIndex(const stw_types::sint32 & orc_Value)
{
   this->ms32_DashboardSelectedTabIndex = orc_Value;
}

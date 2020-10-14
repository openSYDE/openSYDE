//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view data element (implementation)

   System view data element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMap>
#include <QDir>
#include "C_Uti.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_PuiUtil.h"
#include "C_PuiSvData.h"
#include "C_PuiSdUtil.h"
#include "CSCLChecksums.h"
#include "C_PuiSdHandler.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
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
C_PuiSvData::C_PuiSvData(void) :
   mc_Name("NewView"),
   mu16_UpdateRateFast(100U),
   mu16_UpdateRateMedium(500U),
   mu16_UpdateRateSlow(1000U),
   mu32_DeviceConfigSelectedBitRate(125U),
   me_DeviceConfigMode(eDCM_ALL_CONNECTED_INTERFACES),
   mq_DarkModeActive(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_Name.toStdString().c_str(), this->mc_Name.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu32_DeviceConfigSelectedBitRate,
                                      sizeof(this->mu32_DeviceConfigSelectedBitRate), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_DeviceConfigMode, sizeof(this->me_DeviceConfigMode), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mq_DarkModeActive, sizeof(this->mq_DarkModeActive), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu16_UpdateRateFast, sizeof(this->mu16_UpdateRateFast), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu16_UpdateRateMedium, sizeof(this->mu16_UpdateRateMedium),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu16_UpdateRateSlow, sizeof(this->mu16_UpdateRateSlow), oru32_HashValue);
   this->mc_PcData.CalcHash(oru32_HashValue);
   for (uint32 u32_ItUpdate = 0; u32_ItUpdate < this->mc_NodeUpdateInformation.size(); ++u32_ItUpdate)
   {
      const C_PuiSvNodeUpdate & rc_Update = this->mc_NodeUpdateInformation[u32_ItUpdate];
      rc_Update.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_NodeActiveFlags.size(); ++u32_ItNode)
   {
      const bool q_Data = static_cast<bool>(this->mc_NodeActiveFlags[u32_ItNode]);
      stw_scl::C_SCLChecksums::CalcCRC32(&q_Data, sizeof(q_Data), oru32_HashValue);
   }
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Data = this->mc_Dashboards[u32_ItDashboard];
      rc_Data.CalcHash(oru32_HashValue);
   }
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_It =
           this->mc_ReadRailAssignments.begin(); c_It != this->mc_ReadRailAssignments.end(); ++c_It)
   {
      c_It.key().CalcHash(oru32_HashValue);
      c_It.value().CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over setup data

   The hash value is a 32 bit CRC value.

   \return
   Hash value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvData::CalcUpdateHash(void) const
{
   uint32 u32_Retval = 0;

   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_Name.toStdString().c_str(), this->mc_Name.length(), u32_Retval);
   this->mc_PcData.CalcHash(u32_Retval);
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_NodeActiveFlags.size(); ++u32_ItNode)
   {
      const bool q_Data = static_cast<bool>(this->mc_NodeActiveFlags[u32_ItNode]);
      stw_scl::C_SCLChecksums::CalcCRC32(&q_Data, sizeof(q_Data), u32_Retval);
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get PC data

   \return
   Current PC data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvPc & C_PuiSvData::GetPcData(void) const
{
   return this->mc_PcData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set PC data

   \param[in]  orc_Value   New PC data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPcData(const C_PuiSvPc & orc_Value)
{
   this->mc_PcData = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node active flags

   \return
   Current node active flags
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8> & C_PuiSvData::GetNodeActiveFlags(void) const
{
   return this->mc_NodeActiveFlags;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node active flags

   \param[in]  orc_Value   New node active flags
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetNodeActiveFlags(const std::vector<uint8> & orc_Value)
{
   this->mc_NodeActiveFlags = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node update information

   \return
   Current node update information
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvNodeUpdate> & C_PuiSvData::GetAllNodeUpdateInformation(void) const
{
   return this->mc_NodeUpdateInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  orc_NodeUpdateInformation  New node update information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation)
{
   this->mc_NodeUpdateInformation = orc_NodeUpdateInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node update information

   \param[in]  ou32_NodeIndex    Node index

   \return
   NULL Update information not found
   Else Valid update information
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvNodeUpdate * C_PuiSvData::GetNodeUpdateInformation(const uint32 ou32_NodeIndex) const
{
   const C_PuiSvNodeUpdate * pc_Retval = NULL;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      pc_Retval = &this->mc_NodeUpdateInformation[ou32_NodeIndex];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR    No error
   C_RANGE     Node index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetNodeUpdateInformation(const uint32 ou32_NodeIndex,
                                             const C_PuiSvNodeUpdate & orc_NodeUpdateInformation)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      this->mc_NodeUpdateInformation[ou32_NodeIndex] = orc_NodeUpdateInformation;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetNodeUpdateInformationPath(const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                 const QString & orc_Value,
                                                 const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetPath(ou32_Index, orc_Value, oe_Type);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetNodeUpdateInformationParamInfo(const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                      const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetParamInfo(ou32_Index, orc_Value);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_Index           Index to access
   \param[in]  orc_FilePath         New path
   \param[in]  ou32_LastKnownCrc    Last known CRC for this file

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetNodeUpdateInformationParamInfoContent(const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                             const QString & orc_FilePath,
                                                             const uint32 ou32_LastKnownCrc)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.SetParamInfoContent(ou32_Index, orc_FilePath, ou32_LastKnownCrc);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddNodeUpdateInformationPath(const uint32 ou32_NodeIndex, const QString & orc_Value,
                                                 const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.AddPath(orc_Value, oe_Type);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddNodeUpdateInformationParamInfo(const uint32 ou32_NodeIndex,
                                                      const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.AddParamInfo(orc_Value);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name

   \return
   Current name
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_PuiSvData::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set name

   \param[in]  orc_Value   New name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetName(const QString & orc_Value)
{
   this->mc_Name = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboards

   \return
   Current dashboards
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDashboard> & C_PuiSvData::GetDashboards(void) const
{
   return this->mc_Dashboards;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard

   \param[in]  ou32_Index  Dashboard index

   \return
   NULL Not found
   Else Requested dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDashboard * C_PuiSvData::GetDashboard(const uint32 ou32_Index) const
{
   const C_PuiSvDashboard * pc_Retval = NULL;

   if (ou32_Index < this->mc_Dashboards.size())
   {
      pc_Retval = &this->mc_Dashboards[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboards

   \param[in]  orc_Value   New dashboards
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetDashboards(const std::vector<C_PuiSvDashboard> & orc_Value)
{
   this->mc_Dashboards = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get fast update rate

   \return
   Current fast update rate
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint16 C_PuiSvData::GetUpdateRateFast(void) const
{
   return this->mu16_UpdateRateFast;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set fast update rate

   \param[in]  ou16_Value  New fast update rate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetUpdateRateFast(const uint16 ou16_Value)
{
   this->mu16_UpdateRateFast = ou16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get medium update rate

   \return
   Current medium update rate
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint16 C_PuiSvData::GetUpdateRateMedium(void) const
{
   return this->mu16_UpdateRateMedium;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set medium update rate

   \param[in]  ou16_Value  New medium update rate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetUpdateRateMedium(const uint16 ou16_Value)
{
   this->mu16_UpdateRateMedium = ou16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get slow update rate

   \return
   Current slow update rate
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint16 C_PuiSvData::GetUpdateRateSlow(void) const
{
   return this->mu16_UpdateRateSlow;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set slow update rate

   \param[in]  ou16_Value  New slow update rate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetUpdateRateSlow(const uint16 ou16_Value)
{
   this->mu16_UpdateRateSlow = ou16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set slow update rate

   \param[in]   ou8_RailIndex    Number of rail: 0: fast, 1: medium, 2: slow, else: unexpected
   \param[out]  oru16_Value      Update rate of rail

   \return
   C_NO_ERR    No error
   C_RANGE     Rail index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::GetUpdateRate(const uint8 ou8_RailIndex, uint16 & oru16_Value) const
{
   sint32 s32_Return = C_RANGE;

   if (ou8_RailIndex <= 2)
   {
      switch (ou8_RailIndex)
      {
      case 0:
         oru16_Value = this->GetUpdateRateFast();
         break;
      case 1:
         oru16_Value = this->GetUpdateRateMedium();
         break;
      case 2:
         oru16_Value = this->GetUpdateRateSlow();
         break;
      default:
         break;
      }

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set device config screen selected bit rate

   \param[in]  ou32_Value  New device config screen selected bit rate in kbit/s
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetDeviceConfigSelectedBitRate(const uint32 ou32_Value)
{
   this->mu32_DeviceConfigSelectedBitRate = ou32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get device config screen selected mode

   \return
   Current device config screen selected mode
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvData::GetDeviceConfigSelectedBitRate(void) const
{
   return this->mu32_DeviceConfigSelectedBitRate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set device config screen selected mode

   \param[in]  oe_Value    New device config screen selected mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetDeviceConfigMode(const C_PuiSvData::E_DeviceConfigurationMode oe_Value)
{
   this->me_DeviceConfigMode = oe_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get device config screen selected bit rate

   \return
   Current device config screen selected bit rate
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvData::E_DeviceConfigurationMode C_PuiSvData::GetDeviceConfigMode(void) const
{
   return this->me_DeviceConfigMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dark mode active flag

   \return
   Current dark mode active flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvData::GetDarkModeActive(void) const
{
   return this->mq_DarkModeActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active flag

   \param[in]  oq_Value    New dark mode active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetDarkModeActive(const bool oq_Value)
{
   this->mq_DarkModeActive = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get read data elements rail assignments

   \return
   Current read data elements rail assignments
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<C_OSCNodeDataPoolListElementId,
           C_PuiSvReadDataConfiguration> & C_PuiSvData::GetReadRailAssignments(void) const
{
   return this->mc_ReadRailAssignments;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all elements assigned to at least on write widget

   \return
   Set of element ids with all write elements
*/
//----------------------------------------------------------------------------------------------------------------------
const std::set<C_OSCNodeDataPoolListElementId> C_PuiSvData::GetWriteAssignments(void) const
{
   std::set<C_OSCNodeDataPoolListElementId> c_WriteElements;
   uint32 u32_DashboardCounter;

   for (u32_DashboardCounter = 0U; u32_DashboardCounter < this->mc_Dashboards.size(); ++u32_DashboardCounter)
   {
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      this->mc_Dashboards[u32_DashboardCounter].GetAllWidgetItems(c_Widgets);
      uint32 u32_WidgetCounter;

      for (u32_WidgetCounter = 0U; u32_WidgetCounter < c_Widgets.size(); ++u32_WidgetCounter)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_WidgetCounter];

         if ((pc_Widget != NULL) &&
             (pc_Widget->IsReadElement() == false))
         {
            uint32 u32_ElementConfig;

            for (u32_ElementConfig = 0U; u32_ElementConfig < pc_Widget->c_DataPoolElementsConfig.size();
                 ++u32_ElementConfig)
            {
               c_WriteElements.insert(pc_Widget->c_DataPoolElementsConfig[u32_ElementConfig].c_ElementId);
            }
         }
      }
   }

   return c_WriteElements;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a specific rail assignment

   \param[in]   orc_Id        ID
   \param[out]  orc_Config    Current read data element rail assignment of specific element

   \return
   C_NO_ERR    Config found
   C_RANGE     No config for element ID found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::GetReadRailAssignment(const C_OSCNodeDataPoolListElementId & orc_Id,
                                          C_PuiSvReadDataConfiguration & orc_Config) const
{
   sint32 s32_Return = C_RANGE;

   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_ItItem;

   c_ItItem = this->mc_ReadRailAssignments.find(orc_Id);

   if (c_ItItem != this->mc_ReadRailAssignments.end())
   {
      orc_Config = c_ItItem.value();
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set read data elements rail assignments

   \param[in]  orc_Value   New read data elements rail assignments
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetReadRailAssignments(const QMap<C_OSCNodeDataPoolListElementId,
                                                    C_PuiSvReadDataConfiguration> & orc_Value)
{
   this->mc_ReadRailAssignments = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set read rail assignment

   \param[in]  orc_Id      ID
   \param[in]  orc_Config  Config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetReadRailAssignment(const C_OSCNodeDataPoolListElementId & orc_Id,
                                          const C_PuiSvReadDataConfiguration & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mc_ReadRailAssignments.remove(orc_Id) != 1)
   {
      s32_Retval = C_RANGE;
   }
   this->mc_ReadRailAssignments.insert(orc_Id, orc_Config);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get relevant read rail assignments

   \param[in]   orc_Dashboard    Dashboard to analyze
   \param[out]  orc_Rails        Relevant rails
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::GetRelevantReadRailAssigmentsForDashboard(const C_PuiSvDashboard & orc_Dashboard,
                                                            QMap<C_OSCNodeDataPoolListElementId,
                                                                 C_PuiSvReadDataConfiguration> & orc_Rails)
const
{
   std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
   orc_Dashboard.GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0UL; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbWidgetBase * const pc_CurWidget = c_Widgets[u32_ItWidget];
      if ((pc_CurWidget != NULL) && (pc_CurWidget->IsReadElement() == true))
      {
         for (uint32 u32_ItConfig = 0UL; u32_ItConfig < pc_CurWidget->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_CurWidget->c_DataPoolElementsConfig[u32_ItConfig];
            const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                       C_PuiSvReadDataConfiguration>::const_iterator c_It =
               this->mc_ReadRailAssignments.find(rc_Config.c_ElementId);
            if (c_It != this->mc_ReadRailAssignments.end())
            {
               orc_Rails.insert(c_It.key(), c_It.value());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard name

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Name             New dashboard name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardName(const uint32 ou32_DashboardIndex, const QString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.SetName(orc_Name);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard comment

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Comment          New dashboard comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardComment(const uint32 ou32_DashboardIndex, const QString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.SetComment(orc_Comment);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard active flag

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oq_Active            New active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardActive(const uint32 ou32_DashboardIndex, const bool oq_Active)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.SetActive(oq_Active);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current tab index

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  os32_Value           New current tab index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardTabIndex(const uint32 ou32_DashboardIndex, const sint32 os32_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.SetTabIndex(os32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard widget box

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardWidget(const uint32 ou32_DashboardIndex, const uint32 ou32_WidgetIndex,
                                       const C_PuiSvDbWidgetBase * const opc_Box,
                                       const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.SetWidget(ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardBoundary(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                         const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.SetBoundary(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardImage(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                      const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.SetImage(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardLineArrow(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                          const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.SetLineArrow(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetDashboardTextElement(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                            const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.SetTextElement(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to added node index

   \param[in]  ou32_Index  Added node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeAdded(const uint32 ou32_Index)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   if (ou32_Index <= this->mc_NodeActiveFlags.size())
   {
      this->mc_NodeActiveFlags.insert(this->mc_NodeActiveFlags.begin() + ou32_Index, static_cast<uint8>(false));
   }
   if (ou32_Index <= this->mc_NodeUpdateInformation.size())
   {
      uint32 u32_NodeUpdateCounter;
      uint32 u32_Position = 0U;
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_Index);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         C_PuiSvNodeUpdate c_NodeUpdate;

         // Search the highest node update position and add the new node to the end
         for (u32_NodeUpdateCounter = 0U; u32_NodeUpdateCounter < this->mc_NodeUpdateInformation.size();
              ++u32_NodeUpdateCounter)
         {
            if (u32_Position <= this->mc_NodeUpdateInformation[u32_NodeUpdateCounter].u32_NodeUpdatePosition)
            {
               u32_Position = this->mc_NodeUpdateInformation[u32_NodeUpdateCounter].u32_NodeUpdatePosition + 1U;
            }
         }

         c_NodeUpdate.u32_NodeUpdatePosition = u32_Position;

         this->mc_NodeUpdateInformation.insert(this->mc_NodeUpdateInformation.begin() + ou32_Index, c_NodeUpdate);

         //Also handle node applications (could not be synced up to this point) AFTER node update information was added
         for (uint32 u32_ItApplication = 0UL; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
         {
            OnSyncNodeApplicationAdded(ou32_Index, u32_ItApplication);
         }
      }
   }
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeAdded(ou32_Index);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeAdded(c_Id, ou32_Index);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sync view node indices to HALC change

   \param[in]  ou32_Index        Index
   \param[in]  orc_MapCurToNew   Map cur to new
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeHALC(const uint32 ou32_Index, const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                         C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeHALC(ou32_Index, orc_MapCurToNew);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      if (ou32_Index == c_ItReadItem.key().u32_NodeIndex)
      {
         C_OSCNodeDataPool::E_Type e_Type;
         if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(c_ItReadItem.key().u32_NodeIndex,
                                                              c_ItReadItem.key().u32_DataPoolIndex, e_Type) == C_NO_ERR)
         {
            if (e_Type == C_OSCNodeDataPool::eHALC)
            {
               //Manual compare because only base is necessary
               for (std::map<C_PuiSvDbNodeDataPoolListElementId,
                             C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_ItMap =
                       orc_MapCurToNew.begin();
                    c_ItMap != orc_MapCurToNew.end(); ++c_ItMap)
               {
                  if ((((c_ItMap->first.u32_NodeIndex == c_ItReadItem.key().u32_NodeIndex) &&
                        (c_ItMap->first.u32_DataPoolIndex == c_ItReadItem.key().u32_DataPoolIndex)) &&
                       (c_ItMap->first.u32_ListIndex == c_ItReadItem.key().u32_ListIndex)) &&
                      (c_ItMap->first.u32_ElementIndex == c_ItReadItem.key().u32_ElementIndex))
                  {
                     c_NewItems.insert(c_ItMap->second, c_ItReadItem.value());
                     break;
                  }
               }
            }
            else
            {
               c_NewItems.insert(c_ItReadItem.key(), c_ItReadItem.value());
            }
         }
         else
         {
            c_NewItems.insert(c_ItReadItem.key(), c_ItReadItem.value());
         }
      }
      else
      {
         c_NewItems.insert(c_ItReadItem.key(), c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to deleted node index

   \param[in]  ou32_Index  Deleted node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeAboutToBeDeleted(const uint32 ou32_Index)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   if (ou32_Index < this->mc_NodeActiveFlags.size())
   {
      this->mc_NodeActiveFlags.erase(this->mc_NodeActiveFlags.begin() + ou32_Index);
   }
   if (ou32_Index < this->mc_NodeUpdateInformation.size())
   {
      this->mc_NodeUpdateInformation.erase(this->mc_NodeUpdateInformation.begin() + ou32_Index);
   }
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeAboutToBeDeleted(ou32_Index);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeAboutToBeDeleted(c_Id, ou32_Index);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncBusAdded(const uint32 ou32_Index)
{
   this->mc_PcData.OnSyncBusAdded(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to deleted bus index

   \param[in]  ou32_Index  Deleted bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncBusDeleted(const uint32 ou32_Index)
{
   this->mc_PcData.OnSyncBusDeleted(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolAdded(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolSourceIndex   Source data pool index
   \param[in]  ou32_DataPoolTargetIndex   Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolSourceIndex,
                                          const uint32 ou32_DataPoolTargetIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_DataPoolSourceIndex, ou32_DataPoolTargetIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolMoved(c_Id, ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                                  ou32_DataPoolTargetIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolAboutToBeDeleted(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeApplicationAdded(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_Update = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_Update.OnSyncNodeApplicationAdded(ou32_ApplicationIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeApplicationMoved(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationSourceIndex,
                                             const uint32 ou32_ApplicationTargetIndex)
{
   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_Update = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_Update.OnSyncNodeApplicationMoved(ou32_ApplicationSourceIndex, ou32_ApplicationTargetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_Update = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_Update.OnSyncNodeApplicationAboutToBeDeleted(ou32_ApplicationIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                              const uint32 ou32_ListIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  ou32_ListSourceIndex    Source list index
   \param[in]  ou32_ListTargetIndex    Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                              const uint32 ou32_ListSourceIndex, const uint32 ou32_ListTargetIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                               ou32_ListTargetIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListMoved(c_Id, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                                      ou32_ListTargetIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                         const uint32 ou32_ListIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListAboutToBeDeleted(c_Id, ou32_NodeIndex, ou32_DataPoolIndex,
                                                                 ou32_ListIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListDataSetAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex, const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListDataSetAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_DataSetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_DataSetSourceIndex    Source data set index
   \param[in]  ou32_DataSetTargetIndex    Target data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListDataSetMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex, const uint32 ou32_DataSetSourceIndex,
                                                     const uint32 ou32_DataSetTargetIndex)
{
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListDataSetMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_DataSetSourceIndex, ou32_DataSetTargetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                const uint32 ou32_DataPoolIndex,
                                                                const uint32 ou32_ListIndex,
                                                                const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListDataSetAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                                 ou32_DataSetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListElementAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListElementAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_ElementIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                             ou32_ElementIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
   //Check param lists

   //For all param widgets
   {
      const std::vector<C_PuiSvDashboard> & rc_Dashboards = this->GetDashboards();
      for (uint32 u32_ItDashboard = 0; u32_ItDashboard < rc_Dashboards.size(); ++u32_ItDashboard)
      {
         const C_PuiSvDashboard & rc_Dashboard = rc_Dashboards[u32_ItDashboard];
         const std::vector<C_PuiSvDbParam> & rc_Params = rc_Dashboard.GetParams();
         for (uint32 u32_ItParam = 0; u32_ItParam < rc_Params.size(); ++u32_ItParam)
         {
            const C_PuiSvDbParam & rc_Param = rc_Params[u32_ItParam];
            bool q_Found = false;
            //For each list and value item
            tgl_assert(rc_Param.c_ListValues.size() == rc_Param.c_DataPoolElementsConfig.size());
            tgl_assert(rc_Param.c_DataSetSelectionIndices.size() == rc_Param.c_DataPoolElementsConfig.size());
            for (uint32 u32_ItElement = 0; u32_ItElement < rc_Param.c_DataPoolElementsConfig.size(); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_Param.c_DataPoolElementsConfig[u32_ItElement];
               //Check if the item was invalidated
               if ((((rc_Config.c_ElementId.GetIsValid() == true) &&
                     (rc_Config.c_ElementId.u32_NodeIndex == ou32_NodeIndex)) &&
                    (rc_Config.c_ElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
                   (rc_Config.c_ElementId.u32_ListIndex == ou32_ListIndex))
               {
                  q_Found = true;
               }
            }
            if (q_Found == true)
            {
               //Add new element
               tgl_assert(this->AddDashboardParamNewDataPoolElement(u32_ItDashboard, u32_ItParam,
                                                                    C_OSCNodeDataPoolListElementId(ou32_NodeIndex,
                                                                                                   ou32_DataPoolIndex,
                                                                                                   ou32_ListIndex,
                                                                                                   ou32_ElementIndex),
                                                                    NULL) ==
                          C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_ElementSourceIndex    Source element index
   \param[in]  ou32_ElementTargetIndex    Target element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListElementMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex, const uint32 ou32_ElementSourceIndex,
                                                     const uint32 ou32_ElementTargetIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListElementMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_ElementSourceIndex, ou32_ElementTargetIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementMoved(c_Id, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                             ou32_ElementSourceIndex, ou32_ElementTargetIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Type              New element type
   \param[in]  oq_IsArray           New array type
   \param[in]  ou32_ArraySize       New array size
   \param[in]  oq_IsString          Flag if new type is string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListElementArrayChanged(const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_DataPoolIndex,
                                                            const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex,
                                                            const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                            const bool oq_IsArray, const uint32 ou32_ArraySize,
                                                            const bool oq_IsString)
{
   bool q_Changed = false;

   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      if (rc_Dashboard.OnSyncNodeDataPoolListElementArrayChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                                 ou32_ElementIndex, oe_Type, oq_IsArray,
                                                                 ou32_ArraySize, oq_IsString) == true)
      {
         q_Changed = true;
      }
   }
   //Check if any changes
   if (q_Changed == true)
   {
      const C_OSCNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                ou32_ElementIndex);
      //Check if no longer used
      if (this->CheckReadUsage(c_Id) == false)
      {
         //This could fail in case of only write elements being changed
         this->RemoveReadRailItem(c_Id);
      }
   }
   //Detect critical change for rails
   if ((((oq_IsArray == true) || (oe_Type == C_OSCNodeDataPoolContent::eUINT64)) ||
        (oe_Type == C_OSCNodeDataPoolContent::eFLOAT64)) || (oe_Type == C_OSCNodeDataPoolContent::eSINT64))
   {
      const C_OSCNodeDataPoolListElementId c_TmpId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                   ou32_ElementIndex);
      QMap<C_OSCNodeDataPoolListElementId,
           C_PuiSvReadDataConfiguration>::iterator c_It = this->mc_ReadRailAssignments.find(c_TmpId);
      //Check if rail still exists (might have been removed earlier)
      if (c_It != this->mc_ReadRailAssignments.end())
      {
         //Check if valid rail type
         if (c_It.value().e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
         {
            C_PuiSvReadDataConfiguration c_NewConfig = c_It.value();
            //Reinsert this one
            this->mc_ReadRailAssignments.erase(c_It);
            c_NewConfig.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
            this->mc_ReadRailAssignments.insert(c_TmpId, c_NewConfig);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Access            New access type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListElementAccessChanged(const uint32 ou32_NodeIndex,
                                                             const uint32 ou32_DataPoolIndex,
                                                             const uint32 ou32_ListIndex,
                                                             const uint32 ou32_ElementIndex,
                                                             const C_OSCNodeDataPoolListElement::E_Access oe_Access)
{
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListElementAccessChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                              ou32_ElementIndex, oe_Access);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                const uint32 ou32_DataPoolIndex,
                                                                const uint32 ou32_ListIndex,
                                                                const uint32 ou32_ElementIndex)
{
   QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_NewItems;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      rc_Dashboard.OnSyncNodeDataPoolListElementAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                                 ou32_ElementIndex);
   }
   //Read rail assignments
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id = C_PuiSvDbNodeDataPoolListElementId(
         c_ItReadItem.key(), C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false, 0UL);
      C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementAboutToBeDeleted(c_Id, ou32_NodeIndex, ou32_DataPoolIndex,
                                                                        ou32_ListIndex, ou32_ElementIndex);
      if (c_Id.GetIsValid() == true)
      {
         //Add
         c_NewItems.insert(c_Id, c_ItReadItem.value());
      }
   }
   this->mc_ReadRailAssignments.clear();
   this->mc_ReadRailAssignments = c_NewItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked flag for node

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou8_Checked       Checked flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SetNodeCheckedState(const uint32 ou32_NodeIndex, const uint8 ou8_Checked)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_NodeActiveFlags.size())
   {
      this->mc_NodeActiveFlags[ou32_NodeIndex] = ou8_Checked;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set PC UI box

   \param[in]  orc_Box  New PC UI box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPCBox(const C_PuiBsBox & orc_Box)
{
   this->mc_PcData.SetBox(orc_Box);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set PC connection UI data

   \param[in]  orc_Line    New PC connection UI data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPCConnection(const C_PuiBsLineBase & orc_Line)
{
   this->mc_PcData.SetConnectionData(orc_Line);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  oq_Connected   Flag if pc is connected
   \param[in]  ou32_BusIndex  Bus index PC is connected to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPCConnected(const bool oq_Connected, const uint32 ou32_BusIndex)
{
   this->mc_PcData.SetConnected(oq_Connected, ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type of the CAN DLL

   \param[in]  oe_DllType  CAN DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPCCANDllType(const C_PuiSvPc::E_CANDllType oe_DllType)
{
   this->mc_PcData.SetCANDllType(oe_DllType);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set path for the CAN DLL

   \param[in]  orc_DllPath    Path for the CAN DLL
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::SetPCCANDllPath(const QString & orc_DllPath)
{
   this->mc_PcData.SetCustomCANDllPath(orc_DllPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new read rail item to configuration

   \param[in]  orc_Id      ID
   \param[in]  orc_Config  Configuration

   \return
   C_NO_ERR Operation success
   C_NOACT  Already exists
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddReadRailItem(const C_OSCNodeDataPoolListElementId & orc_Id,
                                    const C_PuiSvReadDataConfiguration & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mc_ReadRailAssignments.find(orc_Id) != this->mc_ReadRailAssignments.end())
   {
      s32_Retval = C_NOACT;
   }
   else
   {
      this->mc_ReadRailAssignments.insert(orc_Id, orc_Config);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add dashboard to view

   \param[in]  orc_Dashboard  Dashboard data
   \param[in]  oq_AutoAdapt   Auto adapt name flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::AddDashboard(const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt)
{
   tgl_assert(InsertDashboard(this->mc_Dashboards.size(), orc_Dashboard, oq_AutoAdapt) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert dashboard to view

   \param[in]  ou32_Index     Dashboard index
   \param[in]  orc_Dashboard  Dashboard data
   \param[in]  oq_AutoAdapt   Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboard(const uint32 ou32_Index, const C_PuiSvDashboard & orc_Dashboard,
                                    const bool oq_AutoAdapt)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index <= this->mc_Dashboards.size())
   {
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      C_PuiSvDashboard c_Copy = orc_Dashboard;

      if (oq_AutoAdapt == true)
      {
         c_Copy.SetName(C_Uti::h_GetUniqueNameQ(this->m_GetExistingDashboardNamesMap(), c_Copy.GetName()));
         c_Copy.SetComment(c_Copy.GetComment());
      }
      this->mc_Dashboards.insert(this->mc_Dashboards.begin() + ou32_Index, c_Copy);
      //Handle read rails
      c_Copy.GetAllWidgetItems(c_Widgets);
      for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
         if (pc_Widget != NULL)
         {
            for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                       C_PuiSvReadDataConfiguration>::const_iterator c_ItReadItem = this->mc_ReadRailAssignments.find(
                     rc_Config.c_ElementId);
                  if (c_ItReadItem == this->mc_ReadRailAssignments.end())
                  {
                     const C_OSCNodeDataPoolListElement * const pc_Element =
                        C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                                   rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                                   rc_Config.c_ElementId.u32_ListIndex,
                                                                                   rc_Config.c_ElementId.u32_ElementIndex);
                     if (pc_Element != NULL)
                     {
                        C_PuiSvReadDataConfiguration c_RailConfig;
                        if ((((pc_Element->GetArray() == true) ||
                              (pc_Element->GetType() == C_OSCNodeDataPoolContent::eUINT64)) ||
                             (pc_Element->GetType() == C_OSCNodeDataPoolContent::eSINT64)) ||
                            (pc_Element->GetType() == C_OSCNodeDataPoolContent::eFLOAT64))
                        {
                           c_RailConfig.u8_RailIndex = 0;
                           c_RailConfig.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
                        }
                        else
                        {
                           c_RailConfig.u8_RailIndex = 1;
                           c_RailConfig.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
                        }
                        c_RailConfig.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
                        this->AddReadRailItem(rc_Config.c_ElementId, c_RailConfig);
                     }
                  }
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add widget to view dashboard

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardWidget(const uint32 ou32_DashboardIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                       const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.AddWidget(opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert widget to view dashboard

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboardWidget(const uint32 ou32_DashboardIndex, const uint32 ou32_WidgetIndex,
                                          const C_PuiSvDbWidgetBase * const opc_Box,
                                          const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.InsertWidget(ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardBoundary(const uint32 ou32_DashboardIndex, const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.AddBoundary(orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboardBoundary(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                            const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.InsertBoundary(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardImage(const uint32 ou32_DashboardIndex, const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.AddImage(orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboardImage(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                         const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.InsertImage(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardLineArrow(const uint32 ou32_DashboardIndex, const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.AddLineArrow(orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboardLineArrow(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                             const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.InsertLineArrow(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardTextElement(const uint32 ou32_DashboardIndex, const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.AddTextElement(orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::InsertDashboardTextElement(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index,
                                               const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.InsertTextElement(oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Dashboard param widget clear all data pool elements

   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::ClearDashboardParamDataPoolElements(const uint32 ou32_DashboardIndex,
                                                        const uint32 ou32_ParamWidgetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      const C_PuiSvDbParam * const pc_Param = rc_Dashboard.GetParam(ou32_ParamWidgetIndex);
      if (pc_Param != NULL)
      {
         const std::vector<C_PuiSvDbNodeDataElementConfig> c_Configs = pc_Param->c_DataPoolElementsConfig;
         s32_Retval = rc_Dashboard.ClearParamDataPoolElements(ou32_ParamWidgetIndex);
         if (s32_Retval == C_NO_ERR)
         {
            //Clean up old read rails if necessary
            for (uint32 u32_ItPrevConfig = 0; (u32_ItPrevConfig < c_Configs.size()) && (s32_Retval == C_NO_ERR);
                 ++u32_ItPrevConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = c_Configs[u32_ItPrevConfig];
               if (rc_Config.c_ElementId.GetIsValid() == true)
               {
                  if (this->CheckReadUsage(rc_Config.c_ElementId) == false)
                  {
                     s32_Retval = this->RemoveReadRailItem(rc_Config.c_ElementId);
                  }
               }
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Dashboard param widget add new data pool element

   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index
   \param[in]  orc_NewId               New ID
   \param[in]  opc_Content             Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::AddDashboardParamNewDataPoolElement(const uint32 ou32_DashboardIndex,
                                                        const uint32 ou32_ParamWidgetIndex,
                                                        const C_OSCNodeDataPoolListElementId & orc_NewId,
                                                        const C_OSCNodeDataPoolContent * const opc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_NewId.u32_NodeIndex,
                                                                    orc_NewId.u32_DataPoolIndex,
                                                                    orc_NewId.u32_ListIndex,
                                                                    orc_NewId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
         s32_Retval = rc_Dashboard.AddParamNewDataPoolElement(ou32_ParamWidgetIndex, orc_NewId, opc_Content);
         if (s32_Retval == C_NO_ERR)
         {
            C_PuiSvReadDataConfiguration c_ReadRail;
            C_PuiSvReadDataConfiguration c_NewReadRail;
            //Handle read rail assignments
            c_NewReadRail.u8_RailIndex = 0;
            c_NewReadRail.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
            tgl_assert(c_NewReadRail.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue) ==
                       C_NO_ERR);
            if (this->GetReadRailAssignment(orc_NewId, c_ReadRail) != C_NO_ERR)
            {
               s32_Retval = this->AddReadRailItem(orc_NewId, c_NewReadRail);
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove read rail item configuration

   WARNING: No check if still in use

   \param[in]  orc_Id   ID

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::RemoveReadRailItem(const C_OSCNodeDataPoolListElementId & orc_Id)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mc_ReadRailAssignments.remove(orc_Id) == 0)
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information path

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::RemoveNodeUpdateInformationPath(const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                    const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.RemovePath(ou32_Index, oe_Type);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information parameter set information

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::RemoveNodeUpdateInformationParamInfo(const uint32 ou32_NodeIndex, const uint32 ou32_Index)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      s32_Return = rc_UpdateInformation.RemoveParamInfo(ou32_Index);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information paths as appropriate for the type

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::ClearNodeUpdateInformationAsAppropriate(const uint32 ou32_NodeIndex,
                                                            const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.ClearPathsAsAppropriate(oe_Type);

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information parameter set paths for this node

   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::ClearNodeUpdateInformationParamPaths(const uint32 ou32_NodeIndex)
{
   sint32 s32_Return = C_RANGE;

   if (ou32_NodeIndex < this->mc_NodeUpdateInformation.size())
   {
      C_PuiSvNodeUpdate & rc_UpdateInformation = this->mc_NodeUpdateInformation[ou32_NodeIndex];
      rc_UpdateInformation.ClearParamPaths();

      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete dashboard from view

   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboard(const uint32 ou32_DashboardIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      this->mc_Dashboards.erase(this->mc_Dashboards.begin() + ou32_DashboardIndex);
      //Clean up read rails
      m_CheckAllReadRails();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete widget from view dashboard

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboardWidget(const uint32 ou32_DashboardIndex, const uint32 ou32_WidgetIndex,
                                          const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.DeleteWidget(ou32_WidgetIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboardBoundary(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.DeleteBoundary(oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboardImage(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.DeleteImage(oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboardLineArrow(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.DeleteLineArrow(oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::DeleteDashboardTextElement(const uint32 ou32_DashboardIndex, const uint32 & oru32_Index)
{
   sint32 s32_Retval;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      s32_Retval = rc_Dashboard.DeleteTextElement(oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronise internally stored scaling information with current system definition

   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvData::SyncDashboardScalingInformation(const uint32 ou32_DashboardIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DashboardIndex < this->mc_Dashboards.size())
   {
      C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[ou32_DashboardIndex];
      rc_Dashboard.SyncScalingInformation();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix invalid rail configurations

   Hint: may be introduced by unknown issue not cleaning up rail configurations properly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::FixInvalidRailConfig(void)
{
   //Test all maps
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end();)
   {
      uint32 u32_Count = 0;
      for (uint32 u32_ItDashboard = 0; (u32_ItDashboard < this->mc_Dashboards.size()) && (u32_Count == 0UL);
           ++u32_ItDashboard)
      {
         const C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
         std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
         rc_Dashboard.GetAllWidgetItems(c_Widgets);
         //For each widget
         for (uint32 u32_ItWidget = 0; (u32_ItWidget < c_Widgets.size()) && (u32_Count == 0UL); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
            if ((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true))
            {
               //For each data element
               for (uint32 u32_ItDataElement = 0;
                    (u32_ItDataElement < pc_Widget->c_DataPoolElementsConfig.size()) && (u32_Count == 0UL);
                    ++u32_ItDataElement)
               {
                  const C_PuiSvDbNodeDataElementConfig & rc_Config =
                     pc_Widget->c_DataPoolElementsConfig[u32_ItDataElement];
                  //Check if match
                  if (rc_Config.c_ElementId == c_ItReadItem.key())
                  {
                     ++u32_Count;
                  }
               }
            }
         }
      }
      if (u32_Count == 0UL)
      {
         osc_write_log_warning("fix invalid rail config",
                               QString(
                                  "detected and removed unused rail (node-index: %1, datapool-index: %2, list-index: %3, element-index: %4)").arg(
                                  c_ItReadItem.key().u32_NodeIndex).arg(c_ItReadItem.key().u32_DataPoolIndex).arg(
                                  c_ItReadItem.key().
                                  u32_ListIndex).arg(
                                  c_ItReadItem.key().u32_ElementIndex).toStdString().c_str());
         //New item at current position
         c_ItReadItem = this->mc_ReadRailAssignments.erase(c_ItReadItem);
      }
      else
      {
         //Important iterator step
         ++c_ItReadItem;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from current system definition state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::InitFromSystemDefintion(void)
{
   //Nodes only
   this->mc_NodeActiveFlags.resize(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(), static_cast<uint8>(true));
   //handle node update info
   if (this->mc_NodeUpdateInformation.size() == 0UL)
   {
      this->mc_NodeUpdateInformation.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize());
      for (uint32 u32_ItInfo = 0; u32_ItInfo < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItInfo)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItInfo);
         if (pc_Node != NULL)
         {
            std::vector<QString> c_ApplPaths;
            C_PuiSvNodeUpdate c_Info;
            c_Info.u32_NodeUpdatePosition = u32_ItInfo;
            //Sync applications
            c_ApplPaths.reserve(pc_Node->c_Applications.size());
            for (uint32 u32_ItAppl = 0; u32_ItAppl < pc_Node->c_Applications.size(); ++u32_ItAppl)
            {
               const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItAppl];

               c_ApplPaths.push_back(C_PuiUtil::h_MakeIndependentOfDbProjectPath(rc_Application.c_ProjectPath.c_str(),
                                                                                 rc_Application.c_ResultPath.c_str()));
            }
            c_Info.SetPaths(c_ApplPaths, C_PuiSvNodeUpdate::eFTP_DATA_BLOCK);

            this->mc_NodeUpdateInformation.push_back(c_Info);
         }
      }
   }
   //Connect PC
   if (C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize() > 0)
   {
      uint32 u32_Counter;

      this->mc_PcData.SetConnected(true, 0);

      for (u32_Counter = 0U; u32_Counter < this->mc_NodeActiveFlags.size(); ++u32_Counter)
      {
         this->mc_NodeActiveFlags[u32_Counter] = true;
      }
   }
   else
   {
      this->mc_PcData.SetConnected(false, 0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check name not used in existing dashboards

   \param[in]  orc_Proposal            Proposal for item dashboard
   \param[in]  opu32_DashboardIndex    Optional parameter to skip one index
                                       (Use-case: skip current dashboards to avoid conflict with itself)

   \return
   True  Name available
   False Name already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvData::CheckDashboardName(const QString & orc_Proposal, const uint32 * const opu32_DashboardIndex) const
{
   return C_PuiSdUtil::h_CheckNameAvailable(this->m_GetExistingDashboardNames(), orc_Proposal, opu32_DashboardIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unique dashboard name based on proposal

   \param[in]  orc_Proposal   Application name proposal

   \return
   Unique dashboard name based on proposal
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvData::GetUniqueDashboardName(const QString & orc_Proposal) const
{
   return C_Uti::h_GetUniqueNameQ(this->m_GetExistingDashboardNamesMap(), orc_Proposal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if data element id in use

   \param[in]  orc_Id   Data element id to search for

   \return
   True  In use
   False No usage
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvData::CheckReadUsage(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   bool q_Retval = false;

   //For each dashboards
   for (uint32 u32_ItDashboard = 0; (u32_ItDashboard < this->mc_Dashboards.size()) && (q_Retval == false);
        ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      rc_Dashboard.GetAllWidgetItems(c_Widgets);
      //For each widget
      for (uint32 u32_ItWidget = 0; (u32_ItWidget < c_Widgets.size()) && (q_Retval == false); ++u32_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
         if ((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true))
         {
            //For each data element
            for (uint32 u32_ItDataElement = 0;
                 (u32_ItDataElement < pc_Widget->c_DataPoolElementsConfig.size()) && (q_Retval == false);
                 ++u32_ItDataElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config =
                  pc_Widget->c_DataPoolElementsConfig[u32_ItDataElement];
               //Check if match
               if (rc_Config.c_ElementId == orc_Id)
               {
                  q_Retval = true;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if data element id in use by any non param read item

   \param[in]  orc_Id   Data element id to search for

   \return
   True  In use
   False No usage
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvData::CheckNonParamReadUsage(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   bool q_Retval = false;

   //For each dashboards
   for (uint32 u32_ItDashboard = 0; (u32_ItDashboard < this->mc_Dashboards.size()) && (q_Retval == false);
        ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      rc_Dashboard.GetAllWidgetItems(c_Widgets);
      //For each widget
      for (uint32 u32_ItWidget = 0; (u32_ItWidget < c_Widgets.size()) && (q_Retval == false); ++u32_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         if (((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true)) &&
             (dynamic_cast<const C_PuiSvDbParam * const>(pc_Widget) == NULL))
         {
            //For each data element
            for (uint32 u32_ItDataElement = 0;
                 (u32_ItDataElement < pc_Widget->c_DataPoolElementsConfig.size()) && (q_Retval == false);
                 ++u32_ItDataElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config =
                  pc_Widget->c_DataPoolElementsConfig[u32_ItDataElement];
               //Check if match
               if (rc_Config.c_ElementId == orc_Id)
               {
                  q_Retval = true;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count number of usages for a data element id

   \param[in]  orc_Id   Data element id to search for

   \return
   Number of usages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvData::CountReadUsage(const C_OSCNodeDataPoolListElementId & orc_Id) const
{
   uint32 u32_Retval = 0;

   //For each dashboards
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      rc_Dashboard.GetAllWidgetItems(c_Widgets);
      //For each widget
      for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
         if ((pc_Widget != NULL) && (pc_Widget->IsReadElement() == true))
         {
            //For each data element
            for (uint32 u32_ItDataElement = 0; u32_ItDataElement < pc_Widget->c_DataPoolElementsConfig.size();
                 ++u32_ItDataElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config =
                  pc_Widget->c_DataPoolElementsConfig[u32_ItDataElement];
               //Check if match
               if (rc_Config.c_ElementId == orc_Id)
               {
                  ++u32_Retval;
               }
            }
         }
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of matching cyclic read transmissions

   \param[in]  ou32_NodeIndex    Node index

   \return
   Number of matching cyclic read transmissions
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvData::CountCyclicTransmissions(const uint32 ou32_NodeIndex) const
{
   uint32 u32_Retval = 0;

   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_ItReadItem =
           this->mc_ReadRailAssignments.begin();
        c_ItReadItem != this->mc_ReadRailAssignments.end(); ++c_ItReadItem)
   {
      if (c_ItReadItem.key().u32_NodeIndex == ou32_NodeIndex)
      {
         if (c_ItReadItem.value().e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
         {
            ++u32_Retval;
         }
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if list already in use by param widget

   \param[in]  orc_Id   Node data pool list ID

   \return
   True  Used
   False False unused
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvData::CheckNvmParamListUsage(const C_OSCNodeDataPoolListId & orc_Id) const
{
   bool q_Retval = false;

   //For each dashboards
   for (uint32 u32_ItDashboard = 0; (u32_ItDashboard < this->mc_Dashboards.size()) && (q_Retval == false);
        ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Dashboard = this->mc_Dashboards[u32_ItDashboard];
      const bool q_Usage = rc_Dashboard.CheckNvmParamListUsage(orc_Id);
      if (q_Usage == true)
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered dashboard names

   \return
   Vector of pointers to all currently registered dashboard names
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<const QString *> C_PuiSvData::m_GetExistingDashboardNames(void) const
{
   std::vector<const QString *> c_Retval;
   c_Retval.reserve(this->mc_Dashboards.size());
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Data = this->mc_Dashboards[u32_ItDashboard];
      c_Retval.push_back(&rc_Data.GetName());
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered dashboard names

   \return
   Vector of pointers to all currently registered dashboard names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<stw_scl::C_SCLString, bool> C_PuiSvData::m_GetExistingDashboardNamesMap(void) const
{
   std::map<stw_scl::C_SCLString, bool> c_Retval;
   for (uint32 u32_ItDashboard = 0; u32_ItDashboard < this->mc_Dashboards.size(); ++u32_ItDashboard)
   {
      const C_PuiSvDashboard & rc_Data = this->mc_Dashboards[u32_ItDashboard];
      c_Retval[rc_Data.GetName().toStdString().c_str()] = true;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all read rail assignments for validity
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvData::m_CheckAllReadRails(void)
{
   //No step because vector size changes while deleting
   for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::iterator c_ItRail =
           this->mc_ReadRailAssignments.begin();
        c_ItRail != this->mc_ReadRailAssignments.end();)
   {
      if (this->CheckReadUsage(c_ItRail.key()) == true)
      {
         //Nothing to do, iterate
         ++c_ItRail;
      }
      else
      {
         //New item at current position
         c_ItRail = this->mc_ReadRailAssignments.erase(c_ItRail);
      }
   }
}

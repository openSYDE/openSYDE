//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System views data manager (implementation)

   System views data manager

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>

#include <QFileInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_Uti.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "CSCLChecksums.h"
#include "C_OSCXMLParser.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_PuiSvHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSvHandlerFiler.h"
#include "C_PuiSvHandlerFilerV1.h"
#include "C_SyvRoRouteCalculation.h"
#include "C_OSCRoutingCalculation.h"
#include "C_OSCHALCMagicianUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_PuiSvHandler * C_PuiSvHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system views

   Load system views and store in information in our instance data.

   \param[in]  orc_Path    Path to system views file

   \return
   C_NO_ERR   data read and placed into instance data
   C_RD_WR    problems accessing file system (e.g. no read access to file)
   C_RANGE    specified file does not exist (when loading)
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::LoadFromFile(const QString & orc_Path)
{
   return this->m_LoadFromFile(orc_Path,
                               C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_Nodes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system views

   Save UI data part of system views to XML file.

   \param[in]  orc_Path                   Path to system views file
   \param[in]  oq_UseDeprecatedV1Format   Optional flag to save in deprecated V1 format

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SaveToFile(const QString & orc_Path, const bool oq_UseDeprecatedV1Format)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path.toStdString().c_str()) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.toStdString().c_str());
      if (sn_Return != 0)
      {
         s32_Return = C_RD_WR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      const QFileInfo c_Info(orc_Path);
      const QDir c_BasePath = c_Info.dir();
      if (c_BasePath.mkpath(".") == true)
      {
         C_OSCXMLParser c_XMLParser;
         c_XMLParser.CreateAndSelectNodeChild("opensyde-system-views");
         if (oq_UseDeprecatedV1Format == true)
         {
            c_XMLParser.CreateNodeChild("file-version", "1");
            C_PuiSvHandlerFilerV1::h_SaveViews(this->mc_Views, c_XMLParser);
         }
         else
         {
            c_XMLParser.CreateNodeChild("file-version", "2");
            if (this->mq_IsServiceModeActive)
            {
               tgl_assert(c_XMLParser.CreateAndSelectNodeChild("service-mode") == "service-mode");
               c_XMLParser.SetAttributeBool("active", this->mq_IsServiceModeActive);
               c_XMLParser.SelectNodeParent();
            }
            s32_Return = C_PuiSvHandlerFiler::h_SaveViews(this->mc_Views, c_XMLParser, &c_BasePath);

            C_PuiSvHandlerFiler::h_SaveLastKnownHalcCrcs(this->mc_LastKnownHalcCrcs, c_XMLParser);
            //Only update hash in non deprecated mode
            //calculate the hash value and save it for comparing
            this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
         }
         if (s32_Return == C_NO_ERR)
         {
            s32_Return = c_XMLParser.SaveToFile(orc_Path.toStdString().c_str());
            if (s32_Return != C_NO_ERR)
            {
               s32_Return = C_RD_WR;
            }
         }
         else
         {
            s32_Return = C_RD_WR;
         }
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::HasHashChanged(void) const
{
   const uint32 u32_NewHash = this->m_CalcHashSystemViews();
   bool q_Changed = true;

   if (u32_NewHash == this->mu32_CalculatedHashSystemViews)
   {
      q_Changed = false;
   }

   return q_Changed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get view

   \param[in]  ou32_Index  View index

   \return
   NULL View not found
   Else Valid view
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvData * C_PuiSvHandler::GetView(const uint32 ou32_Index) const
{
   const C_PuiSvData * pc_Retval;

   if (ou32_Index < this->mc_Views.size())
   {
      pc_Retval = &this->mc_Views[ou32_Index];
   }
   else
   {
      pc_Retval = NULL;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of views

   \return
   Number of views
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvHandler::GetViewCount(void) const
{
   return this->mc_Views.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC CRCs

   \return
   Last known HALC crcs
*/
//----------------------------------------------------------------------------------------------------------------------
const std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> & C_PuiSvHandler::GetLastKnownHalcCrcs(
   void) const
{
   return this->mc_LastKnownHalcCrcs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get service mode active

   \return
   Is service mode active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetServiceModeActive(void) const
{
   return this->mq_IsServiceModeActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode active

   \param[in]  oq_NewValue    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::SetServiceModeActive(const bool oq_NewValue)
{
   this->mq_IsServiceModeActive = oq_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known HALC CRCs

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::SetLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                         C_PuiSvLastKnownHalElementId> & orc_Value)
{
   this->mc_LastKnownHalcCrcs = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view name

   \param[in]  ou32_Index  View index
   \param[in]  orc_Name    View name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewName(const uint32 ou32_Index, const QString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetName(orc_Name);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked flag for node in view

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oq_Checked        Checked flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewNodeCheckedState(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                               const bool oq_Checked)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeCheckedState(ou32_NodeIndex, static_cast<uint8>(oq_Checked));
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set View PC UI box

   \param[in]  ou32_Index  View index
   \param[in]  orc_Box     New PC UI box

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewPCBox(const uint32 ou32_Index, const C_PuiBsBox & orc_Box)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPCBox(orc_Box);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set View PC connection UI data

   \param[in]  ou32_Index  View index
   \param[in]  orc_Line    New View PC connection UI data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewPCConnection(const uint32 ou32_Index, const C_PuiBsLineBase & orc_Line)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPCConnection(orc_Line);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  ou32_Index     View index
   \param[in]  oq_Connected   Flag if pc is connected
   \param[in]  ou32_BusIndex  Bus index PC is connected to

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewPCConnected(const uint32 ou32_Index, const bool oq_Connected, const uint32 ou32_BusIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPCConnected(oq_Connected, ou32_BusIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set path for the CAN DLL

   \param[in]  ou32_Index     View index
   \param[in]  oe_Type        CAN DLL Type
   \param[in]  orc_DllPath    Path for the CAN DLL

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewPCCANDll(const uint32 ou32_Index, const C_PuiSvPc::E_CANDllType oe_Type,
                                       const QString & orc_DllPath)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetPCCANDllType(oe_Type);
      rc_View.SetPCCANDllPath(orc_DllPath);

      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewServiceModeActive(const uint32 ou32_Index, const bool oq_NewValue)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode setup active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewServiceModeSetupActive(const uint32 ou32_Index, const bool oq_NewValue)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeSetupActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode update active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewServiceModeUpdateActive(const uint32 ou32_Index, const bool oq_NewValue)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeUpdateActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set service mode dashboard active

   \param[in]  ou32_Index     View index
   \param[in]  oq_NewValue    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  View index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewServiceModeDashboardActive(const uint32 ou32_Index, const bool oq_NewValue)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_Index];
      rc_View.SetServiceModeDashboardActive(oq_NewValue);
      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard name

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Name             New dashboard name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardName(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                        const QString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardName(ou32_DashboardIndex, orc_Name);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard comment

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Comment          New dashboard comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardComment(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                           const QString & orc_Comment)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardComment(ou32_DashboardIndex, orc_Comment);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard type

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oe_Type              Type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardType(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                        const C_PuiSvDashboard::E_TabType oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardType(ou32_DashboardIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard active flag

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oq_Active            New active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardActive(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                          const bool oq_Active)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardActive(ou32_DashboardIndex, oq_Active);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard current tab index

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  os32_Value           New current tab index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardTabIndex(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                            const sint32 os32_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardTabIndex(ou32_DashboardIndex, os32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard widget box

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardWidget(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                          const uint32 ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                          const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardBoundary(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                            const uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardBoundary(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardImage(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                         const uint32 & oru32_Index, const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardImage(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardLineArrow(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                             const uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardLineArrow(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetDashboardTextElement(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                               const uint32 & oru32_Index, const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetDashboardTextElement(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set fast update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New fast update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewUpdateRateFast(const uint32 ou32_ViewIndex, const uint16 ou16_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateFast(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set medium update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New medium update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewUpdateRateMedium(const uint32 ou32_ViewIndex, const uint16 ou16_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateMedium(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set slow update rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou16_Value        New slow update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewUpdateRateSlow(const uint32 ou32_ViewIndex, const uint16 ou16_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetUpdateRateSlow(ou16_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view device config screen selected bit rate

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_Value        New device config screen selected bit rate in kbit/s

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewDeviceConfigSelectedBitRate(const uint32 ou32_ViewIndex, const uint32 ou32_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDeviceConfigSelectedBitRate(ou32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view device config selected mode

   \param[in]  ou32_ViewIndex    View index (identifier)
   \param[in]  oe_Value          New device config screen selected mode

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewDeviceConfigMode(const uint32 ou32_ViewIndex,
                                               const C_PuiSvData::E_DeviceConfigurationMode oe_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDeviceConfigMode(oe_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  oq_Value          New dark mode active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewDarkModeActive(const uint32 ou32_ViewIndex, const bool oq_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetDarkModeActive(oq_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view read rail assignment

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID
   \param[in]  orc_Config        Config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetViewReadRailAssignment(const uint32 ou32_ViewIndex,
                                                 const C_OSCNodeDataPoolListElementId & orc_Id,
                                                 const C_PuiSvReadDataConfiguration & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetReadRailAssignment(orc_Id, orc_Config);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_ViewIndex             View index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformation(const uint32 ou32_ViewIndex,
                                                const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeUpdateInformation(orc_NodeUpdateInformation);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information

   \param[in]  ou32_ViewIndex             View index
   \param[in]  ou32_NodeIndex             Node index
   \param[in]  orc_NodeUpdateInformation  New node update information

   \return
   C_NO_ERR    No error
   C_RANGE     Node index invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformation(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                const C_PuiSvNodeUpdate & orc_NodeUpdateInformation)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.SetNodeUpdateInformation(ou32_NodeIndex, orc_NodeUpdateInformation);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformationPath(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                    const uint32 ou32_Index, const QString & orc_Value,
                                                    const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationPath(ou32_NodeIndex, ou32_Index, orc_Value, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformationParamInfo(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                         const uint32 ou32_Index,
                                                         const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationParamInfo(ou32_NodeIndex, ou32_Index, orc_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information skip flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformationSkipUpdateOfPath(const uint32 ou32_ViewIndex,
                                                                const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                                const bool oq_SkipFile,
                                                                const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationSkipUpdateOfPath(ou32_NodeIndex, ou32_Index, oq_SkipFile, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set skip flag

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to access
   \param[in]  oq_SkipFile       New skip flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformationSkipUpdateOfParamInfo(const uint32 ou32_ViewIndex,
                                                                     const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_Index, const bool oq_SkipFile)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationSkipUpdateOfParamInfo(ou32_NodeIndex, ou32_Index, oq_SkipFile);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node update information parameter set information

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_Index           Index to access
   \param[in]  orc_FilePath         New path
   \param[in]  ou32_LastKnownCrc    Last known CRC for this file

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetNodeUpdateInformationParamInfoContent(const uint32 ou32_ViewIndex,
                                                                const uint32 ou32_NodeIndex, const uint32 ou32_Index,
                                                                const QString & orc_FilePath,
                                                                const uint32 ou32_LastKnownCrc)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SetNodeUpdateInformationParamInfoContent(ou32_NodeIndex, ou32_Index, orc_FilePath,
                                                                    ou32_LastKnownCrc);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add view

   \param[in]  orc_View             View item
   \param[in]  oq_AutoAdaptName     Optional flag to allow or block automatic adaption of input name
   \param[in]  oq_AutoAdaptContent  Optional flag to allow or block automatic adaption of input content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::AddView(const C_PuiSvData & orc_View, const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent)
{
   tgl_assert(this->InsertView(this->mc_Views.size(), orc_View, oq_AutoAdaptName, oq_AutoAdaptContent) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view

   The previous view will be overwritten

   \param[in]  ou32_Index  View index
   \param[in]  orc_View    View item

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SetView(const uint32 ou32_Index, const C_PuiSvData & orc_View)
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_Index <= this->mc_Views.size())
   {
      this->mc_Views[ou32_Index] = orc_View;

      s32_Retval = C_NO_ERR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert view

   \param[in]  ou32_Index           View index
   \param[in]  orc_View             View item
   \param[in]  oq_AutoAdaptName     Optional flag to allow or block automatic adaption of input name
   \param[in]  oq_AutoAdaptContent  Optional flag to allow or block automatic adaption of input content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertView(const uint32 ou32_Index, const C_PuiSvData & orc_View, const bool oq_AutoAdaptName,
                                  const bool oq_AutoAdaptContent)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index <= this->mc_Views.size())
   {
      //Init
      C_PuiSvData c_TmpView = orc_View;
      if (oq_AutoAdaptContent == true)
      {
         c_TmpView.InitFromSystemDefinition();
      }
      if (oq_AutoAdaptName == true)
      {
         c_TmpView.SetName(C_Uti::h_GetUniqueNameQ(this->m_GetExistingViewNames(), c_TmpView.GetName()));
      }
      this->mc_Views.insert(this->mc_Views.begin() + ou32_Index, c_TmpView);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new read rail item to configuration

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID
   \param[in]  orc_Config        Configuration

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_NOACT  Already exists
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddViewReadRailItem(const uint32 ou32_ViewIndex, const C_OSCNodeDataPoolListElementId & orc_Id,
                                           const C_PuiSvReadDataConfiguration & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddReadRailItem(orc_Id, orc_Config);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddNodeUpdateInformationPath(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                    const QString & orc_Value,
                                                    const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddNodeUpdateInformationPath(ou32_NodeIndex, orc_Value, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add node update information for a parameter set

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_Value         New path

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddNodeUpdateInformationParamInfo(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                         const C_PuiSvNodeUpdateParamInfo & orc_Value)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddNodeUpdateInformationParamInfo(ou32_NodeIndex, orc_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add dashboard to view

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Dashboard     Dashboard data
   \param[in]  oq_AutoAdapt      Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboard(const uint32 ou32_ViewIndex, const C_PuiSvDashboard & orc_Dashboard,
                                    const bool oq_AutoAdapt)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.AddDashboard(orc_Dashboard, oq_AutoAdapt);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert dashboard to view

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Dashboard        Dashboard data
   \param[in]  oq_AutoAdapt         Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboard(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                       const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboard(ou32_DashboardIndex, orc_Dashboard, oq_AutoAdapt);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add widget to view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboardWidget(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                          const C_PuiSvDbWidgetBase * const opc_Box,
                                          const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardWidget(ou32_DashboardIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert widget to view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  opc_Box              New widget value
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboardWidget(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                             const uint32 ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                             const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, opc_Box, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboardBoundary(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                            const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardBoundary(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboardBoundary(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                               const uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardBoundary(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboardImage(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                         const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardImage(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboardImage(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                            const uint32 & oru32_Index, const C_PuiBsImage & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardImage(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboardLineArrow(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                             const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardLineArrow(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboardLineArrow(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                                const uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardLineArrow(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddDashboardTextElement(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                               const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardTextElement(ou32_DashboardIndex, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert new item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index
   \param[in]  orc_Data             New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::InsertDashboardTextElement(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                                  const uint32 & oru32_Index, const C_PuiBsTextElement & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.InsertDashboardTextElement(ou32_DashboardIndex, oru32_Index, orc_Data);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete view

   \param[in]  ou32_Index  View index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteView(const uint32 ou32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      this->mc_Views.erase(this->mc_Views.begin() + ou32_Index);
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

   \param[in]  ou32_ViewIndex    View index
   \param[in]  orc_Id            ID

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::RemoveViewReadRailItem(const uint32 ou32_ViewIndex,
                                              const C_OSCNodeDataPoolListElementId & orc_Id)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveReadRailItem(orc_Id);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information path

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::RemoveNodeUpdateInformationPath(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                       const uint32 ou32_Index,
                                                       const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveNodeUpdateInformationPath(ou32_NodeIndex, ou32_Index, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove node update information parameter set information

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_Index        Index to remove

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::RemoveNodeUpdateInformationParamInfo(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_Index)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.RemoveNodeUpdateInformationParamInfo(ou32_NodeIndex, ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information paths as appropriate for the type

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_Type           Selector for structure

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::ClearNodeUpdateInformationAsAppropriate(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                               const C_PuiSvNodeUpdate::E_GenericFileType oe_Type)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearNodeUpdateInformationAsAppropriate(ou32_NodeIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all node update information parameter set paths for this node

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_NodeIndex    Node index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::ClearNodeUpdateInformationParamPaths(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex)
{
   sint32 s32_Retval;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearNodeUpdateInformationParamPaths(ou32_NodeIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete dashboard from view

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboard(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboard(ou32_DashboardIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete widget from view dashboard

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  ou32_WidgetIndex     Widget index
   \param[in]  oe_Type              New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboardWidget(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                             const uint32 ou32_WidgetIndex, const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardWidget(ou32_DashboardIndex, ou32_WidgetIndex, oe_Type);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboardBoundary(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                               const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardBoundary(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboardImage(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                            const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardImage(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboardLineArrow(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                                const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardLineArrow(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete item

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index
   \param[in]  oru32_Index          Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::DeleteDashboardTextElement(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex,
                                                  const uint32 & oru32_Index)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.DeleteDashboardTextElement(ou32_DashboardIndex, oru32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear system views
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::Clear(void)
{
   this->mc_Views.clear();
   this->mc_LastKnownHalcCrcs.clear();

   this->mc_SdNodeErrors.clear();
   this->mc_SdBusErrors.clear();
   this->mq_IsServiceModeActive = false;

   //Reset hash
   this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move view

   \param[in]  ou32_StartIndex   Start index
   \param[in]  ou32_TargetIndex  Target index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::MoveView(const uint32 ou32_StartIndex, const uint32 ou32_TargetIndex)
{
   sint32 s32_Retval;
   const C_PuiSvData * const pc_View = this->GetView(ou32_StartIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvData c_View = *pc_View;
      s32_Retval = this->DeleteView(ou32_StartIndex);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->InsertView(ou32_TargetIndex, c_View, false, false);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronise internally stored scaling information with current system definition

   \param[in]  ou32_ViewIndex       View index
   \param[in]  ou32_DashboardIndex  Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::SyncDashboardScalingInformation(const uint32 ou32_ViewIndex, const uint32 ou32_DashboardIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.SyncDashboardScalingInformation(ou32_DashboardIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update system definition errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::UpdateSystemDefintionErrors(void)
{
   const uint32 u32_SysDefHash = C_PuiSdHandler::h_GetInstance()->CalcHashSystemDefinition();

   if (u32_SysDefHash != this->mu32_PreviousSystemDefintionHash)
   {
      //Clear! Important!
      this->mc_SdNodeErrors.clear();
      this->mc_SdBusErrors.clear();
      //Node error
      this->mc_SdNodeErrors.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize());
      for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
      {
         const bool q_Error = C_PuiSdHandler::h_GetInstance()->CheckNodeConflict(u32_ItNode);
         this->mc_SdNodeErrors.push_back(q_Error);
      }
      //Bus error
      this->mc_SdBusErrors.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize());
      for (uint32 u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
      {
         const bool q_Error = C_PuiSdHandler::h_GetInstance()->CheckBusConflict(u32_ItBus);
         this->mc_SdBusErrors.push_back(q_Error);
      }
      //Clear all previous view errors (might have changed with new SD)
      this->mc_PreviousErrorCheckResults.clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get buffered error for node

   \param[in]  ou32_Index  Index

   \return
   Buffered error for node
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorNode(const uint32 ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdNodeErrors.size())
   {
      q_Retval = this->mc_SdNodeErrors[ou32_Index];
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get buffered error for bus

   \param[in]  ou32_Index  Index

   \return
   Buffered error for bus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorBus(const uint32 ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdBusErrors.size())
   {
      q_Retval = this->mc_SdBusErrors[ou32_Index];
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check view error

   \param[in]      ou32_Index                            View index
   \param[in,out]  opq_NameInvalid                       Name conflict
   \param[in,out]  opq_PCNotConnected                    PC not connected
   \param[in,out]  opq_RoutingInvalid                    Routing invalid
   \param[in,out]  opq_UpdateDisabledButDataBlocks       No interface supports update but there are data blocks
   \param[in,out]  opq_SysDefInvalid                     System definition invalid
   \param[in,out]  opq_NoNodesActive                     No nodes active
   \param[in,out]  opc_RoutingErrorDetails               Error details for routing error if any
                                                         Warning: only set if routing error checked
   \param[in,out]  opc_AutomaticallyDisabledSubDevices   Automatically disabled sub devices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckViewError(const uint32 ou32_Index, bool * const opq_NameInvalid,
                                      bool * const opq_PCNotConnected, bool * const opq_RoutingInvalid,
                                      bool * const opq_UpdateDisabledButDataBlocks, bool * const opq_SysDefInvalid,
                                      bool * const opq_NoNodesActive, QString * const opc_RoutingErrorDetails,
                                      QString * const opc_AutomaticallyDisabledSubDevices)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Views.size())
   {
      const uint32 u32_Hash = this->GetViewHash(ou32_Index);
      const QMap<stw_types::uint32,
                 C_PuiSvViewErrorDetails>::const_iterator c_It = this->mc_PreviousErrorCheckResults.find(u32_Hash);
      if (c_It == this->mc_PreviousErrorCheckResults.end())
      {
         const C_PuiSvData & rc_CheckedData = this->mc_Views[ou32_Index];
         C_PuiSvViewErrorDetails c_Details;
         QString c_Text;
         std::vector<uint32> c_AutomaticallyDisabledSubDevices;
         bool q_Error = true;
         const std::vector<uint8> & rc_NodeActiveFlags = rc_CheckedData.GetNodeActiveFlags();
         //Name check
         c_Details.q_NameInvalid = false;
         for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
         {
            if (u32_ItView != ou32_Index)
            {
               const C_PuiSvData & rc_CurData = this->mc_Views[u32_ItView];
               if (rc_CurData.GetName().compare(rc_CheckedData.GetName()) == 0)
               {
                  c_Details.q_NameInvalid = true;
               }
            }
         }
         //Check PC connected
         c_Details.q_PCNotConnected = !rc_CheckedData.GetPcData().GetConnected();
         //Check all routing details
         tgl_assert(this->m_CheckRoutingAndUpdateNodes(ou32_Index, q_Error, c_Text,
                                                       c_AutomaticallyDisabledSubDevices) == C_NO_ERR);
         c_Details.ConstructAutomaticallyDisabledSubDevicesString(c_AutomaticallyDisabledSubDevices);
         c_Details.q_RoutingInvalid = q_Error;
         c_Details.c_RoutingErrorDetails = c_Text;
         //Check data block issues
         CheckUpdateEnabledForDataBlocks(ou32_Index, q_Error, c_Text);
         c_Details.q_UpdateDisabledButDataBlocks = q_Error;
         //Check system definition
         c_Details.q_SysDefInvalid = false;
         //Node error
         for (uint32 u32_ItNode = 0; u32_ItNode < rc_NodeActiveFlags.size(); ++u32_ItNode)
         {
            if (rc_NodeActiveFlags[u32_ItNode] == true)
            {
               if (this->GetErrorNode(u32_ItNode) == true)
               {
                  c_Details.q_SysDefInvalid = true;
                  break;
               }
            }
         }
         //Bus error
         if (c_Details.q_SysDefInvalid == false)
         {
            for (uint32 u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
            {
               if (this->CheckBusDisabled(ou32_Index, u32_ItBus) == false)
               {
                  if (this->GetErrorBus(u32_ItBus) == true)
                  {
                     c_Details.q_SysDefInvalid = true;
                     break;
                  }
               }
            }
         }
         //Check any nodes active
         c_Details.q_NoNodesActive = true;
         //Node error
         for (uint32 u32_ItNode = 0; u32_ItNode < rc_NodeActiveFlags.size(); ++u32_ItNode)
         {
            if (rc_NodeActiveFlags[u32_ItNode] == true)
            {
               c_Details.q_NoNodesActive = false;
               break;
            }
         }
         c_Details.c_ResultingNodeActiveStatus = rc_CheckedData.GetNodeActiveFlags();

         //Return error details
         c_Details.GetResults(opq_NameInvalid, opq_PCNotConnected, opq_RoutingInvalid,
                              opq_UpdateDisabledButDataBlocks, opq_SysDefInvalid, opq_NoNodesActive,
                              opc_RoutingErrorDetails, opc_AutomaticallyDisabledSubDevices);
         //Store results
         this->mc_PreviousErrorCheckResults.insert(u32_Hash, c_Details);
      }
      else
      {
         //Always update node active flags
         C_PuiSvData & rc_CheckedData = this->mc_Views[ou32_Index];
         rc_CheckedData.SetNodeActiveFlags(c_It.value().c_ResultingNodeActiveStatus);
         c_It.value().GetResults(opq_NameInvalid, opq_PCNotConnected, opq_RoutingInvalid,
                                 opq_UpdateDisabledButDataBlocks, opq_SysDefInvalid, opq_NoNodesActive,
                                 opc_RoutingErrorDetails, opc_AutomaticallyDisabledSubDevices);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check update possible

   \param[in]   ou32_ViewIndex                     View index (identifier)
   \param[out]  orq_UpdateDisabledButDataBlocks    Data blocks but no interface updateable
   \param[out]  orc_ErrorMessage                   Error details (affected nodes)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::CheckUpdateEnabledForDataBlocks(const uint32 ou32_ViewIndex,
                                                     bool & orq_UpdateDisabledButDataBlocks,
                                                     QString & orc_ErrorMessage) const
{
   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_CheckedData = this->mc_Views[ou32_ViewIndex];
      const std::vector<uint8> & rc_NodeActiveFlags = rc_CheckedData.GetNodeActiveFlags();

      orq_UpdateDisabledButDataBlocks = false;
      for (uint32 u32_ItNode = 0; u32_ItNode < rc_NodeActiveFlags.size(); ++u32_ItNode)
      {
         if (rc_NodeActiveFlags[u32_ItNode] == true)
         {
            const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_ItNode);
            if (pc_Node != NULL)
            {
               if (pc_Node->c_Applications.size() > 0UL)
               {
                  bool q_AnyUpdateEnabled = false;
                  for (uint32 u32_ItInterface = 0; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size();
                       ++u32_ItInterface)
                  {
                     const C_OSCNodeComInterfaceSettings & rc_CurInterface =
                        pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
                     if (rc_CurInterface.q_IsUpdateEnabled == true)
                     {
                        q_AnyUpdateEnabled = true;
                     }
                  }
                  if (q_AnyUpdateEnabled == false)
                  {
                     orq_UpdateDisabledButDataBlocks = true;
                     if (orc_ErrorMessage.compare("") != 0)
                     {
                        orc_ErrorMessage += ", ";
                     }
                     orc_ErrorMessage += pc_Node->c_Properties.c_Name.c_str();
                  }
               }
            }
            else
            {
               orq_UpdateDisabledButDataBlocks = true;
               orc_ErrorMessage = "Unknown";
            }
         }
      }
   }
   else
   {
      orq_UpdateDisabledButDataBlocks = true;
      orc_ErrorMessage = "Unknown";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check routing error

   \param[in]      ou32_ViewIndex      View index
   \param[in,out]  orc_ErrorDetails    Error details

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckRouting(const uint32 ou32_ViewIndex, std::map<uint32, QString> & orc_ErrorDetails) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      const std::vector<uint8> & rc_ActiveNodes = rc_View.GetNodeActiveFlags();
      uint32 u32_Counter;

      // check connection state of all active nodes
      for (u32_Counter = 0U; u32_Counter < rc_ActiveNodes.size(); ++u32_Counter)
      {
         if (rc_ActiveNodes[u32_Counter] == true)
         {
            const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
               u32_Counter);

            if (pc_Node != NULL)
            {
               // Check update routes
               const C_SyvRoRouteCalculation c_RouteCalcUpdate(ou32_ViewIndex, u32_Counter,
                                                               stw_opensyde_core::C_OSCRoutingCalculation::eUPDATE);
               // Check diagnostic routes
               const C_SyvRoRouteCalculation c_RouteCalcDiag(ou32_ViewIndex, u32_Counter,
                                                             stw_opensyde_core::C_OSCRoutingCalculation::eDIAGNOSTIC);

               // Normally both variants must fail in this case on the same way
               if ((c_RouteCalcUpdate.GetState() == C_CONFIG) ||
                   (c_RouteCalcDiag.GetState() == C_CONFIG))
               {
                  orc_ErrorDetails[u32_Counter] = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                                  C_GtGetText::h_GetText(" (CAN to Ethernet routing not supported)");
               }
               else
               {
                  //Error check
                  const bool q_RoutingUpdateError = ((c_RouteCalcUpdate.WasPcAvailable() == false) ||
                                                     ((c_RouteCalcUpdate.GetState() != C_NO_ERR) &&
                                                      (c_RouteCalcUpdate.GetState() != C_NOACT)));
                  const bool q_RoutingDiagnosticError = ((c_RouteCalcDiag.WasPcAvailable() == false) ||
                                                         ((c_RouteCalcDiag.GetState() != C_NO_ERR) &&
                                                          (c_RouteCalcDiag.GetState() != C_NOACT)));
                  if (q_RoutingUpdateError == true)
                  {
                     if (q_RoutingDiagnosticError == true)
                     {
                        //Combined error
                        orc_ErrorDetails[u32_Counter] = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                                        C_GtGetText::h_GetText(" (Update + Diagnostic)");
                     }
                     else
                     {
                        //Update error
                        orc_ErrorDetails[u32_Counter] = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                                        C_GtGetText::h_GetText(" (Update)");
                     }
                  }
                  else
                  {
                     if (q_RoutingDiagnosticError == true)
                     {
                        //Diagnostic error
                        orc_ErrorDetails[u32_Counter] = static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) +
                                                        C_GtGetText::h_GetText(" (Diagnostic)");
                     }
                  }
               }
            }
            else
            {
               // Should not happen
               s32_Retval = C_RANGE;
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
/*! \brief   Check view reconnect necessary

   \param[in]   ou32_ViewIndex            View index
   \param[out]  orq_ReconnectNecessary    Flag if reconnect necessary

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckViewReconnectNecessary(const uint32 ou32_ViewIndex, bool & orq_ReconnectNecessary)
{
   sint32 s32_Retval = C_NO_ERR;

   orq_ReconnectNecessary = false;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      // Trigger view check
      // includes hash and is necessary for node indices update, which should be done before loading any view
      this->CheckViewError(ou32_ViewIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      //Check if bus exists
      if (rc_View.GetPcData().GetConnected() == true)
      {
         if (rc_View.GetPcData().GetBusIndex() >= C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize())
         {
            orq_ReconnectNecessary = true;
         }
         else
         {
            //Check if bus enabled
            if (this->CheckBusDisabled(ou32_ViewIndex, rc_View.GetPcData().GetBusIndex()) == true)
            {
               orq_ReconnectNecessary = true;
            }
         }
      }
      else
      {
         //Check if any bus enabled
         for (uint32 u32_ItBus = 0; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
         {
            if (this->CheckBusDisabled(ou32_ViewIndex, u32_ItBus) == false)
            {
               orq_ReconnectNecessary = true;
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
/*! \brief  Check and handle new element

   \param[in]  orc_NewId   New id

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckAndHandleNewElement(const C_PuiSvDbNodeDataPoolListElementId & orc_NewId)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_NewId.GetIsValid())
   {
      C_OSCNodeDataPool::E_Type e_Type;
      if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(orc_NewId.u32_NodeIndex, orc_NewId.u32_DataPoolIndex,
                                                           e_Type) == C_NO_ERR)
      {
         if ((e_Type == C_OSCNodeDataPool::eHALC) ||
             (e_Type == C_OSCNodeDataPool::eHALC_NVM))
         {
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_NewId.u32_NodeIndex);
            const C_OSCNodeDataPool * const pc_Dp =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_NewId.u32_NodeIndex,
                                                               orc_NewId.u32_DataPoolIndex);
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_NewId.u32_NodeIndex,
                                                                          orc_NewId.u32_DataPoolIndex,
                                                                          orc_NewId.u32_ListIndex,
                                                                          orc_NewId.u32_ElementIndex);
            if ((pc_Node != NULL) && ((pc_Element != NULL) && (pc_Dp != NULL)))
            {
               const std::string c_Tmp = orc_NewId.GetHalChannelName().toStdString();
               uint32 u32_Hash = 0UL;
               if (pc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
               {
                  pc_Element->CalcHashElement(u32_Hash, orc_NewId.GetArrayElementIndexOrZero());
               }
               else
               {
                  pc_Element->CalcHashStructure(u32_Hash);
               }
               stw_scl::C_SCLChecksums::CalcCRC32(
                  c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
               this->mc_LastKnownHalcCrcs[orc_NewId] = C_PuiSvLastKnownHalElementId(u32_Hash, pc_Dp->c_Name.c_str());
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc view routing crc name

   \param[in]   ou32_ViewIndex   View index
   \param[in]   orc_NodeName     Node name
   \param[out]  oru32_Crc        Crc

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CalcViewRoutingCrcName(const uint32 ou32_ViewIndex, const QString & orc_NodeName,
                                              uint32 & oru32_Crc) const
{
   uint32 u32_NodeIndex;
   sint32 s32_Retval = C_PuiSdHandler::h_GetInstance()->MapNodeNameToIndex(orc_NodeName, u32_NodeIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->CalcViewRoutingCrcIndex(ou32_ViewIndex, u32_NodeIndex, oru32_Crc);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc view routing crc index

   \param[in]   ou32_ViewIndex   View index
   \param[in]   ou32_NodeIndex   Node index
   \param[out]  oru32_Crc        Crc

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::CalcViewRoutingCrcIndex(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                               uint32 & oru32_Crc) const
{
   sint32 s32_Retval = C_NO_ERR;

   const C_PuiSvData * const pc_View = this->GetView(ou32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetNodeActive(ou32_NodeIndex))
      {
         const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
            ou32_NodeIndex);

         if (pc_Node != NULL)
         {
            // Check update routes
            const C_SyvRoRouteCalculation c_RouteCalcUpdate(ou32_ViewIndex, ou32_NodeIndex,
                                                            stw_opensyde_core::C_OSCRoutingCalculation::eUPDATE);
            if (c_RouteCalcUpdate.GetState() == C_NO_ERR)
            {
               const C_OSCRoutingRoute * const pc_Route = c_RouteCalcUpdate.GetBestRoute();
               if (pc_Route != NULL)
               {
                  QString c_Name;

                  //Calc CRC

                  //Init
                  oru32_Crc = 0xFFFFFFFFUL;

                  if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(pc_View->GetPcData().GetBusIndex(),
                                                                         c_Name) == C_NO_ERR)
                  {
                     stw_scl::C_SCLChecksums::CalcCRC32(c_Name.toStdString().c_str(), c_Name.length(), oru32_Crc);
                     if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(pc_Route->u32_TargetNodeIndex,
                                                                             c_Name) == C_NO_ERR)
                     {
                        stw_scl::C_SCLChecksums::CalcCRC32(c_Name.toStdString().c_str(), c_Name.length(), oru32_Crc);

                        for (uint32 u32_ItRoute = 0UL;
                             (u32_ItRoute < pc_Route->c_VecRoutePoints.size()) && (s32_Retval == C_NO_ERR);
                             ++u32_ItRoute)
                        {
                           const C_OSCRoutingRoutePoint & rc_Route = pc_Route->c_VecRoutePoints[u32_ItRoute];

                           //Others
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.e_InInterfaceType,
                                                              sizeof(rc_Route.e_InInterfaceType), oru32_Crc);
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.e_OutInterfaceType,
                                                              sizeof(rc_Route.e_OutInterfaceType), oru32_Crc);
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.u8_InInterfaceNumber,
                                                              sizeof(rc_Route.u8_InInterfaceNumber), oru32_Crc);
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.u8_OutInterfaceNumber,
                                                              sizeof(rc_Route.u8_OutInterfaceNumber), oru32_Crc);
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.u8_InNodeID,
                                                              sizeof(rc_Route.u8_InNodeID), oru32_Crc);
                           stw_scl::C_SCLChecksums::CalcCRC32(&rc_Route.u8_OutNodeID,
                                                              sizeof(rc_Route.u8_OutNodeID), oru32_Crc);

                           //Names
                           if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(rc_Route.u32_InBusIndex,
                                                                                  c_Name) == C_NO_ERR)
                           {
                              stw_scl::C_SCLChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                 c_Name.length(), oru32_Crc);
                           }
                           else
                           {
                              s32_Retval = C_RANGE;
                           }
                           if (s32_Retval == C_NO_ERR)
                           {
                              if (C_PuiSdHandler::h_GetInstance()->MapBusIndexToName(rc_Route.u32_OutBusIndex,
                                                                                     c_Name) == C_NO_ERR)
                              {
                                 stw_scl::C_SCLChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                    c_Name.length(), oru32_Crc);
                                 if (C_PuiSdHandler::h_GetInstance()->MapNodeIndexToName(rc_Route.u32_NodeIndex,
                                                                                         c_Name) == C_NO_ERR)
                                 {
                                    stw_scl::C_SCLChecksums::CalcCRC32(c_Name.toStdString().c_str(),
                                                                       c_Name.length(), oru32_Crc);
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
/*! \brief   Get namespace for ID

   \param[in]  orc_Id   ID

   \return
   Namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandler::h_GetNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval;

   if (orc_Id.GetIsValid())
   {
      if (orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
      {
         C_OSCNodeDataPool::E_Type e_Type;
         if ((C_PuiSdHandler::h_GetInstance()->GetDataPoolType(orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex,
                                                               e_Type) == C_NO_ERR) &&
             ((e_Type == C_OSCNodeDataPool::eHALC) || (e_Type == C_OSCNodeDataPool::eHALC_NVM)))
         {
            c_Retval = C_PuiSdUtil::h_GetHALCNamespace(orc_Id);
         }
         else
         {
            c_Retval = C_PuiSdUtil::h_GetNamespace(orc_Id);
            if (orc_Id.GetUseArrayElementIndex())
            {
               //Append array element index
               c_Retval += static_cast<QString>(C_GtGetText::h_GetText("[%1]")).arg(orc_Id.GetArrayElementIndex());
            }
         }
      }
      else
      {
         c_Retval = C_PuiSdUtil::h_GetSignalNamespace(orc_Id);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get short namespace from Id

   \param[in]  orc_Id   Datapool element ID

   \return
   short namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvHandler::h_GetShortNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id)
{
   QString c_Retval = C_GtGetText::h_GetText("Unknown HAL data element");
   uint32 u32_DomainIndex;
   bool q_UseChannelIndex;
   uint32 u32_ChannelIndex;

   C_OSCHalcDefDomain::E_VariableSelector e_Selector;
   uint32 u32_ParameterIndex;
   bool q_UseElementIndex;
   uint32 u32_ParameterElementIndex;
   bool q_IsUseCaseIndex;
   bool q_IsChanNumIndex;
   bool q_IsSafetyFlagIndex;

   if (C_PuiSdHandler::h_GetInstance()->TranslateToHALCIndex(orc_Id, orc_Id.GetArrayElementIndexOrZero(),
                                                             u32_DomainIndex, q_UseChannelIndex,
                                                             u32_ChannelIndex, e_Selector, u32_ParameterIndex,
                                                             q_UseElementIndex,
                                                             u32_ParameterElementIndex, q_IsUseCaseIndex,
                                                             q_IsChanNumIndex, q_IsSafetyFlagIndex) == C_NO_ERR)
   {
      {
         const C_OSCNodeDataPoolList * const pc_List =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(orc_Id.u32_NodeIndex,
                                                                orc_Id.u32_DataPoolIndex,
                                                                orc_Id.u32_ListIndex);
         const C_OSCHalcConfigChannel * const pc_Config =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(orc_Id.u32_NodeIndex,
                                                                            u32_DomainIndex,
                                                                            u32_ChannelIndex,
                                                                            q_UseChannelIndex);
         const C_OSCHalcDefDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileDataConst(orc_Id.u32_NodeIndex,
                                                                        u32_DomainIndex);

         if ((((pc_List != NULL)) && (pc_Config != NULL)) && (pc_Domain != NULL))
         {
            QString c_ElementName;

            if (q_IsUseCaseIndex)
            {
               c_ElementName = C_OSCHALCMagicianUtil::h_GetUseCaseVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsChanNumIndex)
            {
               c_ElementName = C_OSCHALCMagicianUtil::h_GetChanNumVariableName(pc_Domain->c_SingularName).c_str();
            }
            else if (q_IsSafetyFlagIndex)
            {
               c_ElementName = C_OSCHALCMagicianUtil::h_GetSafetyFlagVariableName(pc_Domain->c_SingularName).c_str();
            }
            else
            {
               const C_OSCHalcDefStruct * const pc_Param =
                  C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileVariableData(orc_Id.u32_NodeIndex,
                                                                                 u32_DomainIndex, e_Selector,
                                                                                 u32_ParameterIndex);
               c_ElementName = pc_Param->c_Display.c_str();
               if (q_UseElementIndex)
               {
                  if (u32_ParameterElementIndex < pc_Param->c_StructElements.size())
                  {
                     const C_OSCHalcDefElement & rc_Param = pc_Param->c_StructElements[u32_ParameterElementIndex];
                     c_ElementName = rc_Param.c_Display.c_str();
                  }
               }
            }
            c_Retval = static_cast<QString>("%1::%2::%3").
                       arg(pc_Config->c_Name.c_str()).
                       arg(pc_List->c_Name.c_str()).
                       arg(c_ElementName);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus for disabled look

   \param[in]  ou32_ViewIndex    View index
   \param[in]  ou32_BusIndex     Bus index

   \return
   True  Disabled
   False Enabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvHandler::CheckBusDisabled(const uint32 ou32_ViewIndex, const uint32 ou32_BusIndex) const
{
   bool q_Disabled = true;

   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes,
                                                                                      c_InterfaceIndexes);
   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      const C_PuiSvData * const pc_View = this->GetView(ou32_ViewIndex);
      if (pc_View != NULL)
      {
         const std::vector<uint8> & rc_Nodes = pc_View->GetNodeActiveFlags();
         for (uint32 u32_ItNode = 0; u32_ItNode < c_NodeIndexes.size(); ++u32_ItNode)
         {
            if (c_NodeIndexes[u32_ItNode] < rc_Nodes.size())
            {
               if (rc_Nodes[c_NodeIndexes[u32_ItNode]] == true)
               {
                  q_Disabled = false;
               }
            }
         }
      }
   }

   return q_Disabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   View dashboard param widget clear all data pool elements

   \param[in]  ou32_ViewIndex          View index
   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::ClearViewDashboardParamDataPoolElements(const uint32 ou32_ViewIndex,
                                                               const uint32 ou32_DashboardIndex,
                                                               const uint32 ou32_ParamWidgetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.ClearDashboardParamDataPoolElements(ou32_DashboardIndex, ou32_ParamWidgetIndex);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   View dashboard param widget add new data pool element

   \param[in]  ou32_ViewIndex          View index
   \param[in]  ou32_DashboardIndex     Dashboard index
   \param[in]  ou32_ParamWidgetIndex   Param index
   \param[in]  orc_NewId               New ID
   \param[in]  opc_Content             Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::AddViewDashboardParamNewDataPoolElement(const uint32 ou32_ViewIndex,
                                                               const uint32 ou32_DashboardIndex,
                                                               const uint32 ou32_ParamWidgetIndex,
                                                               const C_OSCNodeDataPoolListElementId & orc_NewId,
                                                               const C_OSCNodeDataPoolContent * const opc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      s32_Retval = rc_View.AddDashboardParamNewDataPoolElement(ou32_DashboardIndex, ou32_ParamWidgetIndex, orc_NewId,
                                                               opc_Content);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for view

   \param[in]  ou32_ViewIndex    Index

   \return
   Hash for view
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvHandler::GetViewHash(const uint32 ou32_ViewIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFU;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      rc_View.CalcHash(u32_Retval);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler * C_PuiSvHandler::h_GetInstance(void)
{
   if (C_PuiSvHandler::mhpc_Singleton == NULL)
   {
      C_PuiSvHandler::mhpc_Singleton = new C_PuiSvHandler();
   }
   return C_PuiSvHandler::mhpc_Singleton;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up singleton
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::h_Destroy(void)
{
   delete C_PuiSvHandler::mhpc_Singleton;
   C_PuiSvHandler::mhpc_Singleton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*!
\brief   Load system views

   Load system views and store in information in our instance data.

   \param[in] orc_Path     Path to system views file
   \param[in] orc_OSCNodes OSC node information (Necessary for update information)

   \return
   C_NO_ERR   data read and placed into instance data
   C_RD_WR    problems accessing file system (e.g. no read access to file)
   C_RANGE    specified file does not exist (when loading)
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::m_LoadFromFile(const QString & orc_Path,
                                      const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path.toStdString().c_str()) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path.toStdString().c_str());
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-system-views")
         {
            if (c_XMLParser.SelectNodeChild("file-version") == "file-version")
            {
               bool q_Ok;
               const sintn sn_FileVersion = static_cast<QString>(c_XMLParser.GetNodeContent().c_str()).toInt(&q_Ok, 0);

               //is the file version one we know ?
               if (q_Ok)
               {
                  osc_write_log_info("Loading views", "Value of \"file-version\": " +
                                     stw_scl::C_SCLString::IntToStr(sn_FileVersion));
                  //Check file version
                  if ((sn_FileVersion != 1) && (sn_FileVersion != 2))
                  {
                     osc_write_log_error("Loading views",
                                         "Version defined by \"file-version\" is not supported.");
                     s32_Retval = C_CONFIG;
                  }
               }
               else
               {
                  osc_write_log_error("Loading views", "\"file-version\" could not be converted to a number.");
                  s32_Retval = C_CONFIG;
               }

               if (s32_Retval == C_NO_ERR)
               {
                  tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-views");
                  if (sn_FileVersion == 1)
                  {
                     s32_Retval = C_PuiSvHandlerFilerV1::h_LoadViews(this->mc_Views, c_XMLParser);
                  }
                  else
                  {
                     if (c_XMLParser.SelectNodeChild("service-mode") == "service-mode")
                     {
                        if (c_XMLParser.AttributeExists("active"))
                        {
                           this->SetServiceModeActive(c_XMLParser.GetAttributeBool("active"));
                        }
                        else
                        {
                           s32_Retval = C_CONFIG;
                           osc_write_log_error("Loading views",
                                               "Attribute \"active\" not found in node \"service-mode\".");
                        }
                        tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-views");
                     }
                     else
                     {
                        //Default
                        this->SetServiceModeActive(false);
                     }
                     if (s32_Retval == C_NO_ERR)
                     {
                        const QFileInfo c_Info(orc_Path);
                        const QDir c_BasePath = c_Info.dir();
                        s32_Retval = C_PuiSvHandlerFiler::h_LoadViews(this->mc_Views, orc_OSCNodes,
                                                                      c_XMLParser, &c_BasePath);
                        if (s32_Retval == C_NO_ERR)
                        {
                           s32_Retval = C_PuiSvHandlerFiler::h_LoadLastKnownHalcCrcs(this->mc_LastKnownHalcCrcs,
                                                                                     c_XMLParser);
                           if (s32_Retval == C_NO_ERR)
                           {
                              //calculate the hash value and save it for comparing (only for new file version!)
                              this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
                           }
                        }
                     }
                  }
                  m_FixInvalidRailConfig();
                  m_HandleCompatibilityChart();
               }
               else
               {
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               osc_write_log_error("Loading views", "Could not find \"file-version\" node.");
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add last known HAL CRC

   \param[in]  orc_Id   Id
   \param[in]  orc_Crc  Crc
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_AddLastKnownHalcCrc(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                           const C_PuiSvLastKnownHalElementId & orc_Crc)
{
   this->mc_LastKnownHalcCrcs[orc_Id] = orc_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler::C_PuiSvHandler(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mu32_CalculatedHashSystemViews(0U),
   mu32_PreviousSystemDefintionHash(0U),
   mq_IsServiceModeActive(false)
{
   //Connects for synchronisation
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeAdded, this,
           &C_PuiSvHandler::m_OnSyncNodeAdded);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeHALC, this,
           &C_PuiSvHandler::m_OnSyncNodeHALC);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeAboutToBeDeleted);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncBusAdded, this, &C_PuiSvHandler::m_OnSyncBusAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncBusDeleted, this, &C_PuiSvHandler::m_OnSyncBusDeleted);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncClear, this, &C_PuiSvHandler::m_OnSyncClear);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationAboutToBeChangedFromParamSetHALC, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationChangedToParamSetHALC, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationChangedToParamSetHALC);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeApplicationResultPathSizeChanged, this,
      &C_PuiSvHandler::m_OnSyncNodeApplicationResultPathSizeChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListDataSetAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAdded, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAdded);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementMoved, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementMoved);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementChanged, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementArrayChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAccessChanged, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAccessChanged);
   connect(
      C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeDataPoolListElementAboutToBeDeleted, this,
      &C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAboutToBeDeleted);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvHandler::~C_PuiSvHandler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to added node index

   \param[in]  ou32_Index  Added node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAdded(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAdded(ou32_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sync view node indices to node HALC change

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeHALC(const uint32 ou32_Index)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_Index);

   if (pc_Node != NULL)
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_NewMap;
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvDbNodeDataPoolListElementId> c_MapCurToNew;
      for (uint32 u32_ItDp = 0UL; u32_ItDp < pc_Node->c_DataPools.size(); ++u32_ItDp)
      {
         const C_OSCNodeDataPool & rc_Dp = pc_Node->c_DataPools[u32_ItDp];
         if ((rc_Dp.e_Type == C_OSCNodeDataPool::eHALC) || (rc_Dp.e_Type == C_OSCNodeDataPool::eHALC_NVM))
         {
            for (uint32 u32_ItLi = 0UL; u32_ItLi < rc_Dp.c_Lists.size(); ++u32_ItLi)
            {
               const C_OSCNodeDataPoolList & rc_Li = rc_Dp.c_Lists[u32_ItLi];
               for (uint32 u32_ItEl = 0UL; u32_ItEl < rc_Li.c_Elements.size(); ++u32_ItEl)
               {
                  const C_OSCNodeDataPoolListElement & rc_El = rc_Li.c_Elements[u32_ItEl];
                  for (uint32 u32_ItAr = 0UL; u32_ItAr < rc_El.GetArraySize(); ++u32_ItAr)
                  {
                     C_PuiSvDbNodeDataPoolListElementId c_NewId(ou32_Index, u32_ItDp, u32_ItLi, u32_ItEl,
                                                                C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                                rc_El.GetArraySize() != 1, u32_ItAr);
                     uint32 u32_DomainIndex;
                     bool q_UseChannelIndex;
                     uint32 u32_ChannelIndex;

                     C_OSCHalcDefDomain::E_VariableSelector e_Selector;
                     uint32 u32_ParameterIndex;
                     bool q_UseElementIndex;
                     uint32 u32_ParameterElementIndex;
                     bool q_IsUseCaseIndex;
                     bool q_IsChanNumIndex;
                     bool q_IsSafetyFlagIndex;

                     if (C_PuiSdHandler::h_GetInstance()->TranslateToHALCIndex(c_NewId,
                                                                               c_NewId.GetArrayElementIndexOrZero(),
                                                                               u32_DomainIndex, q_UseChannelIndex,
                                                                               u32_ChannelIndex, e_Selector,
                                                                               u32_ParameterIndex,
                                                                               q_UseElementIndex,
                                                                               u32_ParameterElementIndex,
                                                                               q_IsUseCaseIndex,
                                                                               q_IsChanNumIndex,
                                                                               q_IsSafetyFlagIndex) == C_NO_ERR)
                     {
                        QString c_Name;
                        if (C_PuiSdHandler::h_GetInstance()->GetHalChannelOrDomainName(ou32_Index,
                                                                                       u32_DomainIndex,
                                                                                       u32_ChannelIndex,
                                                                                       c_Name) == C_NO_ERR)
                        {
                           const std::string c_Tmp = c_Name.toStdString();
                           uint32 u32_Hash = 0UL;
                           if (pc_Node->c_HALCConfig.e_SafetyMode == C_OSCHalcDefBase::eTWO_LEVELS_WITH_DROPPING)
                           {
                              rc_El.CalcHashElement(u32_Hash, u32_ItAr);
                           }
                           else
                           {
                              rc_El.CalcHashStructure(u32_Hash);
                           }
                           stw_scl::C_SCLChecksums::CalcCRC32(
                              c_Tmp.c_str(), c_Tmp.length(), u32_Hash);
                           c_NewId.SetHalChannelName(c_Name);
                           for (std::map<C_PuiSvDbNodeDataPoolListElementId,
                                         C_PuiSvLastKnownHalElementId>::const_iterator c_ItCur =
                                   this->mc_LastKnownHalcCrcs.begin();
                                c_ItCur != this->mc_LastKnownHalcCrcs.end(); ++c_ItCur)
                           {
                              if (((c_ItCur->second.u32_Crc == u32_Hash) &&
                                   (c_ItCur->second.c_HalDpName.compare(rc_Dp.c_Name.c_str()) == 0)) &&
                                  ((pc_Node->c_HALCConfig.e_SafetyMode ==
                                    C_OSCHalcDefBase::eTWO_LEVELS_WITH_DROPPING) ||
                                   (c_ItCur->first.GetArrayElementIndexOrZero() == u32_ParameterElementIndex)))
                              {
                                 c_NewMap[c_NewId] = C_PuiSvLastKnownHalElementId(u32_Hash, rc_Dp.c_Name.c_str());
                                 c_MapCurToNew[c_ItCur->first] = c_NewId;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      //Check any removed elements
      for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
      {
         C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
         rc_View.OnSyncNodeHALC(ou32_Index, c_MapCurToNew);
      }
      //Store new map
      this->mc_LastKnownHalcCrcs = c_NewMap;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view node indices to deleted node index

   \param[in]  ou32_Index  Deleted node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAboutToBeDeleted(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAboutToBeDeleted(ou32_Index);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeAboutToBeDeleted(c_Id, ou32_Index);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusAdded(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusAdded(ou32_Index);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to deleted bus index

   \param[in]  ou32_Index  Deleted bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusDeleted(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusDeleted(ou32_Index);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAdded(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolAdded(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolSourceIndex   Source data pool index
   \param[in]  ou32_DataPoolTargetIndex   Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolSourceIndex,
                                               const uint32 ou32_DataPoolTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_DataPoolSourceIndex, ou32_DataPoolTargetIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolMoved(c_Id, ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                                     ou32_DataPoolTargetIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex);
   }
   // HALC
   {
      std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> c_Tmp;
      for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
              this->mc_LastKnownHalcCrcs.begin();
           c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id = c_It->first;
         C_PuiSvDashboard::h_OnSyncNodeDataPoolAboutToBeDeleted(c_Id, ou32_NodeIndex, ou32_DataPoolIndex);
         c_Tmp[c_Id] = c_It->second;
      }
      this->mc_LastKnownHalcCrcs = c_Tmp;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAdded(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_ApplicationSourceIndex   Application source index
   \param[in]  ou32_ApplicationTargetIndex   Application target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationMoved(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationSourceIndex,
                                                  const uint32 ou32_ApplicationTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationMoved(ou32_NodeIndex, ou32_ApplicationSourceIndex, ou32_ApplicationTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                             const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC(const uint32 ou32_NodeIndex,
                                                                             const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationChangedToParamSetHALC(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationChangedToParamSetHALC(ou32_NodeIndex, ou32_ApplicationIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_ApplicationIndex   Application index
   \param[in]  ou32_OldSize            Size of application result path before change
   \param[in]  ou32_NewSize            Size of application result path after change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationResultPathSizeChanged(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_ApplicationIndex,
                                                                  const stw_types::uint32 ou32_OldSize,
                                                                  const stw_types::uint32 ou32_NewSize)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationResultPathSizeChanged(ou32_NodeIndex, ou32_ApplicationIndex, ou32_OldSize,
                                                         ou32_NewSize);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  ou32_ListSourceIndex    Source list index
   \param[in]  ou32_ListTargetIndex    Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListSourceIndex, const uint32 ou32_ListTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                          ou32_ListTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_DataPoolIndex,
                                                              const uint32 ou32_ListIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_DataSetIndex);
   }
   //HALC not affected
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
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex,
                                                          const uint32 ou32_DataSetSourceIndex,
                                                          const uint32 ou32_DataSetTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_DataSetSourceIndex, ou32_DataSetTargetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_DataPoolIndex,
                                                                     const uint32 ou32_ListIndex,
                                                                     const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                            ou32_DataSetIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex);
   }
   //HALC not affected
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
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex,
                                                          const uint32 ou32_ElementSourceIndex,
                                                          const uint32 ou32_ElementTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_ElementSourceIndex, ou32_ElementTargetIndex);
   }
   //HALC not affected
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
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementArrayChanged(const uint32 ou32_NodeIndex,
                                                                 const uint32 ou32_DataPoolIndex,
                                                                 const uint32 ou32_ListIndex,
                                                                 const uint32 ou32_ElementIndex,
                                                                 const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                                 const bool oq_IsArray, const uint32 ou32_ArraySize,
                                                                 const bool oq_IsString)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementArrayChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                        ou32_ElementIndex, oe_Type, oq_IsArray, ou32_ArraySize,
                                                        oq_IsString);
   }
   //HALC not affected
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
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAccessChanged(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_DataPoolIndex,
                                                                  const uint32 ou32_ListIndex,
                                                                  const uint32 ou32_ElementIndex,
                                                                  const C_OSCNodeDataPoolListElement::E_Access oe_Access)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAccessChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                         ou32_ElementIndex, oe_Access);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_DataPoolIndex,
                                                                     const uint32 ou32_ListIndex,
                                                                     const uint32 ou32_ElementIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                            ou32_ElementIndex);
   }
   //HALC not affected
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   React to system definition clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncClear(void)
{
   this->mc_Views.clear();
   this->mc_LastKnownHalcCrcs.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value of the system definition

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvHandler::m_CalcHashSystemViews(void) const
{
   // init value of CRC
   uint32 u32_Hash = 0xFFFFFFFFU;

   for (std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId>::const_iterator c_It =
           this->mc_LastKnownHalcCrcs.begin();
        c_It != this->mc_LastKnownHalcCrcs.end(); ++c_It)
   {
      c_It->first.CalcHash(u32_Hash);
      c_It->second.CalcHash(u32_Hash);
      stw_scl::C_SCLChecksums::CalcCRC32(&c_It->second, sizeof(uint32), u32_Hash);
   }

   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].CalcHash(u32_Hash);
   }

   return u32_Hash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fix invalid rail configurations

   Hint: may be introduced by unknown issue not cleaning up rail configurations properly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_FixInvalidRailConfig(void)
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].FixInvalidRailConfig();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle compatibility chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::m_HandleCompatibilityChart(void)
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].HandleCompatibilityChart();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check routing

   \param[in]      ou32_ViewIndex      View index
   \param[out]     orq_RoutingError    Routing error
   \param[in,out]  orc_ErrorMessage    Error message

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::m_CheckRouting(const uint32 ou32_ViewIndex, bool & orq_RoutingError,
                                      QString & orc_ErrorMessage) const
{
   std::map<uint32, QString> c_ErrorDetails;
   const sint32 s32_Retval = this->CheckRouting(ou32_ViewIndex, c_ErrorDetails);
   QString c_Space = " ";

   //Error
   orq_RoutingError = c_ErrorDetails.size() != 0UL;

   //Message
   orc_ErrorMessage = C_GtGetText::h_GetText("Following nodes can not be reached by the PC:");
   for (std::map<uint32, QString>::const_iterator c_It = c_ErrorDetails.begin(); c_It != c_ErrorDetails.end(); ++c_It)
   {
      orc_ErrorMessage += c_Space;
      orc_ErrorMessage += c_It->second;
      c_Space = ", ";
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check routing and update nodes

   \param[in]      ou32_ViewIndex                        View index
   \param[out]     orq_RoutingError                      Routing error
   \param[in,out]  orc_ErrorMessage                      Error message
   \param[in,out]  orc_AutomaticallyDisabledSubDevices   Automatically disabled sub devices

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvHandler::m_CheckRoutingAndUpdateNodes(const uint32 ou32_ViewIndex, bool & orq_RoutingError,
                                                    QString & orc_ErrorMessage,
                                                    std::vector<uint32> & orc_AutomaticallyDisabledSubDevices)
{
   orc_AutomaticallyDisabledSubDevices.clear();
   //Only necessary for multi cpu
   if (C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().c_NodeSquads.size() > 0UL)
   {
      if (ou32_ViewIndex < this->mc_Views.size())
      {
         C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
         std::map<uint32, QString> c_ErrorDetails;
         //Step 1: activate all where only one active
         rc_View.ActivateAllRelevantSubDevices();
         //Step 2: check
         if (this->CheckRouting(ou32_ViewIndex, c_ErrorDetails) == C_NO_ERR)
         {
            //Step 3: deactivate sub devices where at least one sub device was successful
            orc_AutomaticallyDisabledSubDevices = rc_View.DeactivateSubDevicesBasedOnErrors(c_ErrorDetails);
         }
      }
   }
   return m_CheckRouting(ou32_ViewIndex, orq_RoutingError, orc_ErrorMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pointers to all currently registered view names

   \return
   Vector of pointers to all currently registered view names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<stw_scl::C_SCLString, bool> C_PuiSvHandler::m_GetExistingViewNames(void) const
{
   std::map<stw_scl::C_SCLString, bool> c_Retval;
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      const C_PuiSvData & rc_Data = this->mc_Views[u32_ItView];
      c_Retval[rc_Data.GetName().toStdString().c_str()] = true;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error detail result.

   \param[out]     opq_NameInvalid                       Name invalid
   \param[out]     opq_PCNotConnected                    PC not connected
   \param[out]     opq_RoutingInvalid                    Routing invalid
   \param[out]     opq_UpdateDisabledButDataBlocks       Update disabled though Data Block exists
   \param[out]     opq_SysDefInvalid                     System definition invalid
   \param[out]     opq_NoNodesActive                     No nodes active
   \param[in,out]  opc_RoutingErrorDetails               Routing error details
   \param[in,out]  opc_AutomaticallyDisabledSubDevices   Automatically disabled sub devices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::C_PuiSvViewErrorDetails::GetResults(bool * const opq_NameInvalid, bool * const opq_PCNotConnected,
                                                         bool * const opq_RoutingInvalid,
                                                         bool * const opq_UpdateDisabledButDataBlocks,
                                                         bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                                                         QString * const opc_RoutingErrorDetails,
                                                         QString * const opc_AutomaticallyDisabledSubDevices) const
{
   if (opq_NameInvalid != NULL)
   {
      *opq_NameInvalid = this->q_NameInvalid;
   }
   if (opq_PCNotConnected != NULL)
   {
      *opq_PCNotConnected = this->q_PCNotConnected;
   }
   if (opq_RoutingInvalid != NULL)
   {
      *opq_RoutingInvalid = this->q_RoutingInvalid;
      if (opc_RoutingErrorDetails != NULL)
      {
         *opc_RoutingErrorDetails = this->c_RoutingErrorDetails;
      }
   }
   if (opq_UpdateDisabledButDataBlocks != NULL)
   {
      *opq_UpdateDisabledButDataBlocks = this->q_UpdateDisabledButDataBlocks;
   }
   if (opq_SysDefInvalid != NULL)
   {
      *opq_SysDefInvalid = this->q_SysDefInvalid;
   }
   if (opq_NoNodesActive != NULL)
   {
      *opq_NoNodesActive = this->q_NoNodesActive;
   }
   if (opc_AutomaticallyDisabledSubDevices != NULL)
   {
      *opc_AutomaticallyDisabledSubDevices = this->c_AutomaticallyDisabledSubDevices;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Construct automatically disabled sub devices string

   \param[in]  orc_Indices    Indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvHandler::C_PuiSvViewErrorDetails::ConstructAutomaticallyDisabledSubDevicesString(
   const std::vector<uint32> & orc_Indices)
{
   this->c_AutomaticallyDisabledSubDevices = "";
   for (uint32 u32_ItIndex = 0UL; u32_ItIndex < orc_Indices.size(); ++u32_ItIndex)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(orc_Indices[u32_ItIndex]);
      if (pc_Node != NULL)
      {
         if (!this->c_AutomaticallyDisabledSubDevices.isEmpty())
         {
            this->c_AutomaticallyDisabledSubDevices += ", ";
         }
         this->c_AutomaticallyDisabledSubDevices += pc_Node->c_Properties.c_Name.c_str();
      }
   }
}

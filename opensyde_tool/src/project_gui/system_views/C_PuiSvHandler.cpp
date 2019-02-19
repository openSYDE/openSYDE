//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       System views data manager (implementation)

   System views data manager

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <iostream>

#include <QElapsedTimer>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_Uti.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvHandlerFiler.h"
#include "C_SyvRoRouteCalculation.h"
#include "C_OSCRoutingCalculation.h"
#include "C_OSCXMLParser.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_PuiSvHandler * C_PuiSvHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Load system views

   Load system views and store in information in our instance data.

   \param[in] orc_Path Path to system views file

   \return
   C_NO_ERR   data read and placed into instance data
   C_RD_WR    problems accessing file system (e.g. no read access to file)
   C_RANGE    specified file does not exist (when loading)
   C_NOACT    specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   content of file is invalid or incomplete

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::LoadFromFile(const QString & orc_Path)
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
            s32_Retval = C_PuiSvHandlerFiler::h_LoadViews(this->mc_Views, c_XMLParser);
            m_FixInvalidRailConfig();
            //calculate the hash value and save it for comparing
            this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
         }
         else
         {
            s32_Retval = C_RD_WR;
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

   if (s32_Retval == C_NO_ERR)
   {
      //calculate the hash value and save it for comparing
      this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save system views

   Save UI data part of system views to XML file.

   \param[in] orc_Path Path to system views file

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::SaveToFile(const QString & orc_Path)
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
      C_OSCXMLParser c_XMLParser;
      c_XMLParser.CreateNodeChild("opensyde-system-views");
      tgl_assert(c_XMLParser.SelectRoot() == "opensyde-system-views");
      c_XMLParser.CreateNodeChild("file-version", "1");
      C_PuiSvHandlerFiler::h_SaveViews(this->mc_Views, c_XMLParser);
      //calculate the hash value and save it for comparing
      this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();

      s32_Return = c_XMLParser.SaveToFile(orc_Path.toStdString().c_str());
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed

   \created     07.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get view

   \param[in] ou32_Index View index

   \return
   NULL View not found
   Else Valid view

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get number of views

   \return
   Number of views

   \created     11.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSvHandler::GetViewCount(void) const
{
   return this->mc_Views.size();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set view name

   \param[in] ou32_Index View index
   \param[in] orc_Name   View name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set checked flag for node in view

   \param[in] ou32_ViewIndex View index
   \param[in] ou32_NodeIndex Node index
   \param[in] oq_Checked     Checked flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set View PC UI box

   \param[in] ou32_Index View index
   \param[in] orc_Box    New PC UI box

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set View PC connection UI data

   \param[in] ou32_Index View index
   \param[in] orc_Line   New View PC connection UI data

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set connected state of pc in view

   \param[in] ou32_Index    View index
   \param[in] oq_Connected  Flag if pc is connected
   \param[in] ou32_BusIndex Bus index PC is connected to

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set path for the CAN DLL

   \param[in]     ou32_Index    View index
   \param[in]     orc_DllPath   Path for the CAN DLL

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard name

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Name            New dashboard name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard comment

   \param[in] ou32_ViewIndex        View index
   \param[in] ou32_DashboardIndex   Dashboard index
   \param[in] orc_Comment           New dashboard comment

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard active flag

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oq_Active           New active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     07.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dashboard widget box

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] ou32_WidgetIndex    Widget index
   \param[in] opc_Box             New widget value
   \param[in] oe_Type             New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set fast update rate

   \param[in] ou32_ViewIndex View index
   \param[in] ou16_Value     New fast update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set medium update rate

   \param[in] ou32_ViewIndex View index
   \param[in] ou16_Value     New medium update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set slow update rate

   \param[in] ou32_ViewIndex View index
   \param[in] ou16_Value     New slow update rate

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     28.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set view device config screen selected bit rate

   \param[in] ou32_ViewIndex View index
   \param[in] ou32_Value     New device config screen selected bit rate in kBit/s

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     13.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set view device config selected mode

   \param[in] ou32_ViewIndex View index (identifier)
   \param[in] ou32_Value     New device config screen selected mode

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     09.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set dark mode active flag

   \param[in] ou32_ViewIndex View index
   \param[in] ou16_Value     New dark mode active flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set view read rail assignment

   \param[in] ou32_ViewIndex View index
   \param[in] orc_Id         ID
   \param[in] orc_Config     Config

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     12.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set node update information

   \param[in] ou32_ViewIndex View index
   \param[in] orc_NodeUpdateInformation New node update information

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set node update information

   \param[in] ou32_ViewIndex              View index
   \param[in] ou32_Index                  Node index
   \param[in] orc_NodeUpdateInformation   New node update information

   \return
   C_NO_ERR    No error
   C_RANGE     Node index invalid

   \created     15.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add view

   \param[in] orc_View            View item
   \param[in] oq_AutoAdaptName    Optional flag to allow or block automatic adaption of input name
   \param[in] oq_AutoAdaptContent Optional flag to allow or block automatic adaption of input content

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::AddView(const C_PuiSvData & orc_View, const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent)
{
   tgl_assert(this->InsertView(this->mc_Views.size(), orc_View, oq_AutoAdaptName, oq_AutoAdaptContent) == C_NO_ERR);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set view

   The previous view will be overwritten

   \param[in] ou32_Index   View index
   \param[in] orc_View     View item

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     29.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert view

   \param[in] ou32_Index          View index
   \param[in] orc_View            View item
   \param[in] oq_AutoAdaptName    Optional flag to allow or block automatic adaption of input name
   \param[in] oq_AutoAdaptContent Optional flag to allow or block automatic adaption of input content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
         c_TmpView.InitFromSystemDefintion();
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new read rail item to configuration

   \param[in] ou32_ViewIndex View index
   \param[in] orc_Id         ID
   \param[in] orc_Config     Configuration

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
   C_NOACT  Already exists

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add dashboard to view

   \param[in] ou32_ViewIndex View index
   \param[in] orc_Dashboard  Dashboard data
   \param[in] oq_AutoAdapt   Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert dashboard to view

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Dashboard       Dashboard data
   \param[in] oq_AutoAdapt        Auto adapt name flag

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add widget to view dashboard

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] opc_Box             New widget value
   \param[in] oe_Type             New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert widget to view dashboard

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] ou32_WidgetIndex    Widget index
   \param[in] opc_Box             New widget value
   \param[in] oe_Type             New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Add new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Insert new item

   \param[in] ou32_Index          View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index
   \param[in] orc_Data            New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete view

   \param[in] ou32_Index View index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Remove read rail item configuration

   WARNING: No check if still in use

   \param[in] ou32_ViewIndex View index
   \param[in] orc_Id         ID

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete dashboard from view

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     06.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete widget from view dashboard

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] ou32_WidgetIndex    Widget index
   \param[in] oe_Type             New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delete item

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index
   \param[in] oru32_Index         Item index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Clear system views

   \created     10.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::Clear(void)
{
   this->mc_Views.clear();
   //Reset hash
   this->mu32_CalculatedHashSystemViews = this->m_CalcHashSystemViews();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move view

   \param[in] ou32_StartIndex  Start index
   \param[in] ou32_TargetIndex Target index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::MoveView(const uint32 ou32_StartIndex, const uint32 ou32_TargetIndex)
{
   sint32 s32_Retval;
   const C_PuiSvData * const pc_View = this->GetView(ou32_StartIndex);

   if (pc_View != NULL)
   {
      C_PuiSvData c_View = *pc_View;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Synchronise internally stored scaling information with current system definition

   \param[in] ou32_ViewIndex      View index
   \param[in] ou32_DashboardIndex Dashboard index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     06.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Update system definition errors

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get buffered error for node

   \param[in] ou32_Index Index

   \return
   Buffered error for node

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorNode(const uint32 ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdNodeErrors.size())
   {
      q_Retval = this->mc_SdNodeErrors[ou32_Index];
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get buffered error for bus

   \param[in] ou32_Index Index

   \return
   Buffered error for bus

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvHandler::GetErrorBus(const uint32 ou32_Index) const
{
   bool q_Retval = true;

   if (ou32_Index < this->mc_SdBusErrors.size())
   {
      q_Retval = this->mc_SdBusErrors[ou32_Index];
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check view error

   \param[in]     ou32_Index                        View index
   \param[in,out] opq_NameInvalid                   Name conflict
   \param[in,out] opq_PCNotConnected                PC not connected
   \param[in,out] opq_RoutingInvalid                Routing invalid
   \param[in,out] opq_UpdateDisabledButDataBlocks   No interface supports update but there are data blocks
   \param[in,out] opq_SysDefInvalid                 System definition invalid
   \param[in,out] opq_NoNodesActive                 No nodes active
   \param[in,out] opc_RoutingErrorDetails           Error details for routing error if any
                                                     Warning: only set if routing error checked

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckViewError(const uint32 ou32_Index, bool * const opq_NameInvalid,
                                      bool * const opq_PCNotConnected, bool * const opq_RoutingInvalid,
                                      bool * const opq_UpdateDisabledButDataBlocks, bool * const opq_SysDefInvalid,
                                      bool * const opq_NoNodesActive, QString * const opc_RoutingErrorDetails)
{
   sint32 s32_Retval = C_NO_ERR;
   QElapsedTimer c_Timer;

   if (mq_TIMING_OUTPUT)
   {
      c_Timer.start();
   }

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
         tgl_assert(this->CheckRouting(ou32_Index, q_Error, c_Text) == C_NO_ERR);
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

         //Return error details
         c_Details.GetResults(opq_NameInvalid, opq_PCNotConnected, opq_RoutingInvalid,
                              opq_UpdateDisabledButDataBlocks, opq_SysDefInvalid, opq_NoNodesActive,
                              opc_RoutingErrorDetails);
         //Store results
         this->mc_PreviousErrorCheckResults.insert(u32_Hash, c_Details);
      }
      else
      {
         c_It.value().GetResults(opq_NameInvalid, opq_PCNotConnected, opq_RoutingInvalid,
                                 opq_UpdateDisabledButDataBlocks, opq_SysDefInvalid, opq_NoNodesActive,
                                 opc_RoutingErrorDetails);
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   if (mq_TIMING_OUTPUT)
   {
      std::cout << "CheckView " << c_Timer.elapsed() << " ms" << &std::endl;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check update possible

   \param[in]  ou32_ViewIndex                  View index (identifier)
   \param[out] orq_UpdateDisabledButDataBlocks Data blocks but no interface updateable
   \param[out] orc_ErrorMessage                Error details (affected nodes)

   \created     09.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check routing error

   \param[in]  ou32_ViewIndex   View index
   \param[out] orq_RoutingError Routing error
   \param[out] orc_Message      Error message

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     28.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckRouting(const uint32 ou32_ViewIndex, bool & orq_RoutingError,
                                    QString & orc_ErrorMessage) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
      const std::vector<uint8> & rc_ActiveNodes = rc_View.GetNodeActiveFlags();
      uint32 u32_Counter;
      QString c_Space = " ";

      orq_RoutingError = false;
      orc_ErrorMessage = C_GtGetText::h_GetText("Following nodes can not be reached by the PC:");

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

               if ((c_RouteCalcUpdate.WasPcAvailable() == false) ||
                   ((c_RouteCalcUpdate.GetState() != C_NO_ERR) &&
                    (c_RouteCalcUpdate.GetState() != C_NOACT)))
               {
                  orq_RoutingError = true;
                  orc_ErrorMessage += c_Space + QString(pc_Node->c_Properties.c_Name.c_str()) + " (Update)";
                  c_Space = ", ";
               }

               if ((c_RouteCalcDiag.WasPcAvailable() == false) ||
                   ((c_RouteCalcDiag.GetState() != C_NO_ERR) &&
                    (c_RouteCalcDiag.GetState() != C_NOACT)))
               {
                  orq_RoutingError = true;
                  orc_ErrorMessage += c_Space + QString(pc_Node->c_Properties.c_Name.c_str()) + " (Diagnostic)";
                  c_Space = ", ";
               }
            }
            else
            {
               // Should not happen
               orq_RoutingError = true;
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check view reconnect necessary

   \param[in]  ou32_ViewIndex         View index
   \param[out] orq_ReconnectNecessary Flag if reconnect necessary

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     09.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvHandler::CheckViewReconnectNecessary(const uint32 ou32_ViewIndex, bool & orq_ReconnectNecessary)
{
   sint32 s32_Retval = C_NO_ERR;

   orq_ReconnectNecessary = false;

   if (ou32_ViewIndex < this->mc_Views.size())
   {
      const C_PuiSvData & rc_View = this->mc_Views[ou32_ViewIndex];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check bus for disabled look

   \param[in] ou32_ViewIndex View index
   \param[in] ou32_BusIndex  Bus index

   \return
   True  Disabled
   False Enabled

   \created     28.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   View dashboard param widget clear all data pool elements

   \param[in] ou32_ViewIndex        View index
   \param[in] ou32_DashboardIndex   Dashboard index
   \param[in] ou32_ParamWidgetIndex Param index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     05.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   View dashboard param widget add new data pool element

   \param[in] ou32_ViewIndex        View index
   \param[in] ou32_DashboardIndex   Dashboard index
   \param[in] ou32_ParamWidgetIndex Param index
   \param[in] orc_NewId             New ID
   \param[in] opc_Content           Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get hash for view

   \param[in] ou32_ViewIndex Index

   \return
   Hash for view

   \created     22.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvHandler * C_PuiSvHandler::h_GetInstance(void)
{
   if (C_PuiSvHandler::mhpc_Singleton == NULL)
   {
      C_PuiSvHandler::mhpc_Singleton = new C_PuiSvHandler();
   }
   return C_PuiSvHandler::mhpc_Singleton;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up singleton

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::h_Destroy(void)
{
   if (C_PuiSvHandler::mhpc_Singleton != NULL)
   {
      delete (C_PuiSvHandler::mhpc_Singleton);
      C_PuiSvHandler::mhpc_Singleton = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     21.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvHandler::C_PuiSvHandler(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mu32_CalculatedHashSystemViews(0)
{
   //Connects for synchronisation
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigSyncNodeAdded, this,
           &C_PuiSvHandler::m_OnSyncNodeAdded);
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
   //lint -e{64,918,1025,1703} Interface does match
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvHandler::~C_PuiSvHandler(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view node indices to added node index

   \param[in] ou32_Index Added node index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAdded(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAdded(ou32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view node indices to deleted node index

   \param[in] ou32_Index Deleted node index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeAboutToBeDeleted(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeAboutToBeDeleted(ou32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view bus index to added bus index

   \param[in] ou32_Index Added bus index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusAdded(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusAdded(ou32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view bus index to deleted bus index

   \param[in] ou32_Index Deleted bus index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncBusDeleted(const uint32 ou32_Index)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncBusDeleted(ou32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAdded(ou32_NodeIndex, ou32_DataPoolIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex           Node index
   \param[in] ou32_DataPoolSourceIndex Source data pool index
   \param[in] ou32_DataPoolTargetIndex Target data pool index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolSourceIndex,
                                               const uint32 ou32_DataPoolTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolMoved(ou32_NodeIndex, ou32_DataPoolSourceIndex, ou32_DataPoolTargetIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAdded(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAdded(ou32_NodeIndex, ou32_ApplicationIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex              Node index
   \param[in] ou32_ApplicationSourceIndex Application source index
   \param[in] ou32_ApplicationTargetIndex Application target index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationMoved(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationSourceIndex,
                                                  const uint32 ou32_ApplicationTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationMoved(ou32_NodeIndex, ou32_ApplicationSourceIndex, ou32_ApplicationTargetIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index

   \created     14.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeApplicationAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                             const uint32 ou32_ApplicationIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeApplicationAboutToBeDeleted(ou32_NodeIndex, ou32_ApplicationIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex       Node index
   \param[in] ou32_DataPoolIndex   Data pool index
   \param[in] ou32_ListSourceIndex Source list index
   \param[in] ou32_ListTargetIndex Target list index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListSourceIndex, const uint32 ou32_ListTargetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListMoved(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                          ou32_ListTargetIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_DataPoolIndex,
                                                              const uint32 ou32_ListIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListAboutToBeDeleted(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_DataSetIndex  Data set index

   \created     10.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListDataSetAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListDataSetAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_DataSetIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex          Node index
   \param[in] ou32_DataPoolIndex      Data pool index
   \param[in] ou32_ListIndex          List index
   \param[in] ou32_DataSetSourceIndex Source data set index
   \param[in] ou32_DataSetTargetIndex Target data set index

   \created     10.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_DataSetIndex  Data set index

   \created     10.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementAdded(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex          Node index
   \param[in] ou32_DataPoolIndex      Data pool index
   \param[in] ou32_ListIndex          List index
   \param[in] ou32_ElementSourceIndex Source element index
   \param[in] ou32_ElementTargetIndex Target element index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index
   \param[in] oe_Type            New element type
   \param[in] oq_IsArray         New array type
   \param[in] ou32_ArraySize     New array size

   \created     28.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncNodeDataPoolListElementArrayChanged(const uint32 ou32_NodeIndex,
                                                                 const uint32 ou32_DataPoolIndex,
                                                                 const uint32 ou32_ListIndex,
                                                                 const uint32 ou32_ElementIndex,
                                                                 const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                                 const bool oq_IsArray, const uint32 ou32_ArraySize)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_Views.size(); ++u32_ItView)
   {
      C_PuiSvData & rc_View = this->mc_Views[u32_ItView];
      rc_View.OnSyncNodeDataPoolListElementArrayChanged(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                        ou32_ElementIndex, oe_Type, oq_IsArray, ou32_ArraySize);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index
   \param[in] oe_Access          New access type

   \created     12.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt to system definition change

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   React to system definition clear

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_OnSyncClear(void)
{
   this->mc_Views.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value of the system definition

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_PuiSvHandler::m_CalcHashSystemViews(void) const
{
   // init value of CRC
   uint32 u32_Hash = 0xFFFFFFFFU;

   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].CalcHash(u32_Hash);
   }

   return u32_Hash;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Fix invalid rail configurations

   Hint: may be introduced by unknown issue not cleaning up rail configurations properly

   \created     18.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::m_FixInvalidRailConfig(void)
{
   for (uint32 u32_Counter = 0U; u32_Counter < this->mc_Views.size(); ++u32_Counter)
   {
      this->mc_Views[u32_Counter].FixInvalidRailConfig();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get pointers to all currently registered view names

   \return
   Vector of pointers to all currently registered view names

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   short description of function

   long description of function within several lines

   \param[in]     ou8_Aa         input parameter description
   \param[out]    opu32_Bb       output parameter description
   \param[in,out] opu16_Cc       input/output parameter description

   \return
   possible return value(s) and description

   \created     22.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvHandler::C_PuiSvViewErrorDetails::GetResults(bool * const opq_NameInvalid, bool * const opq_PCNotConnected,
                                                         bool * const opq_RoutingInvalid,
                                                         bool * const opq_UpdateDisabledButDataBlocks,
                                                         bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                                                         QString * const opc_RoutingErrorDetails) const
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
}

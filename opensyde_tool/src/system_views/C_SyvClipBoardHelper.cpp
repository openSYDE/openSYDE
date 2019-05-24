//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (implementation)

   Handle clipboard load and save

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCXMLParser.h"
#include "C_SyvClipBoardHelper.h"
#include "C_PuiSvHandlerFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
/*! \brief   Store system view dashboard to clip board

   \param[in] orc_Data           System view dashboard
   \param[in] orc_Rails          System view dashboard rails
   \param[in] orc_GenericTagName Generic content tag name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvClipBoardHelper::h_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                                       const QMap<C_OSCNodeDataPoolListElementId,
                                                                  C_PuiSvReadDataConfiguration> & orc_Rails,
                                                       const QString & orc_GenericTagName)
{
   stw_scl::C_SCLString c_XMLContent;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.CreateAndSelectNodeChild(orc_GenericTagName.toStdString().c_str());
   c_StringXml.CreateAndSelectNodeChild("rail-assignments");
   C_PuiSvHandlerFiler::h_SaveReadRails(orc_Rails, c_StringXml);
   c_StringXml.SelectNodeParent();
   //System definition compatibility
   c_StringXml.CreateAndSelectNodeChild("gui-only");
   C_PuiSvHandlerFiler::h_SaveDashboard(orc_Data, c_StringXml);
   c_StringXml.SaveToString(c_XMLContent);
   mh_SetClipBoard(c_XMLContent.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out] orc_Data           System view dashboard
   \param[in]  orc_GenericTagName Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                          const QString & orc_GenericTagName)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == orc_GenericTagName.toStdString().c_str())
   {
      if (c_StringXml.SelectNodeChild("gui-only") == "gui-only")
      {
         s32_Retval = C_PuiSvHandlerFiler::h_LoadDashboard(orc_Data, c_StringXml, true);
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system view dashboard from clip board

   \param[out] orc_Rails          System view dashboard rails
   \param[in]  orc_GenericTagName Generic content tag name

   \return
   C_NO_ERR Found and loaded
   C_CONFIG Clipboard invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvClipBoardHelper::h_LoadRailsFromClipboard(QMap<C_OSCNodeDataPoolListElementId,
                                                           C_PuiSvReadDataConfiguration> & orc_Rails,
                                                      const QString & orc_GenericTagName)
{
   sint32 s32_Retval = C_NO_ERR;
   C_OSCXMLParserString c_StringXml;

   c_StringXml.LoadFromString(mh_GetClipBoard().toStdString().c_str());

   if (c_StringXml.SelectRoot() == orc_GenericTagName.toStdString().c_str())
   {
      if (c_StringXml.SelectNodeChild("rail-assignments") == "rail-assignments")
      {
         C_PuiSvHandlerFiler::h_LoadReadRails(orc_Rails, c_StringXml);
         c_StringXml.SelectNodeParent();
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvClipBoardHelper::C_SyvClipBoardHelper(void) :
   C_UtiClipBoardHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for configuration clipboard actions (implementation)

   Utility for configuration clipboard actions

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_CamProHandlerFiler.h"
#include "C_CamProClipBoardHelper.h"

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
/*! \brief   Save messages

   \param[in]     orc_Messages  Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProClipBoardHelper::h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages)
{
   C_OSCXMLParserString c_Parser;

   mh_GetNewParser(c_Parser);
   C_CamProHandlerFiler::h_SaveMessages(orc_Messages, c_Parser);
   mh_StoreParserInClipBoard(c_Parser);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load messages

   \param[out]    orc_Messages  Data

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProClipBoardHelper::h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages)
{
   sint32 s32_Retval;
   C_OSCXMLParserString c_Parser;

   s32_Retval = mh_GetParserFromClipBoard(c_Parser);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_CamProHandlerFiler::h_LoadMessages(orc_Messages, c_Parser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProClipBoardHelper::C_CamProClipBoardHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create fresh clipboard root for can monitor

   \param[out] orc_XMLParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProClipBoardHelper::mh_GetNewParser(C_OSCXMLParserString & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("opensyde-can-monitor-configuration-clip-board");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current xml content as clipboard content

   \param[in] orc_XMLParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProClipBoardHelper::mh_StoreParserInClipBoard(const C_OSCXMLParserString & orc_XMLParser)
{
   stw_scl::C_SCLString c_String;
   orc_XMLParser.SaveToString(c_String);
   C_CamProClipBoardHelper::mh_SetClipBoard(c_String.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get xml parser from clipboard if possible

   \param[out] orc_XMLParser XML parser

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProClipBoardHelper::mh_GetParserFromClipBoard(C_OSCXMLParserString & orc_XMLParser)
{
   sint32 s32_Retval = C_CONFIG;
   const QString c_Content = C_UtiClipBoardHelper::mh_GetClipBoard();

   if (orc_XMLParser.LoadFromString(c_Content.toStdString().c_str()) == C_NO_ERR)
   {
      if (orc_XMLParser.SelectRoot() == "opensyde-can-monitor-configuration-clip-board")
      {
         s32_Retval = C_NO_ERR;
      }
   }

   return s32_Retval;
}

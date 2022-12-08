//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for configuration clipboard actions (implementation)

   Utility for configuration clipboard actions

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CamProHandlerFiler.hpp"
#include "C_CamProClipBoardHelper.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   C_OscXmlParser c_Parser;

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
int32_t C_CamProClipBoardHelper::h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages)
{
   int32_t s32_Retval;
   C_OscXmlParser c_Parser;

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

   \param[out] orc_XmlParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProClipBoardHelper::mh_GetNewParser(C_OscXmlParser & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("opensyde-can-monitor-configuration-clip-board");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current xml content as clipboard content

   \param[in] orc_XmlParser XML parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProClipBoardHelper::mh_StoreParserInClipBoard(const C_OscXmlParser & orc_XmlParser)
{
   stw::scl::C_SclString c_String;
   orc_XmlParser.SaveToString(c_String);
   C_CamProClipBoardHelper::mh_SetClipBoard(c_String.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get xml parser from clipboard if possible

   \param[out] orc_XmlParser XML parser

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamProClipBoardHelper::mh_GetParserFromClipBoard(C_OscXmlParser & orc_XmlParser)
{
   int32_t s32_Retval = C_CONFIG;
   const QString c_Content = C_UtiClipBoardHelper::mh_GetClipBoard();

   if (orc_XmlParser.LoadFromString(c_Content.toStdString().c_str()) == C_NO_ERR)
   {
      if (orc_XmlParser.SelectRoot() == "opensyde-can-monitor-configuration-clip-board")
      {
         s32_Retval = C_NO_ERR;
      }
   }

   return s32_Retval;
}

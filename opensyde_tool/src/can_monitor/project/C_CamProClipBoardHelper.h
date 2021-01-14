//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for configuration clipboard actions (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROCLIPBOARDHELPER_H
#define C_CAMPROCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCXMLParser.h"
#include "C_CamProMessageData.h"
#include "C_UtiClipBoardHelper.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages);
   static stw_types::sint32 h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages);

private:
   C_CamProClipBoardHelper(void);

   static void mh_GetNewParser(stw_opensyde_core::C_OSCXMLParserString & orc_XMLParser);
   static void mh_StoreParserInClipBoard(const stw_opensyde_core::C_OSCXMLParserString & orc_XMLParser);
   static stw_types::sint32 mh_GetParserFromClipBoard(stw_opensyde_core::C_OSCXMLParserString & orc_XMLParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

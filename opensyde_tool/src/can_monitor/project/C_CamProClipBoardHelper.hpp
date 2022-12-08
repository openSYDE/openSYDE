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
#include "C_OscXmlParser.hpp"
#include "C_CamProMessageData.hpp"
#include "C_UtiClipBoardHelper.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamProClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_SaveMessages(const std::vector<C_CamProMessageData> & orc_Messages);
   static int32_t h_LoadMessages(std::vector<C_CamProMessageData> & orc_Messages);

private:
   C_CamProClipBoardHelper(void);

   static void mh_GetNewParser(stw::opensyde_core::C_OscXmlParser & orc_XmlParser);
   static void mh_StoreParserInClipBoard(const stw::opensyde_core::C_OscXmlParser & orc_XmlParser);
   static int32_t mh_GetParserFromClipBoard(stw::opensyde_core::C_OscXmlParser & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

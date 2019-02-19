//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to copy a message and its detected signals to the clipboard as text (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMETCLIPBOARDHELPER_H
#define C_CAMMETCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "C_UtiClipBoardHelper.h"

#include "C_OSCComMessageLoggerData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMetClipBoardHelperCanMessageData
{
public:
   C_CamMetClipBoardHelperCanMessageData() :
      pc_MessageData(NULL),
      q_Extended(false)
   {
   }

   const stw_opensyde_core::C_OSCComMessageLoggerData * pc_MessageData;
   bool q_Extended;
};

class C_CamMetClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreCanMessages(const bool oq_DisplayAsHex, const bool oq_DisplayTimestampRelative,
                                  const std::vector<C_CamMetClipBoardHelperCanMessageData> & orc_MessageData);

protected:
   C_CamMetClipBoardHelper(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

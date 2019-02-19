//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Class with concrete implementation for BLF log files (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVCOMMESSAGELOGGERFILEBLF_H
#define C_SYVCOMMESSAGELOGGERFILEBLF_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OSCComMessageLoggerFileBase.h"

#include "BLF.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvComMessageLoggerFileBlf :
      public stw_opensyde_core::C_OSCComMessageLoggerFileBase
{
public:
   C_SyvComMessageLoggerFileBlf(const stw_scl::C_SCLString & orc_FilePath);
   virtual ~C_SyvComMessageLoggerFileBlf(void);

   virtual stw_types::sint32 OpenFile(void) override;
   virtual void AddMessageToFile(const stw_opensyde_core::C_OSCComMessageLoggerData & orc_MessageData) override;

private:
   Vector::BLF::File mc_File;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

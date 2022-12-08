//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with concrete implementation for BLF log files (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMMESSAGELOGGERFILEBLF_H
#define C_SYVCOMMESSAGELOGGERFILEBLF_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OscComMessageLoggerFileBase.hpp"

#include "BLF.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComMessageLoggerFileBlf :
   public stw::opensyde_core::C_OscComMessageLoggerFileBase
{
public:
   C_SyvComMessageLoggerFileBlf(const stw::scl::C_SclString & orc_FilePath = "");
   ~C_SyvComMessageLoggerFileBlf(void) noexcept override;

   int32_t OpenFile(void) override;
   void AddMessageToFile(const stw::opensyde_core::C_OscComMessageLoggerData & orc_MessageData) override;

private:
   Vector::BLF::File mc_File;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

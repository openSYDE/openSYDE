//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODESTWFLASHLOADEROPTIONS_H
#define C_OSCNODESTWFLASHLOADEROPTIONS_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeStwFlashloaderSettings
{
public:
   C_OSCNodeStwFlashloaderSettings(void);

   //lint -sem(stw_opensyde_core::C_OSCNodeStwFlashloaderSettings::Initialize,initializer)
   void Initialize(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_ResetMessageActive;
   bool q_ResetMessageExtendedId;
   stw_types::uint32 u32_ResetMessageId;
   stw_types::uint8 u8_ResetMessageDlc;
   std::vector<stw_types::uint8> c_Data;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

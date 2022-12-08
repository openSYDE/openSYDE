//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update information for parameter sets (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVNODEUPDATEPARAMINFO_H
#define C_PUISVNODEUPDATEPARAMINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvNodeUpdateParamInfo
{
public:
   C_PuiSvNodeUpdateParamInfo(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   //Set
   void SetContent(const QString & orc_FilePath, const uint32_t ou32_LastKnownCrc);

   //Get
   const QString & GetPath(void) const;
   uint32_t GetLastKnownCrc(void) const;

private:
   QString mc_FilePath;
   uint32_t mu32_LastKnownCrc;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

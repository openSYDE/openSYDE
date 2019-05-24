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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvNodeUpdateParamInfo
{
public:
   C_PuiSvNodeUpdateParamInfo(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   //Set
   void SetContent(const QString & orc_FilePath, const stw_types::uint32 ou32_LastKnownCrc);

   //Get
   const QString & GetPath(void) const;
   stw_types::uint32 GetLastKnownCrc(void) const;

private:
   QString mc_FilePath;
   stw_types::uint32 mu32_LastKnownCrc;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

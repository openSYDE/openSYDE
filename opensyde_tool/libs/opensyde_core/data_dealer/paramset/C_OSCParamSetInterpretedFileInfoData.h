//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for interpreted param set file info data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDFILEINFODATA_H
#define C_OSCPARAMSETINTERPRETEDFILEINFODATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCParamSetInterpretedFileInfoData
{
public:
   C_OSCParamSetInterpretedFileInfoData(void);

   void Clear(void);
   void AddInterpretedFileData(const stw_scl::C_SCLString & orc_DateTime, const stw_scl::C_SCLString & orc_Creator,
                               const stw_scl::C_SCLString & orc_ToolName, const stw_scl::C_SCLString & orc_ToolVersion,
                               const stw_scl::C_SCLString & orc_ProjectName,
                               const stw_scl::C_SCLString & orc_ProjectVersion,
                               const stw_scl::C_SCLString & orc_UserComment);

   //Date and time of creation
   stw_scl::C_SCLString c_DateTime;
   //User who created this file
   stw_scl::C_SCLString c_Creator;
   //Name of tool used for creating this param set file
   stw_scl::C_SCLString c_ToolName;
   //Version of tool used for creating this param set file
   stw_scl::C_SCLString c_ToolVersion;
   //Name of project used for creating this param set file
   stw_scl::C_SCLString c_ProjectName;
   //Version of project used for creating this param set file
   stw_scl::C_SCLString c_ProjectVersion;
   //Optional user comment for this param set file
   stw_scl::C_SCLString c_UserComment;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

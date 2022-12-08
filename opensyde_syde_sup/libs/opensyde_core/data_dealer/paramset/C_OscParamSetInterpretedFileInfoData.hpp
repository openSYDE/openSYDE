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
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscParamSetInterpretedFileInfoData
{
public:
   C_OscParamSetInterpretedFileInfoData(void);

   void Clear(void);
   void AddInterpretedFileData(const stw::scl::C_SclString & orc_DateTime, const stw::scl::C_SclString & orc_Creator,
                               const stw::scl::C_SclString & orc_ToolName,
                               const stw::scl::C_SclString & orc_ToolVersion,
                               const stw::scl::C_SclString & orc_ProjectName,
                               const stw::scl::C_SclString & orc_ProjectVersion,
                               const stw::scl::C_SclString & orc_UserComment);

   //Date and time of creation
   stw::scl::C_SclString c_DateTime;
   //User who created this file
   stw::scl::C_SclString c_Creator;
   //Name of tool used for creating this param set file
   stw::scl::C_SclString c_ToolName;
   //Version of tool used for creating this param set file
   stw::scl::C_SclString c_ToolVersion;
   //Name of project used for creating this param set file
   stw::scl::C_SclString c_ProjectName;
   //Version of project used for creating this param set file
   stw::scl::C_SclString c_ProjectVersion;
   //Optional user comment for this param set file
   stw::scl::C_SclString c_UserComment;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

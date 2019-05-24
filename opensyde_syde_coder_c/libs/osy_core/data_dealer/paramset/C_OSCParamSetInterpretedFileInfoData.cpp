//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for interpreted param set file info data (implementation)

   Storage for interpreted param set file info data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCParamSetInterpretedFileInfoData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCParamSetInterpretedFileInfoData::C_OSCParamSetInterpretedFileInfoData(void)
{
   this->Clear();
}

//-----------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//-----------------------------------------------------------------------------
void C_OSCParamSetInterpretedFileInfoData::Clear(void)
{
   this->c_DateTime = "Do Jan 01 00:00:00 1970";
   this->c_Creator = "unknown";
   this->c_ToolName = "unknown";
   this->c_ToolVersion = "unknown";
   this->c_ProjectName = "unknown";
   this->c_ProjectVersion = "unknown";
   this->c_UserComment = "";
}

//-----------------------------------------------------------------------------
/*! \brief   Add some general information for one param set file

   \param[in] orc_DateTime       Date and time of creation
   \param[in] orc_Creator        User who created this file
   \param[in] orc_ToolName       Name of tool used for creating this param set file
   \param[in] orc_ToolVersion    Version of tool used for creating this param set file
   \param[in] orc_ProjectName    Name of project used for creating this param set file
   \param[in] orc_ProjectVersion Version of project used for creating this param set file
   \param[in] orc_UserComment    Optional user comment for this param set file
*/
//-----------------------------------------------------------------------------
void C_OSCParamSetInterpretedFileInfoData::AddInterpretedFileData(const stw_scl::C_SCLString & orc_DateTime,
                                                                  const stw_scl::C_SCLString & orc_Creator,
                                                                  const stw_scl::C_SCLString & orc_ToolName,
                                                                  const stw_scl::C_SCLString & orc_ToolVersion,
                                                                  const stw_scl::C_SCLString & orc_ProjectName,
                                                                  const stw_scl::C_SCLString & orc_ProjectVersion,
                                                                  const stw_scl::C_SCLString & orc_UserComment)
{
   this->c_DateTime = orc_DateTime;
   this->c_Creator = orc_Creator;
   this->c_ToolName = orc_ToolName;
   this->c_ToolVersion = orc_ToolVersion;
   this->c_ProjectName = orc_ProjectName;
   this->c_ProjectVersion = orc_ProjectVersion;
   this->c_UserComment = orc_UserComment;
}

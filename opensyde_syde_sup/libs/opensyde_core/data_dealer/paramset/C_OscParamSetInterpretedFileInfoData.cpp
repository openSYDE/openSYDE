//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for interpreted param set file info data (implementation)

   Storage for interpreted param set file info data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscParamSetInterpretedFileInfoData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscParamSetInterpretedFileInfoData::C_OscParamSetInterpretedFileInfoData(void)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear internally stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedFileInfoData::Clear(void)
{
   this->c_DateTime = "Do Jan 01 00:00:00 1970";
   this->c_Creator = "unknown";
   this->c_ToolName = "unknown";
   this->c_ToolVersion = "unknown";
   this->c_ProjectName = "unknown";
   this->c_ProjectVersion = "unknown";
   this->c_UserComment = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add some general information for one param set file

   \param[in] orc_DateTime       Date and time of creation
   \param[in] orc_Creator        User who created this file
   \param[in] orc_ToolName       Name of tool used for creating this param set file
   \param[in] orc_ToolVersion    Version of tool used for creating this param set file
   \param[in] orc_ProjectName    Name of project used for creating this param set file
   \param[in] orc_ProjectVersion Version of project used for creating this param set file
   \param[in] orc_UserComment    Optional user comment for this param set file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscParamSetInterpretedFileInfoData::AddInterpretedFileData(const stw::scl::C_SclString & orc_DateTime,
                                                                  const stw::scl::C_SclString & orc_Creator,
                                                                  const stw::scl::C_SclString & orc_ToolName,
                                                                  const stw::scl::C_SclString & orc_ToolVersion,
                                                                  const stw::scl::C_SclString & orc_ProjectName,
                                                                  const stw::scl::C_SclString & orc_ProjectVersion,
                                                                  const stw::scl::C_SclString & orc_UserComment)
{
   this->c_DateTime = orc_DateTime;
   this->c_Creator = orc_Creator;
   this->c_ToolName = orc_ToolName;
   this->c_ToolVersion = orc_ToolVersion;
   this->c_ProjectName = orc_ProjectName;
   this->c_ProjectVersion = orc_ProjectVersion;
   this->c_UserComment = orc_UserComment;
}

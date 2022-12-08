//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with generic interface to handle and write a CAN log file (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstdio>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "C_OscUtils.hpp"
#include "C_OscComMessageLoggerFileBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_FilePath      Complete path with file name of logging file
   \param[in]  orc_ProtocolName  Name of current set protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerFileBase::C_OscComMessageLoggerFileBase(const stw::scl::C_SclString & orc_FilePath,
                                                             const C_SclString & orc_ProtocolName) :
   mc_FilePath(orc_FilePath),
   mc_ProtocolName(orc_ProtocolName)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerFileBase::~C_OscComMessageLoggerFileBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates, if necessary, and opens file and adds the default header of the file.

   Base implementation only creates the folders and deletes an already existing file

   \return
   C_NO_ERR    No errors
   C_RD_WR     Error on creating folders or deleting old file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscComMessageLoggerFileBase::OpenFile(void)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_FolderPath = TglExtractFilePath(this->mc_FilePath);

   // Check and create folder
   if (TglDirectoryExists(c_FolderPath) == false)
   {
      s32_Return = C_OscUtils::h_CreateFolderRecursively(c_FolderPath);

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   else if (TglFileExists(this->mc_FilePath) == true)
   {
      // Delete the old file
      if (remove(this->mc_FilePath.c_str()) != 0)
      {
         s32_Return = C_RD_WR;
      }
   }
   else
   {
      // Nothing to do
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current protocol name

   \param[in]     orc_ProtocolName         Current protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscComMessageLoggerFileBase::SetProtocolName(const C_SclString & orc_ProtocolName)
{
   this->mc_ProtocolName = orc_ProtocolName;
}

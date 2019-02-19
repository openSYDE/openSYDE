//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class with generic interface to handle and write a CAN log file (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cstdio>

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLFile.h"
#include "C_OSCUtils.h"
#include "C_OSCComMessageLoggerFileBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]  orc_FilePath      Complete path with file name of logging file
   \param[in]  orc_ProtocolName  Name of current set protocol

   \created     13.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerFileBase::C_OSCComMessageLoggerFileBase(const stw_scl::C_SCLString & orc_FilePath,
                                                             const C_SCLString & orc_ProtocolName) :
   mc_FilePath(orc_FilePath),
   mc_ProtocolName(orc_ProtocolName)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     13.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerFileBase::~C_OSCComMessageLoggerFileBase(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Creates, if necessary, and opens file and adds the default header of the file.

   Base implementation only creates the folders and deletes an already existing file

   \return
   C_NO_ERR    No errors
   C_RD_WR     Error on creating folders or deleting old file

   \created     14.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_OSCComMessageLoggerFileBase::OpenFile(void)
{
   sint32 s32_Return = C_NO_ERR;
   const C_SCLString c_FolderPath = TGL_ExtractFilePath(this->mc_FilePath);

   // Check and create folder
   if (TGL_DirectoryExists(c_FolderPath) == false)
   {
      s32_Return = C_OSCUtils::h_CreateFolderRecursively(c_FolderPath);

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   else if (TGL_FileExists(this->mc_FilePath) == true)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the current protocol name

   \param[in]     orc_ProtocolName         Current protocol name

   \created     18.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLoggerFileBase::SetProtocolName(const C_SCLString & orc_ProtocolName)
{
   this->mc_ProtocolName = orc_ProtocolName;
}

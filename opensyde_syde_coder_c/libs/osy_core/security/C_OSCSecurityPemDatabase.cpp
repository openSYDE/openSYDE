//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files

   Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLFile.h"
#include "stwerrors.h"
#include "CSCLStringList.h"
#include "C_OSCSecurityPem.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCSecurityPemDatabase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSecurityPemDatabase::C_OSCSecurityPemDatabase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get size of database

   \return
   Size of database
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCSecurityPemDatabase::GetSizeOfDatabase() const
{
   return this->mc_StoredPemFiles.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pem file by serial number

   \param[in]  orc_SerialNumber  Serial number

   \return
   PEM file by serial number
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCSecurityPemKeyInfo * C_OSCSecurityPemDatabase::GetPemFileBySerialNumber(
   const std::vector<uint8> & orc_SerialNumber) const
{
   const C_OSCSecurityPemKeyInfo * pc_Retval = NULL;

   for (uint32 u32_ItFile = 0UL; u32_ItFile < this->mc_StoredPemFiles.size(); ++u32_ItFile)
   {
      const C_OSCSecurityPemKeyInfo & rc_KeyFile = this->mc_StoredPemFiles[u32_ItFile];
      const std::vector<uint8> & rc_CurSerialNumber = rc_KeyFile.GetPubKeySerialNumber();
      if (rc_CurSerialNumber.size() == orc_SerialNumber.size())
      {
         bool q_Matches = true;
         for (uint32 u32_ItKey = 0UL; (u32_ItKey < rc_CurSerialNumber.size()) && (q_Matches == true); ++u32_ItKey)
         {
            if (rc_CurSerialNumber[u32_ItKey] != orc_SerialNumber[u32_ItKey])
            {
               q_Matches = false;
            }
         }
         if (q_Matches)
         {
            pc_Retval = &this->mc_StoredPemFiles[u32_ItFile];
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse folder

   \param[in]  orc_FolderPath    Folder path

   \return
   STW error codes

   \retval   C_NO_ERR   Information extracted
   \retval   C_RANGE    Folder not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSecurityPemDatabase::ParseFolder(const std::string & orc_FolderPath)
{
   sint32 s32_Retval = C_NO_ERR;

   const stw_scl::C_SCLString c_SclFolderPathWithDelimiter = stw_tgl::TGL_FileIncludeTrailingDelimiter(orc_FolderPath);
   const std::string c_FolderPathWithDelimiter = c_SclFolderPathWithDelimiter.c_str();

   // Remove previous results
   this->mc_StoredPemFiles.clear();

   if (TGL_DirectoryExists(c_FolderPathWithDelimiter))
   {
      const std::vector<std::string> c_Files = C_OSCSecurityPemDatabase::mh_GetPEMFiles(c_FolderPathWithDelimiter);
      for (uint32 u32_It = 0UL; u32_It < c_Files.size(); ++u32_It)
      {
         const std::string c_CurFolderPath = c_Files[u32_It];
         C_OSCSecurityPem c_NewFile;
         std::string c_ErrorMessage;
         s32_Retval = c_NewFile.LoadFromFile(c_CurFolderPath, c_ErrorMessage);
         if (s32_Retval == C_NO_ERR)
         {
            m_TryAddKey(c_NewFile.GetKeyInfo(), c_ErrorMessage);
         }
         if (c_ErrorMessage.size() > 0UL)
         {
            osc_write_log_warning("Read PEM database",
                                  "Error reading file \"" + c_CurFolderPath + "\": " + c_ErrorMessage);
         }
      }
      osc_write_log_info("Read PEM database",
                         "Imported " + stw_scl::C_SCLString::IntToStr(
                            this->mc_StoredPemFiles.size()) + " valid PEM files of the total seen " +
                         stw_scl::C_SCLString::IntToStr(
                            c_Files.size()) + " PEM files in folder \"" + c_FolderPathWithDelimiter + "\".");
      //Reset error status
      s32_Retval = C_NO_ERR;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Try add key

   \param[in]      orc_NewKey          New key
   \param[in,out]  orc_ErrorMessage    Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSecurityPemDatabase::m_TryAddKey(const C_OSCSecurityPemKeyInfo & orc_NewKey, std::string & orc_ErrorMessage)
{
   if (orc_NewKey.GetPubKeyTextDecoded().size() > 0UL)
   {
      if (orc_NewKey.GetPubKeySerialNumber().size() > 0UL)
      {
         if (orc_NewKey.GetPrivKeyTextDecoded().size() > 0UL)
         {
            if (this->GetPemFileBySerialNumber(orc_NewKey.GetPubKeySerialNumber()) == NULL)
            {
               this->mc_StoredPemFiles.push_back(orc_NewKey);
            }
            else
            {
               orc_ErrorMessage = "ignored because serial number already exists";
            }
         }
         else
         {
            orc_ErrorMessage = "could not find private key";
         }
      }
      else
      {
         orc_ErrorMessage = "could not find serial number in public key";
      }
   }
   else
   {
      orc_ErrorMessage = "could not find public key";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get PEM files

   \param[in]  orc_FolderPath    Folder path with path delimiter at the end

   \return
   PEM files
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::string> C_OSCSecurityPemDatabase::mh_GetPEMFiles(const std::string & orc_FolderPath)
{
   std::vector<std::string> c_Retval;
   stw_scl::SCLDynamicArray<TGL_FileSearchRecord> c_FilesSCL;

   TGL_FileFind(orc_FolderPath + "*", c_FilesSCL);
   for (sint32 s32_It = 0; s32_It < c_FilesSCL.GetLength(); ++s32_It)
   {
      const TGL_FileSearchRecord & rc_FileRecord = c_FilesSCL[s32_It];
      const stw_scl::C_SCLString c_FileNameSCL = rc_FileRecord.c_FileName;
      const stw_scl::C_SCLString c_Extension = TGL_ExtractFileExtension(c_FileNameSCL);
      if (c_Extension == ".pem")
      {
         c_Retval.push_back(orc_FolderPath + c_FileNameSCL.c_str());
      }
   }
   return c_Retval;
}

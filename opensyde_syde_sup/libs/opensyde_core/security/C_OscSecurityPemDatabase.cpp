//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files

   Class to handle parsing of PEM file folder and stores all necessary information about all read PEM files

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwerrors.hpp"
#include "C_SclStringList.hpp"
#include "C_OscSecurityPem.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSecurityPemDatabase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

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
C_OscSecurityPemDatabase::C_OscSecurityPemDatabase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get size of database

   \return
   Size of database
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscSecurityPemDatabase::GetSizeOfDatabase() const
{
   return static_cast<uint32_t>(this->mc_StoredPemFiles.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pem file by serial number

   \param[in]  orc_SerialNumber  Serial number

   \return
   PEM file by serial number
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscSecurityPemKeyInfo * C_OscSecurityPemDatabase::GetPemFileBySerialNumber(
   const std::vector<uint8_t> & orc_SerialNumber) const
{
   const C_OscSecurityPemKeyInfo * pc_Retval = NULL;

   for (uint32_t u32_ItFile = 0UL; u32_ItFile < this->mc_StoredPemFiles.size(); ++u32_ItFile)
   {
      const C_OscSecurityPemKeyInfo & rc_KeyFile = this->mc_StoredPemFiles[u32_ItFile];
      const std::vector<uint8_t> & rc_CurSerialNumber = rc_KeyFile.GetPubKeySerialNumber();
      if (rc_CurSerialNumber.size() == orc_SerialNumber.size())
      {
         bool q_Matches = true;
         for (uint32_t u32_ItKey = 0UL; (u32_ItKey < rc_CurSerialNumber.size()) && (q_Matches == true); ++u32_ItKey)
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
int32_t C_OscSecurityPemDatabase::ParseFolder(const std::string & orc_FolderPath)
{
   int32_t s32_Retval = C_NO_ERR;

   const stw::scl::C_SclString c_SclFolderPathWithDelimiter =
      stw::tgl::TglFileIncludeTrailingDelimiter(orc_FolderPath);
   const std::string c_FolderPathWithDelimiter = c_SclFolderPathWithDelimiter.c_str();

   // Remove previous results
   this->mc_StoredPemFiles.clear();

   if (TglDirectoryExists(c_FolderPathWithDelimiter))
   {
      const std::vector<std::string> c_Files = C_OscSecurityPemDatabase::mh_GetPemFiles(c_FolderPathWithDelimiter);
      for (uint32_t u32_It = 0UL; u32_It < c_Files.size(); ++u32_It)
      {
         const std::string c_CurFolderPath = c_Files[u32_It];
         C_OscSecurityPem c_NewFile;
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
                         "Imported " + stw::scl::C_SclString::IntToStr(
                            this->mc_StoredPemFiles.size()) + " valid PEM files of the total seen " +
                         stw::scl::C_SclString::IntToStr(
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
void C_OscSecurityPemDatabase::m_TryAddKey(const C_OscSecurityPemKeyInfo & orc_NewKey, std::string & orc_ErrorMessage)
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
std::vector<std::string> C_OscSecurityPemDatabase::mh_GetPemFiles(const std::string & orc_FolderPath)
{
   std::vector<std::string> c_Retval;
   stw::scl::C_SclDynamicArray<C_TglFileSearchRecord> c_FilesScl;

   TglFileFind(orc_FolderPath + "*", c_FilesScl);
   for (int32_t s32_It = 0; s32_It < c_FilesScl.GetLength(); ++s32_It)
   {
      const C_TglFileSearchRecord & rc_FileRecord = c_FilesScl[s32_It];
      const stw::scl::C_SclString c_FileNameScl = rc_FileRecord.c_FileName;
      const stw::scl::C_SclString c_Extension = TglExtractFileExtension(c_FileNameScl);
      if (c_Extension == ".pem")
      {
         c_Retval.push_back(orc_FolderPath + c_FileNameScl.c_str());
      }
   }
   return c_Retval;
}

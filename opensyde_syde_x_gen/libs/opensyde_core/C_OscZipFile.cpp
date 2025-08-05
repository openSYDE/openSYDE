//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip file utilities

   see header in .h file for details.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <fstream>

#include "TglFile.hpp"
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES //prevent namespace pollution
#include "miniz.h"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscZipFile.hpp"
#include "C_OscUtils.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;
using namespace std;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create ZIP archive from files

   In the unlikely case that function fails there is no clean-up done and
   "it's possible the archive could be left without a central directory"
   (see miniz.c).

   The function will not
   * create the target folder
   * erase a pre-existing zip file

   If the zip file already exists behavior is undefined
   (technically the function will probably add to the existing file but this has not been tested).

   Assumptions:
   * consistent data
   * moderate file sizes to zip
    (is not zipping files by chunk, with files greater than 0.5 GB there can occur problems)

   \param[in]  orc_SourcePath                path of folder containing files to add to archive
                                              (needs trailing path terminator)
   \param[in]  orc_SupFiles                  relative file paths for zip archive (files to add to archive)
   \param[in]  orc_ZipArchivePath            zip archive name with full path
   \param[out] opc_ErrorText                 if != NULL and return value != C_NO_ERR: will be filled with textual
                                              details of problem

   \return
   C_NO_ERR    success
   C_CONFIG    at least one input file does not exist
   C_RD_WR     could not open input file
   C_NOACT     could not add data to zip file (does the path to the file exist ?)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipFile::h_CreateZipFile(const C_SclString & orc_SourcePath, const std::set<C_SclString> & orc_SupFiles,
                                      const C_SclString & orc_ZipArchivePath,
                                      stw::scl::C_SclString * const opc_ErrorText)
{
   int32_t s32_Return = C_NO_ERR;

   std::set<C_SclString>::const_iterator c_Iter;

   // check whether input file(s) exist:
   // go through all files and store in zip archive
   for (c_Iter = orc_SupFiles.begin(); c_Iter != orc_SupFiles.end(); ++c_Iter)
   {
      const C_SclString c_AbsPath = orc_SourcePath + (*c_Iter);
      if ((TglFileExists(c_AbsPath) == false) && (TglDirectoryExists(c_AbsPath) == false))
      {
         if (opc_ErrorText != NULL)
         {
            (*opc_ErrorText) = "Input file/folder \"" + c_AbsPath + "\" does not exist.";
         }
         s32_Return = C_CONFIG;
         break;
      }
   }

   // go through all files and store in zip archive
   for (c_Iter = orc_SupFiles.begin(); (c_Iter != orc_SupFiles.end()) && (s32_Return == C_NO_ERR); ++c_Iter)
   {
      const C_SclString c_FileName = *c_Iter;
      if (TglFileExists(orc_SourcePath + c_FileName))
      {
         const C_SclString c_AbsPath = orc_SourcePath + c_FileName;                  // absolute path
         ifstream c_FileStream(c_AbsPath.c_str(), ifstream::binary | ifstream::ate); // open file and set pos
                                                                                     // to the end of file
         if (c_FileStream.is_open() == true)
         {
            // get file length, position is at the end of the input sequence
            const streampos c_FileLength = c_FileStream.tellg();
            // set position to the beginning of the input sequence
            c_FileStream.seekg(0LL, ifstream::beg);

            // allocate memory
            char_t * const pcn_FileData = new char_t[c_FileLength];

            // read file content
            c_FileStream.read(pcn_FileData, static_cast<streamsize>(c_FileLength));

            // close file
            c_FileStream.close();

            s32_Return = C_OscZipFile::mh_AddContentToZipFile(orc_ZipArchivePath, c_FileName,
                                                              pcn_FileData, static_cast<uint32_t>(c_FileLength),
                                                              "file", opc_ErrorText);

            // free memory
            delete[] pcn_FileData;
         }
         else
         {
            if (opc_ErrorText != NULL)
            {
               (*opc_ErrorText) = "Could not open file \"" + c_FileName + "\" for zipping into archive.";
            }
            s32_Return = C_RD_WR;
         }
      }
      else
      {
         s32_Return = C_OscZipFile::mh_AddContentToZipFile(orc_ZipArchivePath, c_FileName, NULL, 0UL,
                                                           "folder", opc_ErrorText);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unpacks source zip archive and stores the contents to target unzip path.

   Assumptions:
   * valid paths
   * read/write permissions

   If the target path does not exist the function will try to create it.

   \param[in]  orc_SourcePath                path of zip archive
   \param[in]  orc_TargetUnzipPath           target path for unpacking the zip archive
                                              (with or without trailing path delimiter)
   \param[out] opc_ErrorText                 if != NULL and return value != C_NO_ERR: will be filled with textual
                                              details of problem

   \return
   C_NO_ERR    success
   C_RD_WR     could not unpack archive to target path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipFile::h_UnpackZipFile(const C_SclString & orc_SourcePath, const C_SclString & orc_TargetUnzipPath,
                                      stw::scl::C_SclString * const opc_ErrorText)
{
   int32_t s32_Return = C_NO_ERR;

   mz_zip_archive c_ZipArchive;
   mz_bool x_MzStatus; //lint !e8080  //using type to match library interface

   vector<mz_zip_archive_file_stat> c_Files; // to store content of zip archive

   // open archive
   (void)memset(&c_ZipArchive, 0, sizeof(c_ZipArchive));
   x_MzStatus = mz_zip_reader_init_file(&c_ZipArchive, orc_SourcePath.c_str(), 0);
   if (x_MzStatus == MZ_FALSE)
   {
      if (opc_ErrorText != NULL)
      {
         (*opc_ErrorText) = "Could not open zip archive \"" + orc_SourcePath + "\". Reason: \"" +
                            mz_zip_get_error_string(c_ZipArchive.m_last_error) + "\".";
      }
      s32_Return = C_RD_WR;
   }

   if (s32_Return == C_NO_ERR)
   {
      // get content of zip archive
      //lint -e{8080}  //using type to match library interface
      for (mz_uint x_Pos = 0; (x_Pos < mz_zip_reader_get_num_files(&c_ZipArchive)) && (x_MzStatus == MZ_TRUE);
           x_Pos++)
      {
         mz_zip_archive_file_stat c_FileStat;
         x_MzStatus = mz_zip_reader_file_stat(&c_ZipArchive, x_Pos, &c_FileStat);
         if (x_MzStatus == MZ_FALSE)
         {
            mz_zip_reader_end(&c_ZipArchive);
            if (opc_ErrorText != NULL)
            {
               (*opc_ErrorText) = "Could not get information of zip archive \"" + orc_SourcePath +
                                  "\" for position \"" + C_SclString::IntToStr(x_Pos) + "\".";
            }
            s32_Return = C_RD_WR;
         }
         else
         {
            // remember file for unpacking
            c_Files.push_back(c_FileStat);
         }
      }

      vector<mz_zip_archive_file_stat>::const_iterator c_Iter;
      for (c_Iter = c_Files.begin(); (c_Iter != c_Files.end()) && (s32_Return == C_NO_ERR); ++c_Iter)
      {
         //lint -e{8080} //using type expected by the library for compatibility
         size_t x_UncompFileSize;
         void * const pv_Data = mz_zip_reader_extract_file_to_heap(&c_ZipArchive, &c_Iter->m_filename[0],
                                                                   &x_UncompFileSize, 0);
         if (pv_Data != NULL)
         {
            // get complete file path of current file
            const C_SclString c_CompleteFilePath = TglFileIncludeTrailingDelimiter(orc_TargetUnzipPath) +
                                                   c_Iter->m_filename;

            // check if we have to create a subfolder
            const C_SclString c_Path = TglExtractFilePath(c_CompleteFilePath);
            if (TglDirectoryExists(c_Path) == false)
            {
               // create subfolder
               s32_Return = C_OscUtils::h_CreateFolderRecursively(c_Path);
               if ((s32_Return != C_NO_ERR) && (opc_ErrorText != NULL))
               {
                  (*opc_ErrorText) = "Could not create subfolder \"" + c_Path + "\".";
               }
            }
            // is it a file or just the folder itself ?
            if (c_Iter->m_is_directory == MZ_FALSE)
            {
               //it's a file -> more to do
               // create new empty file
               if (s32_Return == C_NO_ERR)
               {
                  std::FILE * const pc_File = std::fopen(c_CompleteFilePath.c_str(), "wb");
                  // write data
                  if (pc_File != NULL)
                  {
                     //lint -e{8080} //using type expected by the library for compatibility
                     const size_t x_SIZE_OF_ELEMENT = sizeof(char_t);
                     //lint -e{8080} //using type expected by the library for compatibility
                     const size_t x_NumOfBytesWritten = std::fwrite(pv_Data, x_SIZE_OF_ELEMENT, x_UncompFileSize,
                                                                    pc_File);
                     if (x_NumOfBytesWritten != x_UncompFileSize)
                     {
                        // data not written completely
                        if (opc_ErrorText != NULL)
                        {
                           (*opc_ErrorText) = "Could not unpack file \"" +
                                              c_CompleteFilePath + "\" of zip archive \"" + orc_SourcePath + "\".";
                        }
                        s32_Return = C_RD_WR;
                     }
                     std::fclose(pc_File);
                  }
                  else
                  {
                     if (opc_ErrorText != NULL)
                     {
                        (*opc_ErrorText) = "Could not create file \"" +
                                           c_CompleteFilePath + "\" to unpack from zip archive \"" + orc_SourcePath +
                                           "\".";
                     }
                     s32_Return = C_RD_WR;
                  }
               }
            }
         }
         else
         {
            if (opc_ErrorText != NULL)
            {
               (*opc_ErrorText) = static_cast<C_SclString>("Could not read file \"") +
                                  c_Iter->m_filename + "\" from zip archive \"" + orc_SourcePath + "\".";
            }
            s32_Return = C_RD_WR;
         }
         // we're done, get next file content in case of no error
         mz_free(pv_Data);
      }
   }
   // Close the archive, freeing any resources it was using
   mz_zip_reader_end(&c_ZipArchive);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append files to set as relative paths

   Result can be used for parameter orc_SupFiles of h_CreateZipFile

   \param[in,out] orc_Set      Set to appen paths to
   \param[in]     orc_Files    Vector of files to append
   \param[in]     orc_BasePath Path the files will be raltive to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscZipFile::h_AppendFilesRelative(std::set<C_SclString> & orc_Set, const std::vector<C_SclString> & orc_Files,
                                         const C_SclString & orc_BasePath)
{
   for (uint32_t u32_PosFilesToFlash = 0;
        u32_PosFilesToFlash < orc_Files.size();
        u32_PosFilesToFlash++)
   {
      C_SclString c_RelativeFilePath = orc_Files[u32_PosFilesToFlash];
      c_RelativeFilePath = c_RelativeFilePath.SubString(
         orc_BasePath.Length() + 1,
         c_RelativeFilePath.Length() - orc_BasePath.Length());
      orc_Set.insert(c_RelativeFilePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the file for a zip file

   \param[in]  orc_FilePath                path of file to check

   \return
   C_NO_ERR    File is a zip file
   C_RANGE     File is not a zip file
   C_CONFIG    The input file does not exist
   C_RD_WR     could not open input file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipFile::h_IsZipFile(const C_SclString & orc_FilePath)
{
   int32_t s32_Return;

   if (TglFileExists(orc_FilePath) == false)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      ifstream c_FileStream(orc_FilePath.c_str(), ifstream::binary | ifstream::ate); // open file and set pos
      if (c_FileStream.is_open() == true)
      {
         const int32_t s32_FileLength = static_cast<int32_t>(c_FileStream.tellg());

         s32_Return = C_RANGE;

         if (s32_FileLength > 4)
         {
            //using type to match library interface; if char_t had a size different than a "uint8"
            //this spot would be the smallest of our problems
            const char_t acn_ZIP_HEADER[4] = {0x50, 0x4B, 0x03, 0x04}; //lint !e970 !e9128
            char_t acn_Header[4];
            c_FileStream.seekg(0LL, std::ios::beg);
            c_FileStream.read(&acn_Header[0], 4);

            if (memcmp(&acn_ZIP_HEADER[0], &acn_Header[0], 4) == 0)
            {
               s32_Return = C_NO_ERR;
            }
         }

         c_FileStream.close();
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add content to zip file

   \param[in]      orc_ZipArchivePath  Zip archive path
   \param[in]      orc_ItemName        Item name
   \param[in]      opcn_Content        Content
   \param[in]      ou32_ContentSize    Content size
   \param[in]      orc_ItemType        Item type
   \param[in,out]  opc_ErrorText       Error text

   \return
   C_NO_ERR    success
   C_NOACT     could not add data to zip file (does the path to the file exist ?)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscZipFile::mh_AddContentToZipFile(const C_SclString & orc_ZipArchivePath, const C_SclString & orc_ItemName,
                                             const char_t * const opcn_Content, const uint32_t ou32_ContentSize,
                                             const C_SclString & orc_ItemType, C_SclString * const opc_ErrorText)
{
   int32_t s32_Return = C_NO_ERR;
   C_SclString c_FilePathWithSlashes = orc_ItemName;
   const C_SclString c_Comment = "Zipping " + orc_ItemType + ": " + orc_ItemName; // set filename as comment
   mz_bool x_MzStatus;                                                            //lint !e8080  //using type to match

   // library interface

   // miniz cannot handle windows '\\' directories
   // therefore change to '/'
   c_FilePathWithSlashes.ReplaceAll("\\", "/");

   // store file content to zip archive
   // mz_zip_add_mem_to_archive_file_in_place is creating, appending and always finalizing the archive
   // after successful operation
   x_MzStatus = mz_zip_add_mem_to_archive_file_in_place(
      orc_ZipArchivePath.c_str(), c_FilePathWithSlashes.c_str(),
      opcn_Content, static_cast<size_t>(ou32_ContentSize),
      c_Comment.c_str(), static_cast<uint16_t>(c_Comment.Length()),
      MZ_BEST_COMPRESSION);

   if (x_MzStatus == MZ_FALSE)
   {
      if (opc_ErrorText != NULL)
      {
         (*opc_ErrorText) = "Could not create zip " + orc_ItemType + " \"" + orc_ItemName + "\".";
      }
      s32_Return = C_NOACT;
   }
   return s32_Return;
}

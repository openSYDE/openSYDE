//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core zip/unzip file utilities

   see header in .h file for details.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <fstream>

#include "TGLFile.h"
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES //prevent namespace pollution
#include "miniz.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCZipFile.h"
#include "C_OSCUtils.h"
#include "CSCLString.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;
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

   If the zip file already exists with behavior is undefined
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
sint32 C_OSCZipFile::h_CreateZipFile(const C_SCLString & orc_SourcePath, const std::set<C_SCLString> & orc_SupFiles,
                                     const C_SCLString & orc_ZipArchivePath, stw_scl::C_SCLString * const opc_ErrorText)
{
   sint32 s32_Return = C_NO_ERR;

   std::set<C_SCLString>::const_iterator c_Iter;

   // check whether input file(s) exist:
   // go through all files and store in zip archive
   for (c_Iter = orc_SupFiles.begin(); c_Iter != orc_SupFiles.end(); ++c_Iter)
   {
      const C_SCLString c_AbsPath = orc_SourcePath + (*c_Iter);
      if (TGL_FileExists(c_AbsPath) == false)
      {
         if (opc_ErrorText != NULL)
         {
            (*opc_ErrorText) = "Input file \"" + c_AbsPath + "\" does not exist.";
         }
         s32_Return = C_CONFIG;
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      sintn sn_MzStatus = MZ_TRUE;
      // go through all files and store in zip archive
      for (c_Iter = orc_SupFiles.begin(); (c_Iter != orc_SupFiles.end()) && (sn_MzStatus == MZ_TRUE); ++c_Iter)
      {
         C_SCLString c_FileName = *c_Iter;
         const C_SCLString c_AbsPath = orc_SourcePath + c_FileName;                  // absolute path
         ifstream c_FileStream(c_AbsPath.c_str(), ifstream::binary | ifstream::ate); // open file and set pos
                                                                                     // to the end of file
         if (c_FileStream.is_open() == true)
         {
            C_SCLString c_FilePathWithSlashes;
            // get file length, position is at the end of the input sequence
            const sintn sn_FileLength = static_cast<sintn>(c_FileStream.tellg());
            // set position to the beginning of the input sequence
            c_FileStream.seekg(0LL, ifstream::beg);

            // allocate memory
            charn * const pcn_FileData = new charn[static_cast<uintn>(sn_FileLength)];
            const C_SCLString c_Comment = "Zipping file: " + c_FileName; // set filename as comment

            // read file content
            c_FileStream.read(pcn_FileData, sn_FileLength);

            // close file
            c_FileStream.close();

            // miniz cannot handle windows '\\' directories
            // therefore change to '/'
            for (uint32 u32_Pos = 1U; u32_Pos <= c_FileName.Length(); u32_Pos++)
            {
               if (c_FileName[u32_Pos] == '\\')
               {
                  c_FilePathWithSlashes += '/';
               }
               else
               {
                  c_FilePathWithSlashes += c_FileName[u32_Pos];
               }
            }

            // store file content to zip archive
            // mz_zip_add_mem_to_archive_file_in_place is creating, appending and always finalizing the archive
            // after successful operation
            sn_MzStatus = mz_zip_add_mem_to_archive_file_in_place(
               orc_ZipArchivePath.c_str(), c_FilePathWithSlashes.c_str(),
               pcn_FileData, sn_FileLength,
               c_Comment.c_str(), static_cast<uint16>(c_Comment.Length()),
               MZ_BEST_COMPRESSION);
            if (sn_MzStatus == MZ_FALSE)
            {
               if (opc_ErrorText != NULL)
               {
                  (*opc_ErrorText) = "Could not create zip file \"" + c_FileName + "\".";
               }
               s32_Return = C_NOACT;
            }

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
sint32 C_OSCZipFile::h_UnpackZipFile(const C_SCLString & orc_SourcePath, const C_SCLString & orc_TargetUnzipPath,
                                     stw_scl::C_SCLString * const opc_ErrorText)
{
   sint32 s32_Return = C_NO_ERR;

   mz_zip_archive c_ZipArchive;
   sintn sn_MzStatus;

   vector<mz_zip_archive_file_stat> c_Files; // to store content of zip archive

   // open archive
   memset(&c_ZipArchive, 0, sizeof(c_ZipArchive));
   sn_MzStatus = mz_zip_reader_init_file(&c_ZipArchive, orc_SourcePath.c_str(), 0);
   if (sn_MzStatus == MZ_FALSE)
   {
      if (opc_ErrorText != NULL)
      {
         (*opc_ErrorText) = "Could not open zip archive \"" + orc_SourcePath + "\".";
      }
      s32_Return = C_RD_WR;
   }

   if (s32_Return == C_NO_ERR)
   {
      // get content of zip archive
      for (uintn un_Pos = 0; (un_Pos < mz_zip_reader_get_num_files(&c_ZipArchive)) && (sn_MzStatus == MZ_TRUE);
           un_Pos++)
      {
         mz_zip_archive_file_stat c_FileStat;
         sn_MzStatus = mz_zip_reader_file_stat(&c_ZipArchive, un_Pos, &c_FileStat);
         if (sn_MzStatus == MZ_FALSE)
         {
            mz_zip_reader_end(&c_ZipArchive);
            if (opc_ErrorText != NULL)
            {
               (*opc_ErrorText) = "Could not get information of zip archive \"" + orc_SourcePath +
                                  "\" for position \"" + C_SCLString(un_Pos) + "\".";
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
         size_t un_UncompFileSize;
         void * pv_Data = mz_zip_reader_extract_file_to_heap(&c_ZipArchive, c_Iter->m_filename, &un_UncompFileSize, 0);
         if (pv_Data != NULL)
         {
            // get complete file path of current file
            const C_SCLString c_CompleteFilePath = TGL_FileIncludeTrailingDelimiter(orc_TargetUnzipPath) +
                                                   c_Iter->m_filename;

            // check if we have to create a subfolder
            const C_SCLString c_Path = TGL_ExtractFilePath(c_CompleteFilePath);
            if (TGL_DirectoryExists(c_Path) == false)
            {
               // create subfolder
               s32_Return = C_OSCUtils::h_CreateFolderRecursively(c_Path);
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
                  FILE * pc_File = fopen(c_CompleteFilePath.c_str(), "wb");
                  // write data
                  if (pc_File != NULL)
                  {
                     size_t un_SizeOfElement = sizeof(charn);
                     size_t un_NumOfBytesWritten = fwrite(pv_Data, un_SizeOfElement, un_UncompFileSize, pc_File);
                     if (un_NumOfBytesWritten != un_UncompFileSize)
                     {
                        // data not written completely
                        if (opc_ErrorText != NULL)
                        {
                           (*opc_ErrorText) = "Could not unpack file \"" +
                                              c_CompleteFilePath + "\" of zip archive \"" + orc_SourcePath + "\".";
                        }
                        s32_Return = C_RD_WR;
                     }
                     fclose(pc_File);
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
               (*opc_ErrorText) = static_cast<C_SCLString>("Could not read file \"") +
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

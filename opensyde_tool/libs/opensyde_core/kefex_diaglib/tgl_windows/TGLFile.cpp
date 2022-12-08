//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: File functions

   cf. header for details

   Here: Implementation for MS-Windows.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdio>
#include <climits>
#include <windows.h>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglFile.hpp"
#include "C_SclString.hpp"
#include "C_SclDateTime.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
static bool m_FileAgeDosTime(const C_SclString & orc_FileName, uint16_t * const opu16_Date,
                             uint16_t * const opu16_Time);
static int32_t m_FileFind(const C_SclString & orc_SearchPattern,
                          C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles,
                          const bool oq_IncludeDirectories = false,
                          C_SclDynamicArray<uint8_t> * const opc_IsDirectory = NULL);
static bool m_CheckUncShare(const C_SclString & orc_Path);

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//utility: get operating system file age
static bool m_FileAgeDosTime(const C_SclString & orc_FileName, uint16_t * const opu16_Date, uint16_t * const opu16_Time)
{
   HANDLE pv_Handle;
   WIN32_FIND_DATAA t_FindData;
   FILETIME t_LocalFileTime;
   bool q_Return = false;

   pv_Handle = FindFirstFileA(orc_FileName.c_str(), &t_FindData);
   if (pv_Handle != INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      (void)FindClose(pv_Handle);
      if ((t_FindData.dwFileAttributes & static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) == 0U)
      {
         (void)FileTimeToLocalFileTime(&t_FindData.ftLastWriteTime, &t_LocalFileTime);
         //please Visual C 2005 compiler ...
         q_Return = (FileTimeToDosDateTime(&t_LocalFileTime, opu16_Date, opu16_Time) == 0) ? false : true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   check if string is UNC server path

   For paths of kind "\\mypc" the windows API for file attributes returns invalid, because it is only able
   to handle sub-directories like "\\mypc\share". For this special case we check the last error of a
   probably not existing sub-directory and use it for guessing on existence of the parent path.

   \param[in]     orc_Path     path that might be an UNC server path

   \return
   true      path is an existing UNC share  \n
   false     path is not an existing UNC share
*/
//----------------------------------------------------------------------------------------------------------------------
static bool m_CheckUncShare(const C_SclString & orc_Path)
{
   bool q_Return = false;
   uint32_t u32_Attrib = GetFileAttributesA(orc_Path.c_str());

   if (u32_Attrib == INVALID_FILE_ATTRIBUTES)
   {
      // check beginning of path for "//" or "\\"
      if ((orc_Path.SubString(1, 2) == "//") || (orc_Path.SubString(1, 2) == "\\\\"))
      {
         const size_t un_CharIndex = orc_Path.AsStdString()->find_first_of("\\/", 2);
         // check if no deeper UNC path like "\\mypc\shared"
         if ((un_CharIndex == orc_Path.Length()) || // case path ends on slash
             (un_CharIndex == std::string::npos))   // case path does not contain another slash
         {
            const C_SclString c_SubPath = orc_Path + "\\randomsubdir";
            uint32_t u32_LastErr;

            // check for a random sub-directory if it exists
            u32_Attrib = GetFileAttributesA(c_SubPath.c_str());
            u32_LastErr = GetLastError();

            // check if last error indicates that the server exists (for non-existing servers last error is different)
            if ((u32_Attrib != INVALID_FILE_ATTRIBUTES) /*for the unlikely case this random sub-directory exists*/ ||
                (u32_LastErr == ERROR_BAD_NET_NAME))
            {
               q_Return = true;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get file age time as string

   Report the specified file's timestamp as a string.
   Format of returned string: "dd.mm.yyyy hh:mm:ss"

   \param[in]     orc_FileName     path to file
   \param[out]    orc_String       timestamp as string

   \return
   true      timestamp placed in oc_String  \n
   false     error -> oc_String not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglFileAgeString(const C_SclString & orc_FileName, C_SclString & orc_String)
{
   bool q_Return;
   uint16_t u16_Time;
   uint16_t u16_Date;
   C_SclDateTime c_DateTime;

   q_Return = m_FileAgeDosTime(orc_FileName, &u16_Date, &u16_Time);
   if (q_Return == true)
   {
      c_DateTime.mu16_Day    = static_cast<uint16_t>(u16_Date & 0x1FU);
      c_DateTime.mu16_Month  = static_cast<uint16_t>((u16_Date >> 5U) & 0x0FU);
      c_DateTime.mu16_Year   = static_cast<uint16_t>((u16_Date >> 9U) + 1980U);
      c_DateTime.mu16_Hour   = static_cast<uint16_t>(u16_Time >> 11U);
      c_DateTime.mu16_Minute = static_cast<uint16_t>((u16_Time >> 5U) & 0x3FU);
      c_DateTime.mu16_Second = static_cast<uint16_t>((u16_Time & 0x1FU) * 2U);
   }
   else
   {
      c_DateTime.mu16_Day    = 1U;
      c_DateTime.mu16_Month  = 1U;
      c_DateTime.mu16_Year   = 1970U;
      c_DateTime.mu16_Hour   = 0U;
      c_DateTime.mu16_Minute = 0U;
      c_DateTime.mu16_Second = 0U;
   }
   orc_String = c_DateTime.DateTimeToString();
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get file size in bytes

   Return the specified file's file size in bytes.

   \param[in]     orc_FileName     path to file

   \return
   -1        error \n                  \n
   else      size of file in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t stw::tgl::TglFileSize(const C_SclString & orc_FileName)
{
   std::FILE * pc_File;
   int32_t s32_Size = -1;

   pc_File = std::fopen(orc_FileName.c_str(), "rb");
   if (pc_File != NULL)
   {
      (void)std::fseek(pc_File, 0, SEEK_END);
      s32_Size = std::ftell(pc_File);
      (void)std::fclose(pc_File);
   }
   return s32_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   check whether directory exists

   Detects whether the specified directory exists.
   For the Windows target this also includes simple drive names (e.g. "d:")

   \param[in]     orc_Path     path to directory (works with or without trailing path delimiter)

   \return
   true       directory exists  \n
   false      directory does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool TGL_PACKAGE stw::tgl::TglDirectoryExists(const C_SclString & orc_Path)
{
   bool q_Return = false;
   const uint32_t u32_Attrib = GetFileAttributesA(orc_Path.c_str());

   if ((u32_Attrib != INVALID_FILE_ATTRIBUTES) &&
       ((u32_Attrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
   {
      q_Return = true;
   }
   else
   {
      // UNC path fallback: Windows handles network shares without sub-directory (e.g. \\mypc) as invalid path,
      // but it would be nicer to return true here. So check this special case separately.
      q_Return = m_CheckUncShare(orc_Path);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   check whether file exists

   Detects whether the specified file exists

   \param[in]     orc_FileName     path to file

   \return
   true       file exists  \n
   false      file does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglFileExists(const C_SclString & orc_FileName)
{
   bool q_Return = false;
   HANDLE pv_Handle;
   WIN32_FIND_DATAA t_FindData;

   pv_Handle = FindFirstFileA(orc_FileName.c_str(), &t_FindData);
   if (pv_Handle != INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      (void)FindClose(pv_Handle);
      if ((t_FindData.dwFileAttributes & static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) == 0U)
      {
         q_Return = true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   find files

   Scans the file-system for files and directories matching the specified pattern.
   Files and directories with all attributes are detected.

   \param[in]     orc_SearchPattern search pattern (* and ? wildcards are possible)
   \param[out]    orc_FoundFiles    array with found files (without path, just name + extension)

   \return
   C_NO_ERR     at least one file found \n
   C_NOACT      no files found
*/
//----------------------------------------------------------------------------------------------------------------------
static int32_t m_FileFind(const C_SclString & orc_SearchPattern,
                          C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles, const bool oq_IncludeDirectories,
                          C_SclDynamicArray<uint8_t> * const opc_IsDirectory)
{
   WIN32_FIND_DATAA t_FindFileData;
   HANDLE pv_Find;
   uint8_t u8_IsDirectory;

   orc_FoundFiles.SetLength(0);

   // Find the first file in the directory.
   pv_Find = FindFirstFileA(orc_SearchPattern.c_str(), &t_FindFileData);
   if (pv_Find == INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      return C_CONFIG;
   }

   u8_IsDirectory = ((t_FindFileData.dwFileAttributes & static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) ==
                     static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) ? 1U : 0U;
   if ((u8_IsDirectory == 0U) || (oq_IncludeDirectories == true))
   {
      orc_FoundFiles.IncLength();
      orc_FoundFiles[orc_FoundFiles.GetHigh()].c_FileName = t_FindFileData.cFileName;
      if (opc_IsDirectory != NULL)
      {
         opc_IsDirectory->IncLength();
         (*opc_IsDirectory)[opc_IsDirectory->GetHigh()] = u8_IsDirectory;
      }
   }

   while (FindNextFileA(pv_Find, &t_FindFileData) != 0)
   {
      u8_IsDirectory = ((t_FindFileData.dwFileAttributes & static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) ==
                        static_cast<uint32_t>(FILE_ATTRIBUTE_DIRECTORY)) ? 1U : 0U;
      if ((u8_IsDirectory == 0U) || (oq_IncludeDirectories == true))
      {
         orc_FoundFiles.IncLength();
         orc_FoundFiles[orc_FoundFiles.GetHigh()].c_FileName = t_FindFileData.cFileName;
         if (opc_IsDirectory != NULL)
         {
            opc_IsDirectory->IncLength();
            (*opc_IsDirectory)[opc_IsDirectory->GetHigh()] = u8_IsDirectory;
         }
      }
   }
   (void)FindClose(pv_Find);
   return (orc_FoundFiles.GetLength() > 0) ? C_NO_ERR : C_NOACT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   find files

   Scans the file-system for files matching the specified pattern.
   Files with all attributes are detected.

   \param[in]     orc_SearchPattern search pattern (* and ? wildcards are possible)
   \param[out]    orc_FoundFiles    array with found files (without path, just name + extension)

   \return
   C_NO_ERR     at least one file found \n
   C_NOACT      no files found \n
   C_CONFIG     directory invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t TGL_PACKAGE stw::tgl::TglFileFind(const C_SclString & orc_SearchPattern,
                                           C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles)
{
   return m_FileFind(orc_SearchPattern, orc_FoundFiles);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append trailing path delimiter

   Append path delimiter to path if it does not already end in one.
   The path separator character can be target specific.

   Here:
   Windows supports backslash and slash (at least in most scenarios).

   \param[in]     orc_Path       path (with or without ending delimiter)

   \return
   path with delimiter
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglFileIncludeTrailingDelimiter(const C_SclString & orc_Path)
{
   if (orc_Path.Length() == 0)
   {
      return "\\";
   }
   if ((orc_Path.operator [](orc_Path.Length()) != '\\') &&
       (orc_Path.operator [](orc_Path.Length()) != '/'))
   {
      return orc_Path + "\\";
   }
   return orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract file extension

   Extract a file extension separated from the file name by a ".".
   If there is more than one "." in the file path the last one will be used.
   If the file does not have an extension an empty string is returned.

   \param[in]     orc_Path       full file path (or just name with extension)

   \return
   extension (includes the ".")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglExtractFileExtension(const C_SclString & orc_Path)
{
   C_SclString c_Extension;
   char_t acn_Ext[_MAX_EXT + 1];

   _splitpath(orc_Path.c_str(), NULL, NULL, NULL, &acn_Ext[0]);
   c_Extension = static_cast<C_SclString>(acn_Ext);
   return c_Extension;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change file extension

   Extract a file extension separated from the file name by a ".".
   If there is more than one "." in the file path the last one will be used.
   Then replace it with the new one.

   \param[in]     orc_Path       full file path (or just name with extension)
   \param[in]     orc_Extension  new extension (must be specified with the ".")

   \return
   new file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglChangeFileExtension(const C_SclString & orc_Path,
                                                          const C_SclString & orc_Extension)
{
   uint32_t u32_Pos;
   C_SclString c_NewPath = orc_Path;

   u32_Pos = c_NewPath.LastPos(".");
   if (u32_Pos != 0U)
   {
      //there is a file extension !
      c_NewPath = c_NewPath.Delete(u32_Pos, INT_MAX); //remove everything from and including the "."
      c_NewPath += orc_Extension;
   }
   return c_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Retrieves binary path of the current running process

   \return  full path including "/binary" (including extension if any); empty string on error
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglGetExePath(void)
{
   uint32_t u32_Return;
   char_t acn_Path[MAX_PATH];
   C_SclString c_Path;

   u32_Return = GetModuleFileNameA(NULL, acn_Path, MAX_PATH);
   if (u32_Return != 0)
   {
      //we got a path ...
      c_Path = acn_Path;
   }
   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract file path from full path

   Extract file path from a full file path.
   Can handle "/" and "\" path separators.

   \param[in]     orc_Path       full file path

   \return
   file path   (including final "\", "/" or ":")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglExtractFilePath(const C_SclString & orc_Path)
{
   C_SclString c_Path;
   char_t acn_Drive[_MAX_DRIVE + 1];
   char_t acn_Dir[_MAX_DIR + 1];

   _splitpath(orc_Path.c_str(), acn_Drive, acn_Dir, NULL, NULL);
   c_Path = static_cast<C_SclString>(acn_Drive) + acn_Dir;
   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Extract file name from full path

   Extract file name including extension from a full file path.
   Can handle "/" and "\" path separators.

   \param[in]     orc_Path       full file path

   \return
   file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglExtractFileName(const C_SclString & orc_Path)
{
   C_SclString c_FileName;
   char_t acn_Name[_MAX_FNAME + 1];
   char_t acn_Ext[_MAX_EXT + 1];

   _splitpath(orc_Path.c_str(), NULL, NULL, &acn_Name[0], &acn_Ext[0]);
   c_FileName = static_cast<C_SclString>(acn_Name) + acn_Ext;
   return c_FileName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert relative file path to absolute path

   Caution:
   Depending on the implementation this function might temporarily modify the current directory.

   \param[in]   orc_RelativePath    relative file path
   \param[in]   orc_BasePath        base path the relative path is relative to

   \return
   Absolute path; empty string on error
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString TGL_PACKAGE stw::tgl::TglExpandFileName(const C_SclString & orc_RelativePath,
                                                     const C_SclString & orc_BasePath)
{
   uint32_t u32_Return;
   char_t acn_OriginalDirectory[MAX_PATH];
   C_SclString c_FullPath = "";

   //get original CWD:
   u32_Return = GetCurrentDirectoryA(MAX_PATH, acn_OriginalDirectory);
   if (u32_Return != 0)
   {
      //temporarily change CWD:
      u32_Return = SetCurrentDirectoryA(orc_BasePath.c_str());
      if (u32_Return != 0)
      {
         char_t acn_AbsolutePath[MAX_PATH];
         u32_Return = GetFullPathNameA(orc_RelativePath.c_str(), MAX_PATH, acn_AbsolutePath, NULL);
         if (u32_Return != 0)
         {
            //we got a path ...
            c_FullPath = acn_AbsolutePath;
         }
         //set CWD back to original
         (void)SetCurrentDirectoryA(acn_OriginalDirectory);
      }
   }
   return c_FullPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create single directory

   Create a directory.
   Does not support creating directories recursively.

   \param[in]     orc_Directory    name of directory (absolute or relative)

   \return
   0     directory created (or: directory already exists)
   -1    could not create directory
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t TGL_PACKAGE stw::tgl::TglCreateDirectory(const C_SclString & orc_Directory)
{
   BOOL x_Return;
   int32_t s32_Result = 0;

   if (TglDirectoryExists(orc_Directory.c_str()) == false)
   {
      x_Return = CreateDirectoryA(orc_Directory.c_str(), NULL);
      if (x_Return == FALSE)
      {
         s32_Result = -1;
      }
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove directory with subdirectories

   Removes the directory orc_Directory with all subdirectories.
   (Based on an implementation on codeguru.com)

   \param[in]   orc_Directory             name of directory to remove (absolute or relative)
   \param[in]   oq_ContentOnly            true: only remove content of directory (but including subdirectories)
                                          false: also remove directory itself

   \return
   0     directory removed
   -1    could not remove directory
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t TGL_PACKAGE stw::tgl::TglRemoveDirectory(const C_SclString & orc_Directory, const bool oq_ContentOnly)
{
   C_SclString c_FilePath; // Filepath
   C_SclString c_Pattern;  // Pattern
   int32_t s32_Return;
   uint32_t u32_Index;

   C_SclDynamicArray<C_TglFileSearchRecord> c_Files;
   C_SclDynamicArray<uint8_t> c_IsDirectory;

   c_Pattern = orc_Directory + "\\*.*";

   s32_Return = m_FileFind(c_Pattern, c_Files, true, &c_IsDirectory);
   if (s32_Return != C_NO_ERR)
   {
      s32_Return = -1;
   }
   else
   {
      for (u32_Index = 0U; u32_Index < static_cast<uint32_t>(c_Files.GetLength()); u32_Index++)
      {
         //delete content of directory
         if ((c_Files[u32_Index].c_FileName != ".") && (c_Files[u32_Index].c_FileName != "..")) //ignore "." and ".."
         {
            c_FilePath = orc_Directory + "/" + c_Files[u32_Index].c_FileName;

            if (c_IsDirectory[u32_Index] == 1U)
            {
               // Delete subdirectory
               s32_Return = TglRemoveDirectory(c_FilePath, false);
            }
            else
            {
               //remove file:
               // Set file attributes
               if (SetFileAttributesA(c_FilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
               {
                  s32_Return = -1;
               }
               // Delete file
               else if (DeleteFileA(c_FilePath.c_str()) == FALSE)
               {
                  s32_Return = -1;
               }
               else
               {
                  //nothing more to do ...
               }
            }
            if (s32_Return != 0)
            {
               break;
            }
         }
      }

      if (oq_ContentOnly == false)
      {
         // Set directory attributes
         if (SetFileAttributesA(orc_Directory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
         {
            s32_Return = -1;
         }

         // Delete directory
         else if (RemoveDirectoryA(orc_Directory.c_str()) == FALSE)
         {
            s32_Return = -1;
         }
         else
         {
            //nothing more to do ...
         }
      }
   }

   return s32_Return;
}


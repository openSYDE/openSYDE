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
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdio>
#include <climits>
#include <windows.h>
#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLFile.h"
#include "CSCLString.h"
#include "CSCLDateTime.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */
static bool m_FileAgeDosTime(const C_SCLString & orc_FileName, uint16 * const opu16_Date, uint16 * const opu16_Time);
static sint32 m_FileFind(const C_SCLString & orc_SearchPattern, SCLDynamicArray<TGL_FileSearchRecord> & orc_FoundFiles,
                         const bool oq_IncludeDirectories = false,
                         SCLDynamicArray<uint8> * const opc_IsDirectory = NULL);
static bool m_CheckUncShare(const C_SCLString & orc_Path);

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//utility: get operating system file age
static bool m_FileAgeDosTime(const C_SCLString & orc_FileName, uint16 * const opu16_Date, uint16 * const opu16_Time)
{
   HANDLE pv_Handle;
   WIN32_FIND_DATAA t_FindData;
   FILETIME t_LocalFileTime;
   bool q_Return = false;

   pv_Handle = FindFirstFileA(orc_FileName.c_str(), &t_FindData);
   if (pv_Handle != INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      (void)FindClose(pv_Handle);
      if ((t_FindData.dwFileAttributes & static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) == 0U)
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
static bool m_CheckUncShare(const C_SCLString & orc_Path)
{
   bool q_Return = false;
   uint32 u32_Attrib = GetFileAttributesA(orc_Path.c_str());

   if (u32_Attrib == INVALID_FILE_ATTRIBUTES)
   {
      // check beginning of path for "//" or "\\"
      if ((orc_Path.SubString(1, 2) == "//") || (orc_Path.SubString(1, 2) == "\\\\"))
      {
         size_t un_CharIndex = orc_Path.AsStdString()->find_first_of("\\/", 2);
         // check if no deeper UNC path like "\\mypc\shared"
         if ((un_CharIndex == orc_Path.Length()) || // case path ends on slash
             (un_CharIndex == std::string::npos))   // case path does not contain another slash
         {
            const C_SCLString c_SubPath = orc_Path + "\\randomsubdir";
            uint32 u32_LastErr;

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
bool stw_tgl::TGL_FileAgeString(const C_SCLString & orc_FileName, C_SCLString & orc_String)
{
   bool q_Return;
   uint16 u16_Time;
   uint16 u16_Date;
   C_SCLDateTime c_DateTime;

   q_Return = m_FileAgeDosTime(orc_FileName, &u16_Date, &u16_Time);
   if (q_Return == true)
   {
      c_DateTime.mu16_Day    = static_cast<uint16>(u16_Date & 0x1FU);
      c_DateTime.mu16_Month  = static_cast<uint16>((u16_Date >> 5) & 0x0FU);
      c_DateTime.mu16_Year   = static_cast<uint16>((u16_Date >> 9) + 1980U);
      c_DateTime.mu16_Hour   = static_cast<uint16>(u16_Time >> 11);
      c_DateTime.mu16_Minute = static_cast<uint16>((u16_Time >> 5) & 0x3FU);
      c_DateTime.mu16_Second = static_cast<uint16>((u16_Time & 0x1FU) * 2U);
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
sint32 stw_tgl::TGL_FileSize(const C_SCLString & orc_FileName)
{
   std::FILE * pt_File;
   sint32 s32_Size = -1;

   pt_File = std::fopen(orc_FileName.c_str(), "rb");
   if (pt_File != NULL)
   {
      (void)std::fseek(pt_File, 0, SEEK_END);
      s32_Size = std::ftell(pt_File);
      (void)std::fclose(pt_File);
   }
   return s32_Size;
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
bool stw_tgl::TGL_FileExists(const C_SCLString & orc_FileName)
{
   bool q_Return = false;
   HANDLE pv_Handle;
   WIN32_FIND_DATAA t_FindData;

   pv_Handle = FindFirstFileA(orc_FileName.c_str(), &t_FindData);
   if (pv_Handle != INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      (void)FindClose(pv_Handle);
      if ((t_FindData.dwFileAttributes & static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) == 0U)
      {
         q_Return = true;
      }
   }
   return q_Return;
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
bool TGL_PACKAGE stw_tgl::TGL_DirectoryExists(const C_SCLString & orc_Path)
{
   bool q_Return = false;
   const uint32 u32_Attrib = GetFileAttributesA(orc_Path.c_str());

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
static sint32 m_FileFind(const C_SCLString & orc_SearchPattern, SCLDynamicArray<TGL_FileSearchRecord> & orc_FoundFiles,
                         const bool oq_IncludeDirectories, SCLDynamicArray<uint8> * const opc_IsDirectory)
{
   WIN32_FIND_DATAA t_FindFileData;
   HANDLE pv_Find;
   uint8 u8_IsDirectory;

   orc_FoundFiles.SetLength(0);

   // Find the first file in the directory.
   pv_Find = FindFirstFileA(orc_SearchPattern.c_str(), &t_FindFileData);
   if (pv_Find == INVALID_HANDLE_VALUE) //lint !e923 //provided by system headers; no problems expected
   {
      return C_CONFIG;
   }

   u8_IsDirectory = ((t_FindFileData.dwFileAttributes & static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) ==
                     static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) ? 1U : 0U;
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
      u8_IsDirectory = ((t_FindFileData.dwFileAttributes & static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) ==
                        static_cast<uint32>(FILE_ATTRIBUTE_DIRECTORY)) ? 1U : 0U;
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
sint32 TGL_PACKAGE stw_tgl::TGL_FileFind(const C_SCLString & orc_SearchPattern,
                                         SCLDynamicArray<TGL_FileSearchRecord> & orc_FoundFiles)
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
C_SCLString TGL_PACKAGE stw_tgl::TGL_FileIncludeTrailingDelimiter(const C_SCLString & orc_Path)
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
C_SCLString TGL_PACKAGE stw_tgl::TGL_ExtractFileExtension(const C_SCLString & orc_Path)
{
   C_SCLString c_Extension;
   charn acn_Ext[_MAX_EXT + 1];

   _splitpath(orc_Path.c_str(), NULL, NULL, NULL, &acn_Ext[0]);
   c_Extension = static_cast<C_SCLString>(acn_Ext);
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
C_SCLString TGL_PACKAGE stw_tgl::TGL_ChangeFileExtension(const C_SCLString & orc_Path,
                                                         const C_SCLString & orc_Extension)
{
   uint32 u32_Pos;
   C_SCLString c_NewPath = orc_Path;

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
/*! \brief   Extract file path from full path

   Extract file path from a full file path.
   Can handle "/" and "\" path separators.

   \param[in]     orc_Path       full file path

   \return
   file path   (including final "\", "/" or ":")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString TGL_PACKAGE stw_tgl::TGL_ExtractFilePath(const C_SCLString & orc_Path)
{
   C_SCLString c_Path;
   charn acn_Drive[_MAX_DRIVE + 1];
   charn acn_Dir[_MAX_DIR + 1];

   _splitpath(orc_Path.c_str(), acn_Drive, acn_Dir, NULL, NULL);
   c_Path = static_cast<C_SCLString>(acn_Drive) + acn_Dir;
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
C_SCLString TGL_PACKAGE stw_tgl::TGL_ExtractFileName(const C_SCLString & orc_Path)
{
   C_SCLString c_FileName;
   charn acn_Name[_MAX_FNAME + 1];
   charn acn_Ext[_MAX_EXT + 1];

   _splitpath(orc_Path.c_str(), NULL, NULL, &acn_Name[0], &acn_Ext[0]);
   c_FileName = static_cast<C_SCLString>(acn_Name) + acn_Ext;
   return c_FileName;
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
sint32 TGL_PACKAGE stw_tgl::TGL_CreateDirectory(const C_SCLString & orc_Directory)
{
   sintn sn_Return;
   sint32 s32_Result = 0;

   if (TGL_DirectoryExists(orc_Directory.c_str()) == false)
   {
      sn_Return = CreateDirectoryA(orc_Directory.c_str(), NULL);
      if (sn_Return == FALSE)
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
sint32 TGL_PACKAGE stw_tgl::TGL_RemoveDirectory(const C_SCLString & orc_Directory,
                                                const bool oq_ContentOnly)
{
   C_SCLString c_FilePath; // Filepath
   C_SCLString c_Pattern;  // Pattern
   sint32 s32_Return;
   uint32 u32_Index;

   SCLDynamicArray<TGL_FileSearchRecord> c_Files;
   SCLDynamicArray<uint8> c_IsDirectory;

   c_Pattern = orc_Directory + "\\*.*";

   s32_Return = m_FileFind(c_Pattern, c_Files, true, &c_IsDirectory);
   if (s32_Return != C_NO_ERR)
   {
      s32_Return = -1;
   }
   else
   {
      for (u32_Index = 0U; u32_Index < static_cast<uint32>(c_Files.GetLength()); u32_Index++)
      {
         //delete content of directory
         if (c_Files[u32_Index].c_FileName[1] != '.') //ignore "." and ".."
         {
            c_FilePath = orc_Directory + "/" + c_Files[u32_Index].c_FileName;

            if (c_IsDirectory[u32_Index] == 1U)
            {
               // Delete subdirectory
               s32_Return = TGL_RemoveDirectory(c_FilePath, false);
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
C_SCLString TGL_PACKAGE stw_tgl::TGL_ExpandFileName(const C_SCLString & orc_RelativePath,
                                                    const C_SCLString & orc_BasePath)
{
   uint32 u32_Return;
   charn acn_OriginalDirectory[MAX_PATH];
   C_SCLString c_FullPath = "";

   //get original CWD:
   u32_Return = GetCurrentDirectoryA(MAX_PATH, acn_OriginalDirectory);
   if (u32_Return != 0)
   {
      //temporarily change CWD:
      u32_Return = SetCurrentDirectoryA(orc_BasePath.c_str());
      if (u32_Return != 0)
      {
         charn acn_AbsolutePath[MAX_PATH];
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

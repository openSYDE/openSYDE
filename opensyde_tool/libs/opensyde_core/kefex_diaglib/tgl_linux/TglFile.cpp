//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: File functions

   Implementation for linux.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <cstdio>
#include <ctime>
#include <stdlib.h>
#include <dirent.h>
#include <fnmatch.h>
#include <ftw.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
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
static bool m_FileAgeDosTime(const C_SclString & orc_FileName, uint16_t * const opu16_Date, uint16_t * const opu16_Time);
static int m_RemoveFile(const char * opcn_Pathname, const struct stat * opt_Stat, int osn_Type, struct FTW * opt_Ftwb);

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//utility: get operating system file age
static bool m_FileAgeDosTime(const C_SclString & orc_FileName, uint16_t * const opu16_Date, uint16_t * const opu16_Time)
{
   bool q_Return = false;
   struct stat t_stat;
   struct std::tm t_tm, *pt_tm;

   if (stat(orc_FileName.c_str(), &t_stat) == 0)
   {
      pt_tm = std::localtime(&t_stat.st_mtime);
      if (pt_tm != NULL)
      {
         t_tm = *pt_tm;
         // build date word
         *opu16_Date = (pt_tm->tm_year - 80) << 9 |  // year
		   (pt_tm->tm_mon  + 1 ) << 5 |  // month
		   pt_tm->tm_mday;               // month day
         // build time word
         *opu16_Time = pt_tm->tm_hour << 11 |        // hour
                       pt_tm->tm_min  << 5  |        // minutes
         pt_tm->tm_sec / 2;            // secondes
         // set return value
         q_Return = true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get file age time as string

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
   uint16_t u16_Time = 0;
   uint16_t u16_Date = 0;
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
/*! \brief  Get file size in bytes

   Return the specified file's file size in bytes.

   \param[in]     orc_FileName     path to file

   \return
   -1        error \n                  \n
   else      size of file in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t stw::tgl::TglFileSize(const C_SclString & orc_FileName)
{
   std::FILE * pt_File;
   int32_t s32_Size = -1;

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
/*! \brief  Check whether directory exists

   Detects whether the specified directory exists.
   For the Windows target this also includes simple drive names (e.g. "d:")

   \param[in]     orc_Path     path to directory (works with or without trailing path delimiter)

   \return
   true       directory exists  \n
   false      directory does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglDirectoryExists(const C_SclString & orc_Path)
{
   int32_t s32_Ret;
   struct stat t_Status;

   s32_Ret = stat(orc_Path.c_str(), &t_Status);
   return ((s32_Ret == 0) && S_ISDIR(t_Status.st_mode)) ? true : false;
}


//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check whether file exists

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
   struct stat t_stat;

   if (stat(orc_FileName.c_str(), &t_stat) == 0)
   {
      if (S_ISREG(t_stat.st_mode) != false)
      {
         q_Return = true;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find files

   Scans the file-system for files matching the specified pattern.
   Files with all attributes are detected.

   \param[in]     orc_SearchPattern search pattern (* and ? wildcards are possible)
   \param[out]    orc_FoundFiles    array with found files (without path, just name + extension)

   \return
   C_NO_ERR     at least one file found \n
   C_NOACT      no files found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t stw::tgl::TglFileFind(const C_SclString & orc_SearchPattern,
                              C_SclDynamicArray<C_TglFileSearchRecord> & orc_FoundFiles)
{
   int32_t s32_Error = C_CONFIG;
   DIR * pc_Dir;
   struct dirent *pt_Entry;
   C_SclString c_Path = TglExtractFilePath(orc_SearchPattern);
   C_SclString c_Pattern = TglExtractFileName(orc_SearchPattern);

   orc_FoundFiles.SetLength(0);

   pc_Dir = opendir(c_Path.c_str());
   if (pc_Dir != NULL)
   {
      while ((pt_Entry = readdir(pc_Dir)) != NULL)
      {
         if (pt_Entry->d_type == DT_REG)
         {
            if (fnmatch(c_Pattern.c_str(), pt_Entry->d_name, FNM_PATHNAME | FNM_NOESCAPE) == 0)
            {
               orc_FoundFiles.IncLength();
               orc_FoundFiles[orc_FoundFiles.GetHigh()].c_FileName = pt_Entry->d_name;
               s32_Error = C_NO_ERR;
            }
         }
      }
      closedir(pc_Dir);
   }

   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Append trailing path delimiter

   Append path delimiter to path if it does not already end in one.
   The path separator character can be target specific (e.g. backslash for Windows; slash for Linux)

   \param[in]     orc_Path       path (with or without ending delimiter)

   \return
   path with delimiter
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString stw::tgl::TglFileIncludeTrailingDelimiter(const C_SclString & orc_Path)
{
   if (orc_Path.Length() == 0)
   {
      return "/";
   }
   if (orc_Path.operator[](orc_Path.Length()) != '/')
   {
      return orc_Path + "/";
   }
   return orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract file extension

   Extract a file extension separated from the file name by a ".".
   If there is more than one "." in the file path the last one will be used.
   If the file does not have an extension an empty string is returned.

   \param[in]     orc_Path       full file path (or just name with extension)

   \return
   extension (includes the ".")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString stw::tgl::TglExtractFileExtension(const C_SclString & orc_Path)
{
   uint32_t u32_Pos;
   C_SclString c_Extension = "";

   u32_Pos = orc_Path.LastPos(".");
   if (u32_Pos != 0U)
   {
      c_Extension = orc_Path.SubString(u32_Pos, INT_MAX); //get everything starting with the "."
   }
   return c_Extension;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change file extension

   Extract a file extension separated from the file name by a ".".
   If there is more than one "." in the file path the last one will be used.
   Then replace it with the new one.

   \param[in]     orc_Path       full file path (or just name with extension)
   \param[in]     orc_Extension  new extension (must be specified with the ".")

   \return
   new file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString stw::tgl::TglChangeFileExtension(const C_SclString & orc_Path, const C_SclString & orc_Extension)
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
C_SclString stw::tgl::TglGetExePath(void)
{
    int32_t s32_Return;
    char_t acn_Arg[20];
    std::vector<char_t> c_VecPath;
    C_SclString c_Path;
    bool q_Success = false;
    int32_t s32_BuffSize;

    c_VecPath.resize(1, 0);

    std::sprintf(acn_Arg, "/proc/%d/exe", getpid());
    do
    {
      c_VecPath.resize(c_VecPath.size() + PATH_MAX, 0);
      // Reserving one byte for null termination
      s32_BuffSize = c_VecPath.size() - 1;

      s32_Return = readlink(acn_Arg, &c_VecPath[0], s32_BuffSize);

      if ((s32_Return >= 0) &&
          (s32_Return < s32_BuffSize))
      {
         // Success: Buffer was big enoug and no error occured
         q_Success = true;
      }
      else if (s32_Return < 0)
      {
         // A not buffer size specific error which can not be fixed by trying again with a bigger buffer size
         // Special case: The error "errno == ENAMETOOLONG" seems no to be fixable by
         // increasing the buffer size in all cases. Risking an endless loop
         break;
      }
      else
      {
         // The buffer was probably too small, try a further iteration with an increased buffer size:
         // sn_Return is bigger or equal to sn_BuffSize. This is an indicator for a truncated part of the path
      }
    }
    while (q_Success == false);

    if (q_Success == true)
    {
       //we got a path ...
       c_Path = &c_VecPath[0];
    }

    return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract file path from full path

   Extract file path from a full file path.
   The path separator character can be target specific.

   \param[in]     orc_Path       full file path

   \return
   file path   (including final "\" or ":")
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString stw::tgl::TglExtractFilePath(const C_SclString & orc_Path)
{
   uint32_t u32_Return;
   C_SclString c_Path = ".";
   u32_Return = orc_Path.LastPos("/");

   if (u32_Return != 0U)
   {
      c_Path = orc_Path.SubString(1U, u32_Return);
   }
   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Extract file name from full path

   Extract file name from a full file path.
   The path separator character can be target specific.

   \param[in]     orc_Path       full file path

   \return
   file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString stw::tgl::TglExtractFileName(const C_SclString & orc_Path)
{
   uint32_t u32_Return;
   C_SclString c_FileName;
   u32_Return = orc_Path.LastPos("/");

   if (u32_Return != 0U)
   {
      c_FileName = orc_Path.SubString(u32_Return+1, orc_Path.Length());
   }
   else
   {
      c_FileName = orc_Path;
   }
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
C_SclString stw::tgl::TglExpandFileName(const C_SclString & orc_RelativePath, const C_SclString & orc_BasePath)
{
   char_t * pcn_Path;
   C_SclString c_RelPath = orc_BasePath + "/" + orc_RelativePath;
   C_SclString c_FullPath;

   pcn_Path = realpath(c_RelPath.c_str(), NULL);
   if (pcn_Path != NULL)
   {
      c_FullPath = pcn_Path;
      free(pcn_Path);
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
int32_t stw::tgl::TglCreateDirectory(const C_SclString & orc_Directory)
{
   int32_t s32_Ret;
   int32_t s32_Result = -1;
   struct stat t_Status;

   s32_Ret = stat(orc_Directory.c_str(), &t_Status);
   if ((s32_Ret == 0) && S_ISDIR(t_Status.st_mode))
   {
      // Directory already exists
      s32_Result = 0;
   }
   else
   {
      // Directory does not exist, create it
      s32_Ret =  mkdir(orc_Directory.c_str(), 0777);
      if (s32_Ret == 0)
      {
         s32_Result = 0;
      }
   }

   return s32_Result;
}

//---------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove a single file or directory entry. This function is the callback used in the nftw function

   \param[in]   opcn_Pathname             File or directory to be removed
   \param[in]   opt_Stat                  Stat info of the file / directory
   \param[in]   osn_Type                  File type flags
   \param[in]   opt_Ftwb                  File location in path

   \return
   0     file/directory removed
   -1    could not remove file directory
*/
//---------------------------------------------------------------------------------------------------------------------
static int m_RemoveFile(const char * opcn_Pathname, const struct stat * opt_Stat, int osn_Type, struct FTW * opt_Ftwb)
{
   int32_t s32_Ret = 0;
   (void)opt_Stat;
   (void)osn_Type;
   (void)opt_Ftwb;

   // Do not delete the base folder here
   if (opt_Ftwb->level > 0)
   {
      s32_Ret = remove(opcn_Pathname);
   }
   return s32_Ret;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove directory with subdirectories

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
int32_t stw::tgl::TglRemoveDirectory(const C_SclString & orc_Directory, const bool oq_ContentOnly)
{
   int32_t s32_Ret = -1;
   bool q_DirExists;

   // Make sure orc_Directory is a directory
   q_DirExists = TglDirectoryExists(orc_Directory);
   if (q_DirExists == true)
   {
      // Delete directory content
      s32_Ret = nftw(orc_Directory.c_str(), &m_RemoveFile, 10, (FTW_DEPTH | FTW_MOUNT | FTW_PHYS));
      if ((s32_Ret == 0) && (oq_ContentOnly == false))
      {
         // Delete the base directory if requested
         s32_Ret = remove(orc_Directory.c_str());
      }
   }

   return s32_Ret;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether specified path is a relative path

   For Linux:
   An absolute path starts with "/".

   \param[in]   orc_Path             Path to check

   \return
   true   path is a relative path (or empty)
   false  path is an absolute path
*/
//----------------------------------------------------------------------------------------------------------------------
bool stw::tgl::TglIsRelativePath(const C_SclString & orc_Path) 
{
   bool q_IsAbsolute = false;

   if (orc_Path.Length() >= 1U)
   {
      if (orc_Path[1] == '/')
      {
         q_IsAbsolute = true;
      }
   }
   return !q_IsAbsolute;
}

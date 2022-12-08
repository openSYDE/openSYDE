//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE Core AES file encryption utility

   see header in .h file for details.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <fstream>

#include "TglFile.hpp"
#include "TglUtils.hpp"
#include "AES.h"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscAesFile.hpp"
#include "C_SclString.hpp"
#include "C_Md5Checksum.hpp"
#include "C_OscZipFile.hpp"
#include "C_OscLoggingHandler.hpp"

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
/*! \brief   Create encrypted file

   Create AES encrypted file using ECB mode.
   As the algorithm only can work with blocks of 16 bytes. So we apply PKCS#7 algorithm to fill up.
   The library we use supports padding, but only with zeros, not with PKCS#7.

   Steps:
   * create an MD5 hash over the passed key to use as an 128bit AES encryption key
   * load input file to RAM
   * append values for PKCS#7 algorithm
   * perform encryption in RAM (so not suitable for very large files)
   * write resulting output file

   If the output file already exists the function will overwrite it.

   \param[in]  orc_Key                       Key to use for encryption. Technically a MD5 hash over this key will
                                              be used as 128bit AES encryption key.
   \param[in]  orc_InFilePath                Path to input file
   \param[in]  orc_OutFilePath               Path to resulting output file

   \return
   C_NO_ERR    success
   C_RD_WR     input file does not exist or could not be loaded
   C_NOACT     output could not be written
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscAesFile::h_EncryptFile(const C_SclString & orc_Key, const C_SclString & orc_InFilePath,
                                    const C_SclString & orc_OutFilePath)

{
   int32_t s32_Return = C_NO_ERR;

   //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?& anyway
   const C_SclString c_AesKey = stw::md5::C_Md5Checksum::GetMD5(
      reinterpret_cast<const uint8_t *>(orc_Key.c_str()), orc_Key.Length());

   tgl_assert(c_AesKey.Length() == 32); //really should be 16 bytes, resp. 32 hex characters

   //check whether input file exists:
   if (TglFileExists(orc_InFilePath) == false)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      //load data from input file:
      vector<uint8_t> c_InputData;
      std::ifstream c_InputFileStream;
      const uint32_t u32_InputFileSize = static_cast<uint32_t>(TglFileSize(orc_InFilePath));
      const uint8_t u8_Pkcs7Size = static_cast<uint8_t>(16U - (u32_InputFileSize % 16U));

      c_InputData.resize(static_cast<size_t>(u32_InputFileSize) + u8_Pkcs7Size);

      c_InputFileStream.open(orc_InFilePath.c_str(), std::ifstream::binary);

      if (c_InputFileStream.is_open() == false)
      {
         s32_Return = C_RD_WR;
      }
      else
      {
         //read file content
         bool q_HasFailed;
         //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?& anyway
         c_InputFileStream.read(reinterpret_cast<char_t *>(&c_InputData[0]), u32_InputFileSize);
         //check for error
         q_HasFailed = c_InputFileStream.fail();
         //close file
         c_InputFileStream.close();

         if (q_HasFailed == true)
         {
            s32_Return = C_RD_WR;
         }
         else
         {
            //do the encryption:
            uint8_t au8_Key[16];
            uint8_t * pu8_EncryptedData;
            unsigned int x_EncryptedSize; //lint !e8080 !e970  //using type to match library interface
            std::ofstream c_OutputFileStream;
            AES c_Aes(128);

            //add PKCS#7 values:
            for (uint8_t u8_ByteIndex = 0U; u8_ByteIndex < u8_Pkcs7Size; u8_ByteIndex++)
            {
               c_InputData[static_cast<size_t>(u32_InputFileSize) + u8_ByteIndex] = u8_Pkcs7Size;
            }

            //convert key from string to array:
            for (uint8_t u8_Index = 0U; u8_Index < 16U; u8_Index++)
            {
               const C_SclString c_Text = "0x" + c_AesKey.SubString((static_cast<uint32_t>(u8_Index) * 2U) + 1U, 2U);
               au8_Key[u8_Index] = static_cast<uint8_t>(c_Text.ToInt());
            }

            pu8_EncryptedData = c_Aes.EncryptECB(
               &c_InputData[0],
               static_cast<unsigned int>(c_InputData.size()), //lint !e970  //using type to match library interface
               &au8_Key[0],
               x_EncryptedSize);
            //we added the padding manually: no magic expected here ...
            tgl_assert(x_EncryptedSize == c_InputData.size());

            //save to output file:
            c_OutputFileStream.open(orc_OutFilePath.c_str(), std::ofstream::binary | std::ofstream::trunc);
            if (c_OutputFileStream.is_open())
            {
               //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?&
               // anyway
               c_OutputFileStream.write(reinterpret_cast<const char_t *>(pu8_EncryptedData), x_EncryptedSize);
               q_HasFailed = c_OutputFileStream.fail();
               c_OutputFileStream.close();
               if (q_HasFailed == true)
               {
                  s32_Return = C_NOACT;
               }
            }
            else
            {
               s32_Return = C_NOACT;
            }

            //clean up memory allocated by AES library:
            delete[] pu8_EncryptedData;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create decrypted file

   Steps:
   * create an MD5 hash over the passed key to use as an 128bit AES encryption key
   * load input file
   * perform decryption in RAM (so not suitable for very large files)
   * size decrypted data according to PKCS#7 algorithm
   * write resulting output file

   If the output file already exists the function will overwrite it.

   The size of the file must be a multiple of 16bytes.
   The file must have been written with PKCS#7 algorithm.

   \param[in]  orc_Key                       Key to use for decryption. Technically a MD5 hash over this key will
                                              be used as 128bit AES decryption key.
   \param[in]  orc_InFilePath                Path to input file
   \param[in]  orc_OutFilePath               Path to resulting output file

   \return
   C_NO_ERR    success
   C_RD_WR     input file does not exist or could not be loaded
   C_CONFIG    input file size is not a multiple of 16
   C_CHECKSUM  input file is invalid; PKCS#7 value is > 16 or > file size (checked after decryption)
   C_NOACT     output file could not be written
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscAesFile::h_DecryptFile(const C_SclString & orc_Key, const C_SclString & orc_InFilePath,
                                    const C_SclString & orc_OutFilePath)

{
   int32_t s32_Return = C_NO_ERR;

   //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?& anyway
   const C_SclString c_AesKey = stw::md5::C_Md5Checksum::GetMD5(
      reinterpret_cast<const uint8_t *>(orc_Key.c_str()), orc_Key.Length());

   tgl_assert(c_AesKey.Length() == 32); //really should be 16 bytes, resp. 32 hex characters

   //check whether input file exists:
   if (TglFileExists(orc_InFilePath) == false)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      //load data from input file:
      vector<uint8_t> c_InputData;
      std::ifstream c_InputFileStream;
      const uint32_t u32_InputFileSize = TglFileSize(orc_InFilePath);

      //is the file correctly padded ?
      if ((u32_InputFileSize % 16U) != 0U)
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         c_InputData.resize(TglFileSize(orc_InFilePath));

         c_InputFileStream.open(orc_InFilePath.c_str(), std::ifstream::binary);

         if (c_InputFileStream.is_open() == false)
         {
            s32_Return = C_RD_WR;
         }
         else
         {
            //read file content
            bool q_HasFailed;
            //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?&
            // anyway
            c_InputFileStream.read(reinterpret_cast<char_t *>(&c_InputData[0]), c_InputData.size());
            //check for error
            q_HasFailed = c_InputFileStream.fail();
            //close file
            c_InputFileStream.close();

            if (q_HasFailed == true)
            {
               s32_Return = C_RD_WR;
            }
            else
            {
               //do the decryption:
               uint8_t au8_Key[16];
               uint8_t * pu8_DecryptedData;
               std::ofstream c_OutputFileStream;
               uint8_t u8_Pkcs7Value;
               AES c_Aes(128);
               //using type to match library interface
               const unsigned int x_InputSize = static_cast<unsigned int>(c_InputData.size()); //lint  !e8080 !e970

               //convert key from string to array:
               for (uint8_t u8_Index = 0U; u8_Index < 16U; u8_Index++)
               {
                  const C_SclString c_Text = "0x" + c_AesKey.SubString((static_cast<uint32_t>(u8_Index) * 2U) + 1U, 2U);
                  au8_Key[u8_Index] = static_cast<uint8_t>(c_Text.ToInt());
               }

               pu8_DecryptedData = c_Aes.DecryptECB(&c_InputData[0], x_InputSize, &au8_Key[0]);
               //remove padding:
               u8_Pkcs7Value = pu8_DecryptedData[c_InputData.size() - 1];
               if ((u8_Pkcs7Value > 16) || (u8_Pkcs7Value > c_InputData.size()))
               {
                  //this is unexpected; possible reasons: incorrect key; not a file encrypted with AES + PKCS#7
                  s32_Return = C_CHECKSUM;
               }
               else
               {
                  //save to output file:
                  c_OutputFileStream.open(orc_OutFilePath.c_str(), std::ofstream::binary | std::ofstream::trunc);
                  if (c_OutputFileStream.is_open())
                  {
                     //write without the PKCS#7 bytes:
                     //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep
                     // !"=?& anyway
                     c_OutputFileStream.write(reinterpret_cast<const char_t *>(pu8_DecryptedData),
                                              static_cast<streamsize>(c_InputData.size()) - u8_Pkcs7Value);
                     q_HasFailed = c_OutputFileStream.fail();
                     c_OutputFileStream.close();
                     if (q_HasFailed == true)
                     {
                        s32_Return = C_NOACT;
                     }
                  }
                  else
                  {
                     s32_Return = C_NOACT;
                  }
               }
               //clean up memory allocated by AES library:
               delete[] pu8_DecryptedData;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create a zip file will all files of a folder and encrypts it with AES

   \param[in]  orc_FolderPathToZip  Base folder to pack into zip package
   \param[in]  orc_FilesToZip       All files which will be part of the package
   \param[in]  orc_PathForZipFile   File path for generated zip file
   \param[in]  orc_Key              Key for AES encryption (In case of an empty key, the file will not be encrypted)
   \param[out] opc_ErrorMessage     Optional string for error messages

   \return
   C_NO_ERR    Encrypted zip file generated
   C_CONFIG    at least one input file does not exist
   C_RD_WR     could not open input file
               could not write output file
   C_NOACT     could not add data to zip file (does the path to the file exist ?)
               output file could not be written
   C_BUSY      Problems with deleting the temporary file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscAesFile::h_CreateEncryptedZipFile(const C_SclString & orc_FolderPathToZip,
                                               const std::vector<C_SclString> & orc_FilesToZip,
                                               const C_SclString & orc_PathForZipFile, const C_SclString & orc_Key,
                                               C_SclString * const opc_ErrorMessage)
{
   int32_t s32_Return;

   std::set<C_SclString> c_SupFiles;
   const C_SclString c_ZipFileTmp = orc_PathForZipFile + static_cast<C_SclString>("_tmp");
   C_SclString c_ErrorText;

   C_OscZipFile::h_AppendFilesRelative(c_SupFiles, orc_FilesToZip, orc_FolderPathToZip);

   s32_Return = C_OscZipFile::h_CreateZipFile(orc_FolderPathToZip, c_SupFiles, c_ZipFileTmp, &c_ErrorText);

   if (s32_Return == C_NO_ERR)
   {
      if (orc_Key != "")
      {
         s32_Return = C_OscAesFile::h_EncryptFile(orc_Key, c_ZipFileTmp, orc_PathForZipFile);
      }
      else
      {
         // No key, just copy the original zip file as result
         osc_write_log_info("Creating Encrypted Zip File", "No key defined. Encryption not necessary.");

         s32_Return = C_OscUtils::h_CopyFile(c_ZipFileTmp, orc_PathForZipFile, NULL, &c_ErrorText);
      }

      // Remove the non encrypted temporary file
      if (std::remove(c_ZipFileTmp.c_str()) != 0)
      {
         if (s32_Return == C_NO_ERR)
         {
            // Do not overwrite an other error code
            s32_Return = C_BUSY;
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Creating Encrypted Zip File", "Encrypting zip file failed with error: " +
                             C_OscLoggingHandler::h_StwError(s32_Return));
      }
      else
      {
         osc_write_log_info("Creating Encrypted Zip File", "Encrypted zip file created.");
      }
   }
   else
   {
      osc_write_log_error("Creating Encrypted Zip File", "Creating zip file failed with error: " +
                          C_OscLoggingHandler::h_StwError(s32_Return) +
                          " and error text: " + c_ErrorText.c_str());

      if (opc_ErrorMessage != NULL)
      {
         *opc_ErrorMessage = c_ErrorText;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decrypts and unpacks a zip file to a specific folder

   Sequence (if "orc_Key" is not an empty string):
   * decrypt file "orc_PathOfZipFile" to file "<orc_PathOfZipFile>_tmp"
   * extract content of "<orc_PathOfZipFile>_tmp" to path "orc_FolderPathToUnzip"
   * remove file "<orc_PathOfZipFile>_tmp"

   \param[in]  orc_PathOfZipFile       File path of encrypted zip package
   \param[in]  orc_FolderPathToUnzip   Folder to unpack zip package
   \param[in]  orc_Key                 Key for AES decryption (In case of an empty key, the file will not be decypted)
   \param[out] opc_ErrorMessage        Optional string for error messages

   \return
   C_NO_ERR    Encrypted zip file unpacked
   C_RD_WR     input file does not exist or could not be loaded
               could not unpack archive to target path
   C_BUSY      Problems with deleting the temporary file
   C_CONFIG    input file size is not a multiple of 16
   C_CHECKSUM  input file is invalid; PKCS#7 value is > 16 or > file size (checked after decryption)
   C_NOACT     output file already exists or could not be written
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscAesFile::h_UnpackEncryptedZipFile(const C_SclString & orc_PathOfZipFile,
                                               const C_SclString & orc_FolderPathToUnzip, const C_SclString & orc_Key,
                                               C_SclString * const opc_ErrorMessage)
{
   int32_t s32_Return;
   C_SclString c_ZipFileTmp = orc_PathOfZipFile + static_cast<C_SclString>("_tmp");
   bool q_TemporaryFileUsed = true;

   if (orc_Key != "")
   {
      s32_Return = C_OscAesFile::h_DecryptFile(orc_Key, orc_PathOfZipFile, c_ZipFileTmp);
   }
   else
   {
      // No encrypted zip file. The original file can be used directly
      c_ZipFileTmp = orc_PathOfZipFile;
      q_TemporaryFileUsed = false;
      s32_Return = C_NO_ERR;
   }

   if (s32_Return == C_NO_ERR)
   {
      C_SclString c_ErrorText;
      s32_Return = C_OscZipFile::h_UnpackZipFile(c_ZipFileTmp, orc_FolderPathToUnzip, &c_ErrorText);

      if (q_TemporaryFileUsed == true)
      {
         // Remove the non encrypted temporary file
         if (std::remove(c_ZipFileTmp.c_str()) != 0)
         {
            if (s32_Return == C_NO_ERR)
            {
               // Do not overwrite an other error code, but remove the file in any case
               s32_Return = C_BUSY;
            }
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Unpacking Encrypted Zip File", "Unpacking decrypted zip file failed with error: " +
                             C_OscLoggingHandler::h_StwError(s32_Return) +
                             " and error text: " + c_ErrorText.c_str());

         if (opc_ErrorMessage != NULL)
         {
            *opc_ErrorMessage = c_ErrorText;
         }
      }
   }
   else
   {
      osc_write_log_error("Unpacking Encrypted Zip File", "Decrypting zip file failed with error: " +
                          C_OscLoggingHandler::h_StwError(s32_Return));
   }

   return s32_Return;
}

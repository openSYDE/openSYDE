//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Data Block file details (implementation)

   Widget for Data Block file details that displays HEX file information.

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>

#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "C_FlaUpHexFileInfo.hpp"
#include "C_OscHexFile.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *
 *  \param[in,out]  orc_File    Hex file to load
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpHexFileInfo::C_FlaUpHexFileInfo(const QString & orc_File) :
   s32_CurrentHexFileIndex(0)
{
   SetHexFileInfo(orc_File);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set hex file information

   \param[in]     orc_File    HEX file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpHexFileInfo::SetHexFileInfo(const QString & orc_File)
{
   const QFileInfo c_FileInfo(orc_File);
   C_OscHexFile c_HexFile;
   uint32_t u32_Result;

   C_SclDynamicArray<stw::diag_lib::C_XFLECUInformation> c_InfoBlocks;

   c_BlockInfo.resize(0);
   c_HexFileInfo.s32_NumberOfBlocks = 0;
   c_HexFileInfo.c_FileName = c_FileInfo.fileName();

   u32_Result = c_HexFile.LoadFromFile(orc_File.toStdString().c_str());
   c_HexFile.GetECUInformationBlocks(c_InfoBlocks, 0UL, false, false, false);

   if (u32_Result == stw::hex_file::NO_ERR)
   {
      uint32_t u32_Crc;
      c_HexFileInfo.c_TimeStamp = c_FileInfo.lastModified().toString("dd.MM.yyyy HH:mm:ss");
      c_HexFileInfo.c_Size = static_cast<QString>("%1 bytes").arg(c_FileInfo.size());
      c_HexFileInfo.c_Checksum = mh_GetMd5Hex(orc_File);
      c_HexFileInfo.c_NumberOfBytes = QString::number(c_HexFile.ByteCount());
      c_HexFileInfo.c_BitCrc = static_cast<QString>("0x%1").arg(c_HexFile.CalcFileChecksum(u32_Crc), 0, 16);
      if (c_InfoBlocks.GetLength() > 0)
      {
         c_HexFileInfo.s32_NumberOfBlocks = c_InfoBlocks.GetLength();
      }
      else
      {
         c_HexFileInfo.s32_NumberOfBlocks = 0;
      }
      if (c_InfoBlocks.GetLength() > 0)
      {
         c_BlockInfo.resize(c_InfoBlocks.GetLength());
         for (int32_t s32_ItAppl = 0UL; s32_ItAppl < c_InfoBlocks.GetLength(); ++s32_ItAppl)
         {
            const stw::diag_lib::C_XFLECUInformation & rc_CurInfo = c_InfoBlocks[s32_ItAppl];
            c_BlockInfo.at(s32_ItAppl).c_BlockType = rc_CurInfo.GetInfoLevelAsString().c_str();
            c_BlockInfo.at(s32_ItAppl).c_Version = QString::number(rc_CurInfo.u8_StructVersion);
            if (rc_CurInfo.ContainsDeviceID() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_DeviceId = rc_CurInfo.GetDeviceID().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_DeviceId = C_GtGetText::h_GetText("Unknown");
            }
            if (rc_CurInfo.ContainsDateAndTime() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_Date = rc_CurInfo.GetDate().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_Date = C_GtGetText::h_GetText("Unknown");
            }
            if (rc_CurInfo.ContainsDateAndTime() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_Time = rc_CurInfo.GetTime().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_Time = C_GtGetText::h_GetText("Unknown");
            }
            if (rc_CurInfo.ContainsProjectName() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_ProjectName = rc_CurInfo.GetProjectName().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_ProjectName = C_GtGetText::h_GetText("Unknown");
            }
            if (rc_CurInfo.ContainsProjectVersion() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_ProjectVersion = rc_CurInfo.GetProjectVersion().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_ProjectVersion = C_GtGetText::h_GetText("Unknown");
            }
            if (rc_CurInfo.ContainsAdditionalInfo() == true)
            {
               c_BlockInfo.at(s32_ItAppl).c_AdditionalInfo = rc_CurInfo.GetAdditionalInfo().c_str();
            }
            else
            {
               c_BlockInfo.at(s32_ItAppl).c_AdditionalInfo = C_GtGetText::h_GetText("Unknown");
            }
         }
      }
   }
   else
   {
      c_HexFileInfo.c_TimeStamp = "No information available";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get MD5 checksum for file encoded in hex

   \param[in] orc_Path File path

   \return
   MD5 checksum for file encoded in hex
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_FlaUpHexFileInfo::mh_GetMd5Hex(const QString & orc_Path)
{
   QString c_Retval;
   QFile c_File(orc_Path);

   if (c_File.open(QFile::ReadOnly))
   {
      QCryptographicHash c_Md5Algorithm(QCryptographicHash::Md5);
      if (c_Md5Algorithm.addData(&c_File))
      {
         c_Retval = c_Md5Algorithm.result().toHex();
      }
      c_File.close();
   }
   return c_Retval;
}

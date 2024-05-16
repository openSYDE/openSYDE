//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Data Block file details (header)

   See cpp file for detailed description
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPHEXFILEINFO_H
#define C_FLAUPHEXFILEINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUpHexFileInfo
{
public:
   C_FlaUpHexFileInfo(const QString & orc_File);
   void SetHexFileInfo(const QString & orc_File);
   class C_HexFileInfo
   {
   public:
      QString c_TimeStamp;
      QString c_Size;
      QString c_Checksum;
      QString c_NumberOfBytes;
      QString c_BitCrc;
      QString c_FileName;
      QString c_FilePath;
      int32_t s32_NumberOfBlocks;
   };

   C_HexFileInfo c_HexFileInfo;

   class C_BlockInfo
   {
   public:
      QString c_BlockType;
      QString c_Version;
      QString c_DeviceId;
      QString c_Date;
      QString c_Time;
      QString c_ProjectName;
      QString c_ProjectVersion;
      QString c_AdditionalInfo;
   };

   std::vector<C_BlockInfo> c_BlockInfo;
   int32_t s32_CurrentHexFileIndex;

private:
   static QString mh_GetMd5Hex(const QString & orc_Path);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

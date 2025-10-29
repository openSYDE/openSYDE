//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (implementation)

   Handle save'n load for user settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>
#include <QDir>
#include "stwerrors.hpp"
#include "C_Uti.hpp"
#include "C_UsFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsFiler::C_UsFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user setting to default ini file

   \param[in]  orc_UserSettings  User settings to save
   \param[in]  orc_Path          File path

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsFiler::h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path)
{
   int32_t s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      s32_Retval = C_NO_ERR;
      {
         //Helper to seperate path and file name
         const QFileInfo c_File(orc_Path);
         //Check if directory exists
         const QDir c_Dir(c_File.path());
         if (c_Dir.exists() == false)
         {
            c_Dir.mkpath(".");
         }
      }
      try
      {
         //Parse ini
         C_SclIniFile c_Ini(orc_Path.toStdString().c_str());
         mh_SaveProjectIndependentSection(orc_UserSettings, c_Ini);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all values of ini file

   If ini not existing set default values.

   \param[in,out]  orc_UserSettings    User settings to load
   \param[in]      orc_Path            File path

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsFiler::h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path)
{
   int32_t s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      try
      {
         C_SclIniFile c_Ini(orc_Path.toStdString().c_str());
         s32_Retval = C_NO_ERR;

         orc_UserSettings.SetDefault();
         mh_LoadProjectIndependentSection(orc_UserSettings, c_Ini);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project independent part of user settings

   \param[in]      orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   const QStringList c_HexFilePaths = orc_UserSettings.GetLastKnownUpdateHexFilePaths();
   const QStringList c_HexFilePathsAsRelativeOrAbsolute = orc_UserSettings.GetHexFilePathsAsRelativeOrAbsolute();

   //Screen position
   orc_Ini.WriteInteger("Screen", "Position_x", orc_UserSettings.GetScreenPos().x());
   orc_Ini.WriteInteger("Screen", "Position_y", orc_UserSettings.GetScreenPos().y());

   // Application size
   orc_Ini.WriteInteger("Screen", "Size_width", orc_UserSettings.GetAppSize().width());
   orc_Ini.WriteInteger("Screen", "Size_height", orc_UserSettings.GetAppSize().height());

   // Application maximizing flag
   orc_Ini.WriteBool("Screen", "Size_maximized", orc_UserSettings.GetAppMaximized());

   // Application screen index
   orc_Ini.WriteInteger("Screen", "Screen_index", orc_UserSettings.GetAppScreenIndex());

   // Settings splitter
   orc_Ini.WriteInteger("Layout", "SettingsSplitter_x", orc_UserSettings.GetSplitterSettingsHorizontal());
   orc_Ini.WriteBool("Layout", "Settings_expanded", orc_UserSettings.GetSettingsAreExpanded());

   // Settings expanded collapsed
   orc_Ini.WriteBool("Settings", "ProgressExpanded", orc_UserSettings.GetWiProgressExpanded());
   orc_Ini.WriteBool("Settings", "AdvSettExpanded", orc_UserSettings.GetWiAdvSettExpanded());
   orc_Ini.WriteBool("Settings", "DllExpanded", orc_UserSettings.GetWiDllConfigExpanded());
   orc_Ini.WriteInteger("Settings", "PopOpenSection", static_cast<int32_t>(orc_UserSettings.GetPopOpenSection()));
   orc_Ini.WriteString("Settings", "CustomCanDllPath", orc_UserSettings.GetCustomCanDllPath().toStdString().c_str());
   orc_Ini.WriteString("Settings", "CanDllType", C_UsFiler::mh_GetStringFromDllType(
                          orc_UserSettings.GetCanDllType()).toStdString().c_str());

   // Values of Property widget
   orc_Ini.WriteInteger("Properties", "NodeId", orc_UserSettings.GetPropNodeId());
   orc_Ini.WriteInteger("Properties", "Bitrate", orc_UserSettings.GetPropBitrate());

   // Values of Node Configuration Dialog
   orc_Ini.WriteInteger("Properties", "InterfaceIndex", orc_UserSettings.GetNodeCfgInterfaceIndex());

   // Advanced Properties values
   orc_Ini.WriteInteger("Advanced_Properties", "FlashloaderResetWaitTime",
                        orc_UserSettings.GetFlashloaderResetWaitTime());
   orc_Ini.WriteInteger("Advanced_Properties", "RequestDownloadTimeout", orc_UserSettings.GetRequestDownloadTimeout());
   orc_Ini.WriteInteger("Advanced_Properties", "TransferDataTimeout", orc_UserSettings.GetTransferDataTimeout());

   // Values from Update widget
   orc_Ini.WriteInteger("Update", "HexFileCount", c_HexFilePaths.size());
   for (int32_t s32_SectionCounter = 0; s32_SectionCounter < static_cast<int32_t>(c_HexFilePaths.size());
        ++s32_SectionCounter)
   {
      const std::string c_HexFilePath = c_HexFilePaths[s32_SectionCounter].toStdString();
      orc_Ini.WriteString("Update", "HexFiles[" + std::to_string(s32_SectionCounter) + "]",
                          c_HexFilePath);
      const std::string c_HexFilePathAsRelativeOrAbsolute =
         c_HexFilePathsAsRelativeOrAbsolute[s32_SectionCounter].toStdString();
      orc_Ini.WriteString("Update", "HexFilesAsRelativeOrAbsolute[" + std::to_string(s32_SectionCounter) + "]",
                          c_HexFilePathAsRelativeOrAbsolute);
   }

   orc_Ini.WriteString("Update", "LastKnownHexFileLocation",
                       orc_UserSettings.GetLastKnownUpdateHexFileLocation().toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI project independent section

   \param[in,out]  orc_UserSettings    User settings
   \param[in,out]  orc_Ini             Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, C_SclIniFile & orc_Ini)
{
   QPoint c_Pos;
   QSize c_Size;
   bool q_Flag;
   int32_t s32_Value;
   QString c_Tmp;
   QStringList c_HexFilePaths;
   QStringList c_HexFilePathsAsRelativeOrAbsolute;

   // Screen position
   c_Pos.setX(orc_Ini.ReadInteger("Screen", "Position_x", 50));
   c_Pos.setY(orc_Ini.ReadInteger("Screen", "Position_y", 50));
   orc_UserSettings.SetScreenPos(c_Pos);

   // Application size
   c_Size.setWidth(orc_Ini.ReadInteger("Screen", "Size_width", 1000));
   c_Size.setHeight(orc_Ini.ReadInteger("Screen", "Size_height", 700));
   orc_UserSettings.SetAppSize(c_Size);

   // Application maximizing flag
   q_Flag = orc_Ini.ReadBool("Screen", "Size_maximized", true);
   orc_UserSettings.SetAppMaximized(q_Flag);

   // Application screen index
   s32_Value = orc_Ini.ReadInteger("Screen", "Screen_index", 0);
   orc_UserSettings.SetAppScreenIndex(static_cast<uint32_t>(s32_Value));

   // Settings splitter
   s32_Value = orc_Ini.ReadInteger("Layout", "SettingsSplitter_x", 0);
   orc_UserSettings.SetSplitterSettingsHorizontal(s32_Value);
   q_Flag = orc_Ini.ReadBool("Layout", "Settings_expanded", true);
   orc_UserSettings.SetSettingsAreExpanded(q_Flag);

   // Settings expanded collapsed
   q_Flag = orc_Ini.ReadBool("Settings", "ProgressExpanded", true);
   orc_UserSettings.SetWiProgressExpanded(q_Flag);
   q_Flag = orc_Ini.ReadBool("Settings", "AdvSettExpanded", true);
   orc_UserSettings.SetWiAdvSettExpanded(q_Flag);
   q_Flag = orc_Ini.ReadBool("Settings", "DllExpanded", true);
   orc_UserSettings.SetWiDllConfigExpanded(q_Flag);
   s32_Value = orc_Ini.ReadInteger("Settings", "PopOpenSection",
                                   static_cast<int32_t>(C_UsHandler::E_SettingsSubSection::eNONE));
   orc_UserSettings.SetPopOpenSection(static_cast< C_UsHandler::E_SettingsSubSection>(s32_Value));
   c_Tmp = orc_Ini.ReadString("Settings", "CustomCanDllPath", "").c_str();
   orc_UserSettings.SetCustomCanDllPath(c_Tmp);
   c_Tmp = orc_Ini.ReadString("Settings", "CanDllType", "").c_str();
   orc_UserSettings.SetCanDllType(C_UsFiler::mh_GetDllTypeFromString(c_Tmp));

   // Values from Properties widget
   s32_Value = orc_Ini.ReadInteger("Properties", "NodeId", 0);
   orc_UserSettings.SetPropNodeId(s32_Value);
   s32_Value = orc_Ini.ReadInteger("Properties", "Bitrate", 0);
   orc_UserSettings.SetPropBitrate(s32_Value);

   // Values from Node Configuration Dialog
   s32_Value = orc_Ini.ReadInteger("Properties", "InterfaceIndex", 1);
   orc_UserSettings.SetNodeCfgInterfaceIndex(s32_Value);

   // Advanced Properties values
   s32_Value = orc_Ini.ReadInteger("Advanced_Properties", "FlashloaderResetWaitTime", 1000);
   orc_UserSettings.SetFlashloaderResetWaitTime(s32_Value);
   s32_Value = orc_Ini.ReadInteger("Advanced_Properties", "RequestDownloadTimeout", 40000);
   orc_UserSettings.SetRequestDownloadTimeout(s32_Value);
   s32_Value = orc_Ini.ReadInteger("Advanced_Properties", "TransferDataTimeout", 1000);
   orc_UserSettings.SetTransferDataTimeout(s32_Value);

   // Values from Update widget
   s32_Value = orc_Ini.ReadInteger("Update", "HexFileCount", 0);
   for (int32_t s32_SectionCounter = 0; s32_SectionCounter < s32_Value; ++s32_SectionCounter)
   {
      c_HexFilePaths.append(
         orc_Ini.ReadString("Update", "HexFiles[" + C_SclString::IntToStr(s32_SectionCounter) + "]", "").c_str());
      c_HexFilePathsAsRelativeOrAbsolute.append(
         orc_Ini.ReadString("Update", "HexFilesAsRelativeOrAbsolute[" + C_SclString::IntToStr(s32_SectionCounter) + "]",
                            "").c_str());
   }
   orc_UserSettings.SetLastKnownUpdateHexFilePaths(c_HexFilePaths);
   orc_UserSettings.SetHexFilePathsAsRelativeOrAbsolute(c_HexFilePathsAsRelativeOrAbsolute);

   c_Tmp = orc_Ini.ReadString("Update", "LastKnownHexFileLocation", "").c_str();
   orc_UserSettings.SetLastKnownUpdateHexFileLocation(c_Tmp);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get DLL type from string

   \param[in]  orc_Value   Value

   \return
   DLL type from string
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_CanDllType C_UsFiler::mh_GetDllTypeFromString(const QString & orc_Value)
{
   C_UsHandler::E_CanDllType e_Retval;
   if (orc_Value == "peak")
   {
      e_Retval = C_UsHandler::ePEAK;
   }
   else if (orc_Value == "vector")
   {
      e_Retval = C_UsHandler::eVECTOR;
   }
   else if (orc_Value == "other")
   {
      e_Retval = C_UsHandler::eOTHER;
   }
   else
   {
      e_Retval = C_UsHandler::ePEAK;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get string from DLL type

   \param[in]  oe_Value    Value

   \return
   String from DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsFiler::mh_GetStringFromDllType(const C_UsHandler::E_CanDllType oe_Value)
{
   QString c_Retval;

   switch (oe_Value)
   {
   case C_UsHandler::ePEAK:
      c_Retval = "peak";
      break;
   case C_UsHandler::eVECTOR:
      c_Retval = "vector";
      break;
   case C_UsHandler::eOTHER:
      c_Retval = "other";
      break;
   default:
      c_Retval = "";
      break;
   }
   return c_Retval;
}

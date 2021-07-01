//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       OpenSYDE utility functions (implementation)

   OpenSYDE utility functions

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <sstream>
#include <limits>

#include <QApplication>
#include <QFileInfo>
#include <QStorageInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_Uti.h"
#include "TGLTime.h"
#include "C_OSCUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCBinaryHash.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Ensure vector contains each element only once

   Sort vector ascending by value and remove duplicate entries.

   \param[in]  orc_Indices    Input to uniqueify
*/
//----------------------------------------------------------------------------------------------------------------------
void C_Uti::h_Uniqueify(std::vector<uint32> & orc_Indices)
{
   std::vector<uint32>::const_iterator c_Last;
   std::sort(orc_Indices.begin(), orc_Indices.end());

   c_Last = std::unique(orc_Indices.begin(), orc_Indices.end());
   orc_Indices.erase(c_Last, orc_Indices.end());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Split input into contiguous sections

   Also: sorted ascending

   \param[in]  orc_Indices    Indices

   \return
   Input as contiguous sections
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<uint32> > C_Uti::h_GetContiguousSectionsAscending(const std::vector<uint32> & orc_Indices)
{
   std::vector<std::vector<uint32> > c_Retval;
   std::vector<uint32> c_InProgress;
   std::vector<uint32> c_Copy = orc_Indices;
   C_Uti::h_Uniqueify(c_Copy);
   for (uint32 u32_It = 0UL; u32_It < c_Copy.size(); ++u32_It)
   {
      if (c_InProgress.size() > 0UL)
      {
         if (c_Copy[u32_It] ==
             (c_InProgress[static_cast<std::vector<uint32>::size_type>(c_InProgress.size() - 1UL)] + 1UL))
         {
            //Contiguous
            c_InProgress.push_back(c_Copy[u32_It]);
         }
         else
         {
            //Not contiguous
            c_Retval.push_back(c_InProgress);
            c_InProgress.clear();
            c_InProgress.push_back(c_Copy[u32_It]);
         }
      }
      else
      {
         c_InProgress.push_back(c_Copy[u32_It]);
      }
   }
   //Append currently processed one (last section)
   if (c_InProgress.size() > 0UL)
   {
      c_Retval.push_back(c_InProgress);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if floating point value does contain a fraction part

   \param[in]  of64_Value  Value to check

   \return
   True  Value does not have fraction part
   False Fraction part exists
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_Uti::h_CheckFloatHasNoFractionPart(const float64 of64_Value)
{
   bool q_Retval;

   if (C_Uti::h_GetNumberOfDecimals(of64_Value) == 0)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of decimals for floating point value

   \param[in]  of64_Value  Value to check

   \return
   -1   Decimals could not be determined
   Else Detected number of decimals (Restricted to maximum msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_Uti::h_GetNumberOfDecimals(const float64 of64_Value)
{
   sint32 s32_Retval = -1;
   const QString c_String = h_GetStringFromDouble(of64_Value);

   //Guaranteed
   if (c_String.contains(".") == true)
   {
      //Split into integer and decimals
      const QStringList c_List = c_String.split(".");
      if (c_List.size() == 2)
      {
         sint32 s32_ItFromEnd;
         const QString & rc_Str = c_List.at(1);
         //Start from end and iterate over all zeros until first valid number
         for (s32_ItFromEnd = rc_Str.size(); s32_ItFromEnd > 0; --s32_ItFromEnd)
         {
            if (rc_Str.at(static_cast<sintn>(s32_ItFromEnd - 1L)) != QChar('0'))
            {
               break;
            }
         }
         s32_Retval = s32_ItFromEnd;
      }
      else
      {
         s32_Retval = -1;
      }
   }
   else
   {
      s32_Retval = 0;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get a string from a double with reasonable decimals.

   Cut annoying trailing zeros but show many leading zeros if necessary.
   Maximum number of decimals is msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT.
   Examples for strings:
   2.0
   11.1234567898765
   3333333333333333
   0.000000000000456677

   not: 1.9000 or 33333333333333333333333333333333333.0

   \param[in]  of64_Value  Value

   \return
   Obtained QString.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetStringFromDouble(const float64 of64_Value)
{
   QString c_StringFromValue;
   QStringList c_SplitString;

   sintn sn_Precision = std::numeric_limits<float64>::digits10 - 1;
   // -1 because of:
   // at maximum precision the last decimal digit may be rounded because of first meaningless digit
   // this leads to cases like 2.00000000000001 which we do not want
   sintn sn_DigitsWithoutDecimals;
   sintn sn_Index;

   // first get a string with very much decimals
   c_StringFromValue = QString::number(of64_Value, 'f', msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT);

   // split string at decimal point
   c_SplitString = c_StringFromValue.split(".");

   // precaution
   if (c_SplitString.size() == 2)
   {
      // count digits left of decimal point
      sn_DigitsWithoutDecimals = c_SplitString.at(0).length();

      // more than one or one that is not zero --> substract from decimal precision
      if ((sn_DigitsWithoutDecimals > 0) && (c_SplitString.at(0) != "0"))
      {
         sn_Precision -= sn_DigitsWithoutDecimals;
      }
      // one that is zero --> count zeros right of decimal point and add to decimal precision
      else if (c_SplitString.at(0) == "0")
      {
         // example:
         // 0,0000321 --> 0000321 --> 321 --> 3 --> index of 3 in 0000321 is 4 and this is exactly what should be added
         const QString c_HelpString = c_SplitString.at(1);
         sn_Index = c_HelpString.length() - 1;
         for (sintn sn_Pos = 0; sn_Pos < c_HelpString.length(); sn_Pos++)
         {
            if (c_HelpString.at(sn_Pos) != '0')
            {
               sn_Index = sn_Pos;
               break;
            }
         }
         sn_Precision += sn_Index;
      }
      else
      {
         // nothing to do
      }

      // do not allow precisions less than 1
      if (sn_Precision < 1)
      {
         sn_Precision = 1;
      }

      // take the double and convert it again with the calculated precision
      c_StringFromValue = QString::number(of64_Value, 'f', sn_Precision);

      // remove trailing zeros
      sn_Index = c_StringFromValue.length() - 1;
      for (sintn sn_Pos = c_StringFromValue.length() - 1; sn_Pos > 0; sn_Pos--)
      {
         if (c_StringFromValue.at(sn_Pos) != '0')
         {
            sn_Index = sn_Pos;
            break;
         }
      }
      c_StringFromValue.truncate(sn_Index + 1);

      // replace cases like "1." with "1.0"
      if (c_StringFromValue.endsWith("."))
      {
         c_StringFromValue.append("0");
      }
   }

   return c_StringFromValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Uniquify & sort ascending

   Sort vector ascending by value and remove duplicate entries.

   \param[in]  orc_Items   Items

   \return
   Items uniquified and sorted ascending
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_Uti::h_UniquifyAndSortAscending(const std::vector<uint32> & orc_Items)
{
   std::vector<uint32> c_Retval = orc_Items;
   h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Uniquify & sort descending

   Sort vector ascending by value and remove duplicate entries.

   \param[in]  orc_Items   Items

   \return
   Items uniquified and sorted descending
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_Uti::h_UniquifyAndSortDescending(const std::vector<uint32> & orc_Items)
{
   std::vector<uint32> c_Retval;
   std::vector<uint32> c_Ascending = h_UniquifyAndSortAscending(orc_Items);

   //Reverse order
   c_Retval.reserve(c_Ascending.size());
   for (uint32 u32_It = c_Ascending.size(); u32_It > 0UL; --u32_It)
   {
      c_Retval.push_back(c_Ascending[static_cast<uintn>(u32_It - 1UL)]);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scales the color

   The functions adapts the color by calculating the percentage of the difference to the maximum 255
   of all three colors.

   \param[in]  orc_Color         Reference color
   \param[in]  ou32_Percentage   Percentage value

   \return
   Adapted color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_Uti::h_ScaleColor(const QColor & orc_Color, const uint32 ou32_Percentage)
{
   QColor c_AdaptedColor;
   sintn sn_DeltaColor;
   const sintn sn_MAX_VALUE_COLOR = 255;

   if (ou32_Percentage != 100U)
   {
      // the difference for red
      sn_DeltaColor = sn_MAX_VALUE_COLOR - orc_Color.red();
      // the relative difference by the percentage
      if (ou32_Percentage > 0)
      {
         sn_DeltaColor = (sn_DeltaColor * 100) / (10000 / static_cast<sintn>(ou32_Percentage));
      }
      else
      {
         sn_DeltaColor = 0;
      }
      c_AdaptedColor.setRed(sn_MAX_VALUE_COLOR - sn_DeltaColor);

      // the difference for green
      sn_DeltaColor = sn_MAX_VALUE_COLOR - orc_Color.green();
      // the relative difference by the percentage
      if (ou32_Percentage > 0)
      {
         sn_DeltaColor = (sn_DeltaColor * 100) / (10000 / static_cast<sintn>(ou32_Percentage));
      }
      else
      {
         sn_DeltaColor = 0;
      }
      c_AdaptedColor.setGreen(sn_MAX_VALUE_COLOR - sn_DeltaColor);

      // the difference for blue
      sn_DeltaColor = sn_MAX_VALUE_COLOR - orc_Color.blue();
      // the relative difference by the percentage
      if (ou32_Percentage > 0)
      {
         sn_DeltaColor = (sn_DeltaColor * 100) / (10000 / static_cast<sintn>(ou32_Percentage));
      }
      else
      {
         sn_DeltaColor = 0;
      }
      c_AdaptedColor.setBlue(sn_MAX_VALUE_COLOR - sn_DeltaColor);
   }
   else
   {
      // no change of the color
      c_AdaptedColor = orc_Color;
   }

   return c_AdaptedColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to calculate disabled look for current color

   \param[in]  orc_DefaultColorLook    Default color look

   \return
   Disabled look for orc_DefaultColorLook
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_Uti::h_GetDisabledColorLook(const QColor & orc_DefaultColorLook)
{
   const sintn sn_Red = orc_DefaultColorLook.red() + ((255 - orc_DefaultColorLook.red()) / 2);
   const sintn sn_Green = orc_DefaultColorLook.green() + ((255 - orc_DefaultColorLook.green()) / 2);
   const sintn sn_Blue = orc_DefaultColorLook.blue() + ((255 - orc_DefaultColorLook.blue()) / 2);
   const QColor c_Retval(sn_Red, sn_Green, sn_Blue, orc_DefaultColorLook.alpha());

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the string with three dots at the end if not enough space is available

   \param[in]  orc_String        Input string
   \param[in]  orc_FontMetrics   Used font configuration
   \param[in]  osn_Width         Available space

   \return
   Adapted string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                   const sintn osn_Width)
{
   return orc_FontMetrics.elidedText(orc_String, Qt::ElideRight, osn_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the string with three dots at the end if not enough space is available

   \param[in]  orc_String        Input string
   \param[in]  orc_FontMetrics   Used font configuration
   \param[in]  of64_Width        Available space

   \return
   Adapted string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                   const float64 of64_Width)
{
   return C_Uti::h_AdaptStringToSize(orc_String, orc_FontMetrics, static_cast<sintn>(of64_Width));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare byte count as string

   \param[in]  ou32_ByteCount    Byte count

   \return
   String: 1 Byte or ou32_ByteCount Bytes
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetByteCountAsString(const stw_types::uint32 ou32_ByteCount)
{
   QString c_Text = QString::number(ou32_ByteCount);

   if (ou32_ByteCount == 1)
   {
      c_Text += static_cast<QString>(" Byte");
   }
   else
   {
      c_Text += static_cast<QString>(" Bytes");
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for specific key modifier

   \param[in]  orc_ActiveKeyModifiers  Active key modifiers
   \param[in]  ore_CheckKeyModifier    Key modifier to check for

   \return
   true  Modifier active
   false Modifier inactive
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_Uti::h_CheckKeyModifier(const Qt::KeyboardModifiers & orc_ActiveKeyModifiers,
                               const Qt::KeyboardModifier & ore_CheckKeyModifier)
{
   return (orc_ActiveKeyModifiers.testFlag(ore_CheckKeyModifier) == true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check for specific style state modifier

   \param[in]  orc_ActiveState   Active states
   \param[in]  ore_CheckState    State to check for

   \return
   true  State active
   false State inactive
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_Uti::h_CheckStyleState(const QStyle::State & orc_ActiveState, const QStyle::StateFlag & ore_CheckState)
{
   return (orc_ActiveState.testFlag(ore_CheckState) == true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current executable path (No slash at end)

   \return
   Current executable path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetExePath(void)
{
   return QCoreApplication::applicationDirPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get resource version number of a file as an QString

   Extracts the windows version number of the specified file and returns it
    in the commonly used STW format: "Vx.yyrz".
   This function is Windows specific and needs to be replaced by another solution
    when porting to a non-Windows system

   \param[in]  oq_UseSTWFormat   Optional flag to output STW format

   \return
   string with version information ("V?.??r?" on error)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetApplicationVersion(const bool oq_UseSTWFormat)
{
   const QFileInfo c_FileInfo(QApplication::applicationFilePath());
   const QString c_FileName = c_FileInfo.fileName();
   VS_FIXEDFILEINFO * pt_Info;
   uintn un_ValSize;
   sint32 s32_InfoSize;
   C_SCLString c_Version;

   c_Version = "V?.\?\?r?";

   s32_InfoSize = GetFileVersionInfoSizeA(c_FileName.toStdString().c_str(), NULL);
   if (s32_InfoSize != 0)
   {
      uint8 * pu8_Buffer;
      pu8_Buffer = new uint8[static_cast<uintn>(s32_InfoSize)];
      if (GetFileVersionInfoA(c_FileName.toStdString().c_str(), 0, s32_InfoSize, pu8_Buffer) != FALSE)
      {
         //reinterpret_cast required due to function interface
         if (VerQueryValueA(pu8_Buffer, "\\",
                            reinterpret_cast<PVOID *>(&pt_Info), //lint !e929 !e9176
                            &un_ValSize) != FALSE)
         {
            if (oq_UseSTWFormat)
            {
               c_Version.PrintFormatted("V%d.%02dr%d", (pt_Info->dwFileVersionMS >> 16U),
                                        pt_Info->dwFileVersionMS & 0x0000FFFFUL,
                                        (pt_Info->dwFileVersionLS >> 16U));
            }
            else
            {
               c_Version.PrintFormatted("%d.%02d.%d", (pt_Info->dwFileVersionMS >> 16U),
                                        pt_Info->dwFileVersionMS & 0x0000FFFFUL,
                                        (pt_Info->dwFileVersionLS >> 16U));
            }
         }
      }
      delete[] pu8_Buffer;
   }
   return c_Version.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check default path and replace by exe path if necessary

   \param[in]  orc_Path    Default path to choose if set

   \return
   Always contains some path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_CheckAndReplaceWithExePathIfNecessary(const QString & orc_Path)
{
   QString c_Retval;

   if (orc_Path.compare("") == 0)
   {
      c_Retval = C_Uti::h_GetExePath();
   }
   else
   {
      c_Retval = orc_Path;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert text to styled link

   There are problems when using this function together with "\n" as new line symbol, especially in text browsers.
   Just use "<br>" instead to make it work fine.

   \param[in]  orc_DisplayedText    Text displayed by link
   \param[in]  orc_Color            Link color
   \param[in]  orc_HyperLink        Hyperlink target

   \return
   Styled link text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetLink(const QString & orc_DisplayedText, const QColor & orc_Color, const QString & orc_HyperLink)
{
   const QString c_Retval = static_cast<QString>(
      "<a href=\"%5\">"
      "<span style=\"color: rgba(%1,%2,%3,%4);\">" + orc_DisplayedText +
      "</span></a>").arg(
      QString::number(orc_Color.red()), QString::number(orc_Color.green()),
      QString::number(orc_Color.blue()), QString::number(orc_Color.alpha()), orc_HyperLink);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get absolute log file path and location

   Warning: only for onetime usage

   \param[in]  orc_Extension  Extension to use for log file

   \return
   Absolute log file path and location
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetCompleteLogFileLocation(const QString & orc_Extension)
{
   QString c_Retval;
   //Set up logging (FIRST)
   C_TGLDateTime c_DateTime;

   TGL_GetDateTimeNow(c_DateTime);
   //Format:2017-08-29 07:32:19.123
   QString c_FileBaseName = C_OSCLoggingHandler::h_UtilConvertDateTimeToString(
      c_DateTime).c_str();

   //Replace invalid characters
   c_FileBaseName = c_FileBaseName.replace(4, 1, '_');
   c_FileBaseName = c_FileBaseName.replace(7, 1, '_');
   c_FileBaseName = c_FileBaseName.replace(13, 1, '_');
   c_FileBaseName = c_FileBaseName.replace(16, 1, '_');
   //Add file ending
   c_FileBaseName = c_FileBaseName.replace(19, 4, orc_Extension.toStdString().c_str());
   //Final step as this step changes the format size
   c_FileBaseName = c_FileBaseName.replace(10, 1, "__");

   c_Retval = static_cast<QString>("%1/Logs/%2").arg(C_Uti::h_GetExePath()).arg(c_FileBaseName);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get readable stw error

   \param[in]  os32_Error  STW error value

   \return
   STW error string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_StwError(const sint32 os32_Error)
{
   const QString c_Retval = C_OSCLoggingHandler::h_StwError(os32_Error).c_str();

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Minimize full paths to fit in GUI edit fields.

   The given width of an edit field for example is optimally used, so that a user can
   expect a full file path. The drive name, the file name and as many subfolders as possible
   can be seen for the given width in points. The number of characters seen is dependent of
   the used font type and size. The subfolders are recursively tried to fit for the given width
   from the end of the path and completely showed:

   E.g.: C:/.../MyFolder/MySubFolder/MyFile.txt

   In case the given width is even to small for the drive and the file name,
   the full file path is hard cut with an ellipsis in the middle:

   E.g.: C:/LoooooooooooonnnnnnggggggggNnnnn…aaaaaaaaaaaaaaammmmmmmmeeeeeeee.txt

   Assumptions:
   * a valid full file path must be given to work correctly.

   Hint:
   * this function is not expecting network paths.
   * do not convert the result of this function to std::string because the ellipsis "..."
     has an Unicode character representation (U+2026).

   \param[in]  orc_Path       full file path
   \param[in]  orc_Font       font to calculate number of maximal possible characters
   \param[in]  ou32_Width     size in pixels for minimized file path to fit e.g. in line edit field
   \param[in]  ou32_Buffer    left and right margin for text e.g. in line edit field, if this parameter is not needed
                              then set value to 0.

   \return
   minimized file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_MinimizePath(const QString & orc_Path, const QFont & orc_Font, const uint32 ou32_Width,
                              const uint32 ou32_Buffer)
{
   QString c_MinimizedPath = orc_Path; // result

   if (orc_Path != "")
   {
      const QFileInfo c_FileInfo(orc_Path);

      const QFontMetrics c_FontMetrics(orc_Font);
      const QString c_Path = c_FileInfo.filePath(); // in case we have windows-style path
      // get drive name
      const QStorageInfo c_StorageInfo(c_FileInfo.absoluteDir());
      QString c_DriveName = c_StorageInfo.rootPath();

      // try to extract drive name from string in case of not existing files or relative path
      if ((c_DriveName == "") ||             // not existing path: drive name is empty
          (c_FileInfo.isRelative() == true)) // relative path: use characters before first "/" (e.g. "./" or "Dir")
      {
         const QStringList c_Temp = c_Path.split("/"); // c_Path is Qt style path, i.e. uses "/" as separators
         if (c_Temp.size() > 1)                        // if no "/" occurs in path, split returns a list containing one
                                                       // string
         {
            c_DriveName = c_Temp[0] + "/";
         }
      }

      // get file name
      const QString c_FileName = c_FileInfo.fileName();

      const QString c_Placeholder = static_cast<QString>(static_cast<QChar>(0x2026)) + "/"; // with horizontal ellipsis
                                                                                            // "..."

      sintn sn_CurrentWidth;
      uint32 u32_AvailableWidth; // width excluding text margins

      if (ou32_Width > ou32_Buffer)
      {
         u32_AvailableWidth = ou32_Width - ou32_Buffer;
      }
      else
      {
         u32_AvailableWidth = 0;
      }

      // does the full path fit?
      c_MinimizedPath = c_Path;
      sn_CurrentWidth = c_FontMetrics.horizontalAdvance(c_MinimizedPath);
      if (sn_CurrentWidth > static_cast<sintn>(u32_AvailableWidth))
      {
         // no, full path does not fit

         // does the drive and file name alone with placeholder string fit?
         // check length of drive and file name plus placeholder
         c_MinimizedPath = c_DriveName + c_Placeholder + c_FileName;
         sn_CurrentWidth = c_FontMetrics.horizontalAdvance(c_MinimizedPath);
         if (sn_CurrentWidth < static_cast<sintn>(u32_AvailableWidth))
         {
            // yes, try to reduce folders recursively
            c_MinimizedPath = c_Path;
            sintn sn_DelimPos = c_MinimizedPath.indexOf("/", 0); // first delimiter
            do
            {
               // get next folder
               sn_DelimPos = c_MinimizedPath.indexOf("/", sn_DelimPos + 1); // next delimiter

               c_MinimizedPath = c_DriveName + c_Placeholder +
                                 c_MinimizedPath.right((c_MinimizedPath.length() - sn_DelimPos) - 1);

               sn_DelimPos = static_cast<QString>(c_DriveName + c_Placeholder).length();
               sn_CurrentWidth = c_FontMetrics.horizontalAdvance(c_MinimizedPath);
            }
            // second condition should not occur and is only defensive
            while ((sn_CurrentWidth > (static_cast<sintn>(u32_AvailableWidth))) || (sn_DelimPos == -1));
         }
         else
         {
            // drive and filename does not fit for minimizing therefore we do a hard cut of the full path
            c_MinimizedPath = c_FontMetrics.elidedText(c_Path, Qt::ElideMiddle,
                                                       static_cast<sintn>(u32_AvailableWidth),
                                                       static_cast<sintn>(Qt::TextSingleLine));
         }
      }
   }

   return c_MinimizedPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Converts an IP address to readable string

   4 byte array to x.x.x.x

   \param[in]  orau8_IpAddress   IP address for conversion

   \return
   String with IP address
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_IpAddressToString(const uint8 (&orau8_IpAddress)[4])
{
   QString c_Result;

   c_Result = static_cast<QString>("%1.%2.%3.%4").
              arg(QString::number(orau8_IpAddress[0])).
              arg(QString::number(orau8_IpAddress[1])).
              arg(QString::number(orau8_IpAddress[2])).
              arg(QString::number(orau8_IpAddress[3]));

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert font from point size into pixel size

   \param[in]  orc_Font    Font to convert

   \return
   Font with previous point size as new pixel size
*/
//----------------------------------------------------------------------------------------------------------------------
QFont C_Uti::h_GetFontPixel(const QFont & orc_Font)
{
   QFont c_Retval = orc_Font;

   c_Retval.setPixelSize(c_Retval.pointSize());
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert version in format 1.00.0 to STW format V1.00r0

   \param[in]  orc_Version    version in format 1.00.0 (e.g. APPLICATION_VERSION)

   \return
   version
      in format V1.00r0 if orc_Version is in format 1.00.0
      else orc_Version as it is
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_ConvertVersionToSTWStyle(const QString & orc_Version)
{
   QString c_Return = orc_Version;
   QStringList c_Temp;

   c_Temp = c_Return.split(".");

   // APPLICATION_VERSION should be in format 1.00.0, i.e. 3 parts
   if (c_Temp.size() == 3)
   {
      c_Return = "V" + c_Temp.at(0) + "." + c_Temp.at(1) + "r" + c_Temp.at(2);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to convert relative path to absolute path if necessary

   Warning: assuming orc_AbsoluteBaseDir is not an empty string and no file.

   \param[in]  orc_BaseDir                   Base path if relative and could itself be relative
   \param[in]  orc_RelativeOrAbsolutePath    Path which might be relative or absolute (and could be empty)

   \return
   Absolute file path if input fulfills assumptions
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_ConcatPathIfNecessary(const QString & orc_BaseDir, const QString & orc_RelativeOrAbsolutePath)
{
   QString c_Retval;

   if ((orc_BaseDir.isEmpty() == false) &&
       (QDir::isAbsolutePath(orc_RelativeOrAbsolutePath) == false) &&
       (static_cast<QFileInfo>(orc_BaseDir).isFile() == false))
   {
      c_Retval = orc_BaseDir + "/" + orc_RelativeOrAbsolutePath;

      // do some path beautifying
      if (c_Retval.contains("%") == false)
      {
         // clean path only if there are no path variables
         // (else %{BLUB}/../folder would be cleaned to folder which is wrong)
         c_Retval = QDir::cleanPath(c_Retval);
      }
   }
   else
   {
      c_Retval = orc_RelativeOrAbsolutePath;
   }

   // remove all double slashes but the first (network paths)
   if (c_Retval.startsWith("//"))
   {
      c_Retval = '/' + c_Retval;
   }
   c_Retval.replace("//", "/");

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unique item name based on proposal

   \param[in]  orc_ExistingStrings  Existing item names
   \param[in]  orc_ProposedName     Proposal for item name
   \param[in]  orc_SkipName         Optional name to block any adaptations for

   \return
   Unique node name
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_Uti::h_GetUniqueName(const std::map<C_SCLString, bool> & orc_ExistingStrings,
                                            const stw_scl::C_SCLString & orc_ProposedName,
                                            const stw_scl::C_SCLString & orc_SkipName)
{
   C_SCLString c_Retval = orc_ProposedName;
   bool q_Conflict;
   sint32 s32_MaxDeviation;
   C_SCLString c_BaseStr;

   std::map<C_SCLString, bool>::const_iterator c_ItString;

   do
   {
      q_Conflict = false;
      c_ItString = orc_ExistingStrings.find(c_Retval);
      if (c_ItString != orc_ExistingStrings.end())
      {
         const C_SCLString & rc_ConflictingValue = c_ItString->first;
         //Search for the SkipName if the skip name is a valid string
         const uint32 u32_Pos = orc_SkipName.IsEmpty() ? 0UL : rc_ConflictingValue.LastPos(orc_SkipName);
         q_Conflict = true;
         if (u32_Pos == 0UL)
         {
            //Continue examining the complete string
            h_GetNumberAtStringEnd(rc_ConflictingValue, c_BaseStr, s32_MaxDeviation);
         }
         else
         {
            //Hint: SubString and LastPos start counting at 1
            const uint32 u32_ZeroBasedpos = u32_Pos - 1UL;
            //Extract the part of the string that may be adapted
            const C_SCLString c_StringAfterSkip = rc_ConflictingValue.SubString(
               (u32_ZeroBasedpos + orc_SkipName.Length()) + 1UL,
               (rc_ConflictingValue.Length() - orc_SkipName.Length()) - u32_ZeroBasedpos);
            const C_SCLString c_SkippedPart =
               rc_ConflictingValue.SubString(1UL, u32_ZeroBasedpos + orc_SkipName.Length());
            //Search remaining part for any number
            h_GetNumberAtStringEnd(c_StringAfterSkip, c_BaseStr, s32_MaxDeviation);
            //Add skipped part to base string again
            c_BaseStr = c_SkippedPart + c_BaseStr;
         }
         //Do not use 0 and 1 for name adaptation
         if (s32_MaxDeviation <= 0)
         {
            s32_MaxDeviation = 1;
         }
         c_Retval = c_BaseStr + '_' + C_SCLString::IntToStr(s32_MaxDeviation + static_cast<sint32>(1));
      }
   }
   while (q_Conflict == true);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get unique item name based on proposal

   \param[in]  orc_ExistingStrings  Existing item names
   \param[in]  orc_ProposedName     Proposal for item name

   \return
   Unique node name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetUniqueNameQ(const std::map<C_SCLString, bool> & orc_ExistingStrings,
                                const QString & orc_ProposedName)
{
   const C_SCLString c_Result = C_Uti::h_GetUniqueName(orc_ExistingStrings, orc_ProposedName.toStdString().c_str());

   return c_Result.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number at string end

   \param[in]   orc_ProposedName    Proposal for name
   \param[out]  orc_CutString       String without number
   \param[out]  ors32_Number        Number at end (else -1)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_Uti::h_GetNumberAtStringEnd(const C_SCLString & orc_ProposedName, C_SCLString & orc_CutString,
                                   sint32 & ors32_Number)
{
   C_SCLString c_Number;
   bool q_UnderscoreDetected = false;
   bool q_NumberDetected = false;
   uint32 u32_ItStr;

   //Default return
   orc_CutString = orc_ProposedName;
   ors32_Number = -1;

   for (u32_ItStr = orc_ProposedName.Length(); u32_ItStr > 0; --u32_ItStr)
   {
      if ((orc_ProposedName[u32_ItStr] >= '0') && (orc_ProposedName[u32_ItStr] <= '9'))
      {
         //Continue
         q_NumberDetected = true;
      }
      else
      {
         if ('_' == orc_ProposedName[u32_ItStr])
         {
            q_UnderscoreDetected = true;
         }
         //Stop
         break;
      }
   }
   if ((q_NumberDetected == true) && (q_UnderscoreDetected == true))
   {
      //Cut string
      orc_CutString = orc_ProposedName.SubString(1, u32_ItStr - 1);                    //Without underscore
      c_Number = orc_ProposedName.SubString(u32_ItStr + 1, orc_ProposedName.Length()); //Without underscore
      try
      {
         ors32_Number = c_Number.ToInt();
      }
      catch (...)
      {
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create ascending sorted index map for input vector

   Another attempt to explain this:
   this class actually looks at the value stored in the vector
   and creates and output vector with the length of that value
   and then stores the index at the index of that value
   e.g. first element is 7
   -> resize output to seven elements (all initialized with minus one)
   -> set the 7. output element to be 0 (we said this is the first found element)
   -> and so on

   Once the function is done you can iterate over the output
   and the first non negative value you find is the index of the "lowest" value element
   Also as you might notice this will directly make the output unique

   Pre-requirement:
   The input vector should be an assortment of unique indices.
   Purpose:
   The output vector:
   -1:        index was not part of input vector
   otherwise: index was part of input vector and the value shows at which position this index was standing

   \param[in]  orc_UnsortedIndices  Unsorted indices

   \return
   Ascending sorted index map
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_Uti::h_CreateAscendingIndexMap(const std::vector<uint32> & orc_UnsortedIndices)
{
   std::vector<sint32> c_IndexMap;
   c_IndexMap.resize(orc_UnsortedIndices.size(), -1);
   for (uint32 u32_Index = 0; u32_Index < orc_UnsortedIndices.size(); ++u32_Index)
   {
      if (orc_UnsortedIndices[u32_Index] >= c_IndexMap.size())
      {
         const uint32 u32_NewSize = orc_UnsortedIndices[u32_Index] + 1U;
         c_IndexMap.resize(u32_NewSize, -1);
      }
      c_IndexMap[orc_UnsortedIndices[u32_Index]] = u32_Index;
   }
   return c_IndexMap;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if input vector is sorted ascending

   \param[in]  orc_Indices    Input vector to evaluate

   \return
   true:  Sorted
   false: Unsorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_Uti::h_CheckSortedAscending(const std::vector<uint32> & orc_Indices)
{
   bool q_Retval = true;

   if (orc_Indices.size() > 1)
   {
      uint32 u32_PrevVal = orc_Indices[0];

      for (uint32 u32_It = 1; u32_It < orc_Indices.size(); ++u32_It)
      {
         if (u32_PrevVal <= orc_Indices[u32_It])
         {
            u32_PrevVal = orc_Indices[u32_It];
         }
         else
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Always get absolute path from path relative to executable.

   Empty input results in exe path!

   \param[in]  orc_Path    Absolute or relative path

   \return
   Absolute path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetAbsolutePathFromExe(const QString & orc_Path)
{
   const QString c_Folder = C_Uti::h_GetExePath(); // always absolute

   return C_Uti::h_ConcatPathIfNecessary(c_Folder, orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paths after file save dialog.

   Check if path could be made relative.

   Note: If one of the input paths is empty this simply returns the given path and its concatenation with the directory.
   If the reference path is a file path (ending on File.txt), behavior is undefined
   (handling directory AND files AND existing AND non-existing did not work well).
   If the reference path is not absolute, behavior is undefined
   (Qt then defaults to calling path, which is often but not always the path of the executable).

   \param[in]   orc_PathIn                relative or absolute path of file or directory
   \param[in]   orc_AbsoluteReferenceDir  absolute path of reference directory
   \param[out]  orc_PathAbsolute          absolute path
   \param[out]  orc_PathRelative          relative path

   \return
   true  path can be represented relative to directory
   false path can not be represented relative to directory
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_Uti::h_IsPathRelativeToDir(const QString & orc_PathIn, const QString & orc_AbsoluteReferenceDir,
                                  QString & orc_PathAbsolute, QString & orc_PathRelative)
{
   bool oq_Return = false;

   // reset
   orc_PathAbsolute = orc_PathIn;
   orc_PathRelative = orc_PathIn;

   if ((orc_PathIn.isEmpty() == false) && (orc_AbsoluteReferenceDir.isEmpty() == false))
   {
      const QDir c_Dir(orc_AbsoluteReferenceDir);
      QFileInfo c_PathInfo;

      // check if path can be represented relative to given directory
      c_PathInfo.setFile(c_Dir.relativeFilePath(orc_PathIn));

      oq_Return = c_PathInfo.isRelative();
      if (oq_Return == true)
      {
         orc_PathRelative = c_PathInfo.filePath();
         orc_PathAbsolute = C_Uti::h_ConcatPathIfNecessary(orc_AbsoluteReferenceDir, orc_PathIn);
      }
   }

   return oq_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if path contains project independent variables (e.g. %{OPENSYDE_BINARY}) and resolve them.

   Do not call this function for replacing every path variable!
   This functionality can be found in a utility class that knows project stuff.

   \param[in]  orc_Path    path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_ResolveProjIndependentPlaceholderVariables(const QString & orc_Path)
{
   QString c_Return = orc_Path;
   C_SCLString c_Help;

   if (c_Return.contains(mc_PATH_VARIABLE_OPENSYDE_BIN) == true)
   {
      c_Return.replace(mc_PATH_VARIABLE_OPENSYDE_BIN, C_Uti::h_GetExePath() + '/');
   }

   if (c_Return.contains(mc_PATH_VARIABLE_USER_NAME) == true)
   {
      tgl_assert(TGL_GetSystemUserName(c_Help));
      c_Return.replace(mc_PATH_VARIABLE_USER_NAME, c_Help.c_str());
   }

   if (c_Return.contains(mc_PATH_VARIABLE_COMPUTER_NAME) == true)
   {
      // find out computer name is analogue to find out user name, but TGLUtils do not offer this
      QString c_UserName;
      charn acn_WinUserName[255];
      uint32 u32_Size = sizeof(acn_WinUserName);
      const bool q_Return = (GetComputerNameA(acn_WinUserName, &u32_Size) == 0) ? false : true;
      if (q_Return == true)
      {
         c_UserName = acn_WinUserName;
      }
      else
      {
         c_UserName = "?\?\?\?\?";
      }

      c_Return.replace(mc_PATH_VARIABLE_COMPUTER_NAME, c_UserName);
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Casting the MD5-Checksum from C_SCLString to QString

   \return  MD5-Checksum
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetHashValueAsQString(void)
{
   return QString::fromStdString(stw_opensyde_core::C_OSCBinaryHash::h_CreateBinaryHash().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get value as hex

   \param[in]  ou64_Value        Value
   \param[in]  ou8_FieldWidth    Field width

   \return
   Hexadecimal representation of value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetValueAsHex(const uint64 ou64_Value, const uint8 ou8_FieldWidth)
{
   return "0x" + static_cast<QString>("%1").arg(ou64_Value, ou8_FieldWidth, 16, QChar('0')).toUpper();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get value as hex

   \param[in]  ou32_Value        Value
   \param[in]  ou8_FieldWidth    Field width

   \return
   Hexadecimal representation of value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_Uti::h_GetValueAsHex(const uint32 ou32_Value, const uint8 ou8_FieldWidth)
{
   return C_Uti::h_GetValueAsHex(static_cast<uint64>(ou32_Value), ou8_FieldWidth);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Parses a specific folder for files

   All files will be added to orc_FilePaths and all found folders will be checked recursively.
   orc_FilePaths will not be cleared

   \param[in]      orc_FolderPath   Path for searching files and other folders
   \param[in,out]  orc_FilePaths    Found file paths with absolute paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_Uti::h_GetAllFilePathsInFolder(const QString & orc_FolderPath, std::vector<C_SCLString> & orc_FilePaths)
{
   const QDir c_Dir(orc_FolderPath);
   const QStringList c_AllFiles = c_Dir.entryList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
   sintn sn_Counter;

   for (sn_Counter = 0; sn_Counter < c_AllFiles.size(); ++sn_Counter)
   {
      const QString c_Path = orc_FolderPath + c_AllFiles[sn_Counter];
      const QFileInfo c_File(c_Path);

      if (c_File.isDir() == true)
      {
         h_GetAllFilePathsInFolder(c_Path + "/", orc_FilePaths);
      }
      else if (c_File.isFile() == true)
      {
         orc_FilePaths.push_back(c_Path.toStdString().c_str());
      }
      else
      {
         // Nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending ( Sorting steps are done for the content vectors in sync)

   \param[in,out]  orc_IndicesTmp   Unsorted indices
   \param[in,out]  orc_SyncContent  Unsorted content to sync

   \return
   ascending sorted indices
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void C_Uti::h_SortIndicesAscendingAndSync(std::vector<uint32> & orc_IndicesTmp, std::vector<T> & orc_SyncContent)
{
   if (C_Uti::h_CheckSortedAscending(orc_IndicesTmp) == false)
   {
      std::vector<stw_types::uint32> c_IndicesTmp;
      //lint -e{8080} //template naming not correctly handled by naming convention checker
      std::vector<T> c_SyncContentTmp;
      //Step 1: Fill new vector in sorted order with which element should be copied to which position
      const std::vector<stw_types::sint32> c_IndexMap = C_Uti::h_CreateAscendingIndexMap(orc_IndicesTmp);
      //Step 2: Copy existing elements to new structures according to plan
      c_IndicesTmp.reserve(orc_IndicesTmp.size());
      c_SyncContentTmp.reserve(orc_SyncContent.size());
      for (stw_types::uint32 u32_ItIndex = 0; u32_ItIndex < c_IndexMap.size(); ++u32_ItIndex)
      {
         if (c_IndexMap[u32_ItIndex] >= 0)
         {
            const stw_types::uint32 u32_CurIndex = static_cast<stw_types::uint32>(c_IndexMap[u32_ItIndex]);
            if ((u32_CurIndex < orc_IndicesTmp.size()) &&
                (u32_CurIndex < orc_SyncContent.size()))
            {
               c_IndicesTmp.push_back(orc_IndicesTmp[u32_CurIndex]);
               c_SyncContentTmp.push_back(orc_SyncContent[u32_CurIndex]);
            }
         }
      }
      //Step 3: Copy data structures to internal ones
      orc_IndicesTmp = c_IndicesTmp;
      orc_SyncContent = c_SyncContentTmp;
   }
}

//Explicit declaration of every type usage is necessary for templates to allow split of declaration and implementation
//lint -esym(754,stw_opensyde_gui_logic::C_Uti::h_SortIndicesAscendingAndSync*)
template
void C_Uti::h_SortIndicesAscendingAndSync<stw_types::uint32>(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                                             std::vector<uint32> & orc_SyncContent);

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_Uti::C_Uti(void)
{
}

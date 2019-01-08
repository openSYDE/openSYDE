//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       OpenSYDE utility functions (implementation)

   OpenSYDE utility functions

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cmath>
#include <sstream>
#include <limits>

#include <QCoreApplication>
#include <QFileInfo>
#include <QStorageInfo>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_Uti.h"
#include "TGLTime.h"
#include "C_OSCUtils.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Ensure vector contains each element only once

   Sort vector ascending by value and remove duplicate entries.

   \param[in] orc_Indices Input to uniqueify

   \created     13.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_Uti::h_Uniqueify(std::vector<uint32> & orc_Indices)
{
   std::vector<uint32>::const_iterator c_Last;
   //lint -e{864} Call as expected by interface
   std::sort(orc_Indices.begin(), orc_Indices.end());

   //lint -e{864} Call as expected by interface
   c_Last = std::unique(orc_Indices.begin(), orc_Indices.end());
   //lint -e{64,119,1025,1703} Function not recognized
   orc_Indices.erase(c_Last, orc_Indices.end());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if floating point value does contain a fraction part

   \param[in] of64_Value Value to check

   \return
   True  Value does not have fraction part
   False Fraction part exists

   \created     25.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get number of decimals for floating point value

   \param[in] of64_Value Value to check

   \return
   -1   Decimals could not be determined
   Else Detected number of decimals (Restricted to maximum msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT)

   \created     25.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get a string from a double with reasonable decimals.

   Cut annoying trailing zeros but show many leading zeros if necessary.
   Maximum number of decimals is msn_DOUBLE_SPIN_BOX_DECIMAL_COUNT.
   Examples for strings:
   2.0
   11.1234567898765
   3333333333333333
   0.000000000000456677

   not: 1.9000 or 33333333333333333333333333333333333.0

   \return
   Obtained QString.

   \created     08.06.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
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
         QString c_HelpString = c_SplitString.at(1);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Uniquify & sort ascending

   Sort vector ascending by value and remove duplicate entries.

   \param[in] orc_Items Items

   \return
   Items uniquified and sorted ascending

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_Uti::h_UniquifyAndSortAscending(const std::vector<uint32> & orc_Items)
{
   std::vector<uint32> c_Retval = orc_Items;
   h_Uniqueify(c_Retval);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Uniquify & sort descending

   Sort vector ascending by value and remove duplicate entries.

   \param[in] orc_Items Items

   \return
   Items uniquified and sorted descending

   \created     21.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_Uti::h_UniquifyAndSortDescending(const std::vector<uint32> & orc_Items)
{
   std::vector<uint32> c_Retval;
   std::vector<uint32> c_Ascending = h_UniquifyAndSortAscending(orc_Items);

   //Reverse order
   c_Retval.reserve(c_Ascending.size());
   for (uint32 u32_It = c_Ascending.size(); u32_It > 0UL; --u32_It)
   {
      //lint -e{1960} All types as expected
      c_Retval.push_back(c_Ascending[u32_It - 1UL]);
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Scales the color

   The functions adapts the color by calculating the percentage of the difference to the maximum 255
   of all three colors.

   \param[in] orc_Color        Reference color
   \param[in] ou32_Percentage  Percentage value

   \return
   Adapted color

   \created     23.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to calculate disabled look for current color

   \param[in] orc_DefaultColorLook Default color look

   \return
   Disabled look for orc_DefaultColorLook

   \created     28.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QColor C_Uti::h_GetDisabledColorLook(const QColor & orc_DefaultColorLook)
{
   const sintn sn_Red = orc_DefaultColorLook.red() + ((255 - orc_DefaultColorLook.red()) / 2);
   const sintn sn_Green = orc_DefaultColorLook.green() + ((255 - orc_DefaultColorLook.green()) / 2);
   const sintn sn_Blue = orc_DefaultColorLook.blue() + ((255 - orc_DefaultColorLook.blue()) / 2);
   const QColor c_Retval(sn_Red, sn_Green, sn_Blue, orc_DefaultColorLook.alpha());

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the string with three dots at the end if not enough space is available

   \param[in] orc_FontMetrics Used font configuration
   \param[in] osn_Width       Available space

   \return
   Adapted string

   \created     10.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                   const sintn osn_Width)
{
   return orc_FontMetrics.elidedText(orc_String, Qt::ElideRight, osn_Width);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts the string with three dots at the end if not enough space is available

   \param[in] orc_FontMetrics Used font configuration
   \param[in] of64_Width      Available space

   \return
   Adapted string

   \created     10.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                   const float64 of64_Width)
{
   return C_Uti::h_AdaptStringToSize(orc_String, orc_FontMetrics, static_cast<sintn>(of64_Width));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare byte count as string

   \param[in] ou32_ByteCount   Byte count

   \return
   String: 1 Byte or ou32_ByteCount Bytes

   \created     19.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_GetByteCountAsString(const stw_types::uint32 ou32_ByteCount)
{
   QString c_Text = QString::number(ou32_ByteCount);

   if (ou32_ByteCount == 1)
   {
      c_Text += QString(" Byte");
   }
   else
   {
      c_Text += QString(" Bytes");
   }

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check for specific key modifier

   \param[in] orc_ActiveKeyModifiers Active key modifiers
   \param[in] ore_CheckKeyModifier   Key modifier to check for

   \return
   true  Modifier active
   false Modifier inactive

   \created     09.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_Uti::h_CheckKeyModifier(const Qt::KeyboardModifiers & orc_ActiveKeyModifiers,
                               const Qt::KeyboardModifier & ore_CheckKeyModifier)
{
   return (orc_ActiveKeyModifiers.testFlag(ore_CheckKeyModifier) == true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check for specific style state modifier

   \param[in] orc_ActiveState Active states
   \param[in] ore_CheckState  State to check for

   \return
   true  State active
   false State inactive

   \created     09.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_Uti::h_CheckStyleState(const QStyle::State & orc_ActiveState, const QStyle::StateFlag & ore_CheckState)
{
   return (orc_ActiveState.testFlag(ore_CheckState) == true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current executable path (No slash at end)

   \created     16.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_GetExePath(void)
{
   return QCoreApplication::applicationDirPath();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check default path and replace by exe path if necessary

   \param[in] orc_Path Default path to choose if set

   \return
   Always contains some path

   \created     05.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Convert text to styled link

   \param[in] orc_DisplayedText Text displayed by link
   \param[in] orc_Color         Link color
   \param[in] orc_HyperLink     Hyperlink target

   \return
   Styled link text

   \created     23.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_GetLink(const QString & orc_DisplayedText, const QColor & orc_Color, const QString & orc_HyperLink)
{
   const QString c_Retval = QString(
      "<a href=\"%5\">"
      "<span style=\"color: rgba(%1,%2,%3,%4);\">" + orc_DisplayedText +
      "</span></a>").arg(
      QString::number(orc_Color.red()), QString::number(orc_Color.green()),
      QString::number(orc_Color.blue()), QString::number(orc_Color.alpha()), orc_HyperLink);

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get absolute log file path and location

   Warning: only for onetime usage

   \return
   Absolute log file path and location

   \created     15.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_GetCompleteLogFileLocation(void)
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
   c_FileBaseName = c_FileBaseName.replace(19, 4, ".syde_log");
   //Final step as this step changes the format size
   c_FileBaseName = c_FileBaseName.replace(10, 1, "__");

   c_Retval = QString("%1/Logs/%2").arg(C_Uti::h_GetExePath()).arg(c_FileBaseName);

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get readable stw error

   \param[in] os32_Error STW error value

   \return
   STW error string

   \created     20.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_StwError(const sint32 os32_Error)
{
   const QString c_Retval = C_OSCLoggingHandler::h_StwError(os32_Error).c_str();

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Minimize full paths to fit in GUI edit fields.

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

   \param[in]     orc_Path       full file path
   \param[in]     orc_Font       font to calculate number of maximal possible characters
   \param[in]     ou32_Width     size in pixels for minimized file path to fit e.g. in line edit field
   \param[in]     ou32_Buffer    left and right margin for text e.g. in line edit field, if this parameter is not needed
                                 then set value to 0.

   \return
   minimized file path

   \created     04.07.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_MinimizePath(const QString & orc_Path, const QFont & orc_Font, const uint32 ou32_Width,
                              const uint32 ou32_Buffer)
{
   QString c_MinimizedPath = orc_Path; // result

   if (orc_Path != "")
   {
      QFileInfo c_FileInfo(orc_Path);

      QFontMetrics c_FontMetrics(orc_Font);
      QString c_Path = c_FileInfo.filePath(); // in case we have windows-style path
      // get drive name
      const QStorageInfo c_StorageInfo(c_FileInfo.absoluteDir());
      QString c_DriveName = c_StorageInfo.rootPath();

      // if path does not exist, drive name is empty -> try to extract drive name from string
      if (c_DriveName == "")
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

      const QString c_Placeholder = QString(QChar(0x2026)) + "/"; // with horizontal ellipsis "..."

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
      sn_CurrentWidth = c_FontMetrics.width(c_MinimizedPath);
      if (sn_CurrentWidth > static_cast<sintn>(u32_AvailableWidth))
      {
         // no, full path does not fit

         // does the drive and file name alone with placeholder string fit?
         // check length of drive and file name plus placeholder
         c_MinimizedPath = c_DriveName + c_Placeholder + c_FileName;
         sn_CurrentWidth = c_FontMetrics.width(c_MinimizedPath);
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

               sn_DelimPos = QString(c_DriveName + c_Placeholder).length();
               sn_CurrentWidth = c_FontMetrics.width(c_MinimizedPath);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Converts an IP address to readable string

   4 byte array to x.x.x.x

   \param[in]     orau8_IpAddress         IP address for conversion

   \return
   String with IP address

   \created     02.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_IpAddressToString(const uint8 (&orau8_IpAddress)[4])
{
   QString c_Result;

   c_Result = QString("%1.%2.%3.%4").
              arg(QString::number(orau8_IpAddress[0])).
              arg(QString::number(orau8_IpAddress[1])).
              arg(QString::number(orau8_IpAddress[2])).
              arg(QString::number(orau8_IpAddress[3]));

   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert font from point size into pixel size

   \param[in] orc_Font Font to convert

   \return
   Font with previous point size as new pixel size

   \created     27.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QFont C_Uti::h_GetFontPixel(const QFont & orc_Font)
{
   QFont c_Retval = orc_Font;

   c_Retval.setPixelSize(c_Retval.pointSize());
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert relative file path to absolute file path if necessary

   Warning: assuming orc_AbsoluteBaseDir is absolute

   \param[in] orc_AbsoluteBaseDir        Base path if relative
   \param[in] orc_RelativeOrAbsoluteFile Path which might be relative or absolute

   \return
   Always absolute dir if possible

   \created     09.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_ConcatPathIfNecessary(const QString & orc_AbsoluteBaseDir, const QString & orc_RelativeOrAbsoluteFile)
{
   QString c_Retval;

   if (QDir::isAbsolutePath(orc_RelativeOrAbsoluteFile) == false)
   {
      //Only use base if possible
      if (orc_AbsoluteBaseDir.isEmpty() == false)
      {
         QDir c_Dir(orc_AbsoluteBaseDir);
         c_Retval = QDir::cleanPath(c_Dir.absoluteFilePath(orc_RelativeOrAbsoluteFile));
      }
   }
   else
   {
      c_Retval = orc_RelativeOrAbsoluteFile;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to convert data block project path to absolute path

   Does nothing if the path is already absolute.
   If the path is relative it is meant to be relative to .syde file
   and therefore gets concatenated with this.
   If the resulting path is still relative (because path to .syde is relative)
   it is made absolute by Qt functionality which uses openSYDE.exe as base directory.

   \param[in]     orc_ProjectPath   Path that may be relative to .syde file

   \return
   absolute path to data block path

   \created     18.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
QString C_Uti::h_GetAbsoluteProjectPath(const QString & orc_SydeProjectPath, const QString & orc_ProgramProjectPath)
{
   QString c_ProjectPath;

   // project path relative -> relative to openSYDE project file (.syde)
   // project path empty -> default is openSYDE project file location
   if ((orc_ProgramProjectPath == "") || (QDir::isRelativePath(orc_ProgramProjectPath)))
   {
      c_ProjectPath = orc_SydeProjectPath + QDir::separator() + orc_ProgramProjectPath;
   }
   // path already absolute
   else
   {
      c_ProjectPath = orc_ProgramProjectPath;
   }

   const QDir c_ProjectDir(c_ProjectPath); // no cleanPath because absolute path also cleans up

   return c_ProjectDir.absolutePath();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert version in format 1.00.0 to STW format V1.00r0

   \param[in]     orc_Version    version in format 1.00.0 (e.g. APPLICATION_VERSION)

   \return
   version
      in format V1.00r0 if orc_Version is in format 1.00.0
      else orc_Version as it is

   \created     23.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_Uti::C_Uti(void)
{
}

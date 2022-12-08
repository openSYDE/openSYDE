//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       OpenSYDE utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_UTI_HPP
#define C_UTI_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QColor>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QStyle>

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_Uti
{
public:
   static void h_Uniqueify(std::vector<uint32_t> & orc_Indices);
   static std::vector<std::vector<uint32_t> > h_GetContiguousSectionsAscending(
      const std::vector<uint32_t> & orc_Indices);
   static bool h_CheckFloatHasNoFractionPart(const float64_t of64_Value);
   static int32_t h_GetNumberOfDecimals(const float64_t of64_Value);
   static QString h_GetStringFromDouble(const float64_t of64_Value);
   static std::vector<uint32_t> h_UniquifyAndSortAscending(const std::vector<uint32_t> & orc_Items);
   static std::vector<uint32_t> h_UniquifyAndSortDescending(const std::vector<uint32_t> & orc_Items);
   static QColor h_ScaleColor(const QColor & orc_Color, const uint32_t ou32_Percentage);
   static QColor h_GetDisabledColorLook(const QColor & orc_DefaultColorLook);
   static QString h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                      const int32_t os32_Width);
   static QString h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                      const float64_t of64_Width);
   static QString h_GetByteCountAsString(const uint32_t ou32_ByteCount);
   static bool h_CheckKeyModifier(const Qt::KeyboardModifiers & orc_ActiveKeyModifiers,
                                  const Qt::KeyboardModifier & ore_CheckKeyModifier);
   static bool h_CheckStyleState(const QStyle::State & orc_ActiveState, const QStyle::StateFlag & ore_CheckState);
   static QString h_GetExePath(void);
   static QString h_GetPemDbPath(void);
   static QString h_GetApplicationVersion(const bool oq_UseStwFormat = true);
   static QString h_CheckAndReplaceWithExePathIfNecessary(const QString & orc_Path);
   static QString h_GetLink(const QString & orc_DisplayedText,
                            const QColor & orc_Color = stw::opensyde_gui::mc_STYLE_GUIDE_COLOR_LINK,
                            const QString & orc_HyperLink = "...");
   static QString h_GetCompleteLogFileLocation(const QString & orc_Extension);
   static QString h_StwError(const int32_t os32_Error);
   static QString h_MinimizePath(const QString & orc_Path, const QFont & orc_Font, const uint32_t ou32_Width,
                                 const uint32_t ou32_Buffer);
   static QString h_IpAddressToString(const uint8_t (&orau8_IpAddress)[4]);
   static QFont h_GetFontPixel(const QFont & orc_Font);
   static QString h_ConvertVersionToStwStyle(const QString & orc_Version);
   static QString h_ConcatPathIfNecessary(const QString & orc_BaseDir, const QString & orc_RelativeOrAbsolutePath);
   static stw::scl::C_SclString h_GetUniqueName(const std::map<stw::scl::C_SclString, bool> & orc_ExistingStrings,
                                                const stw::scl::C_SclString & orc_ProposedName,
                                                const stw::scl::C_SclString & orc_SkipName = "");
   static QString h_GetUniqueNameQt(const std::map<stw::scl::C_SclString, bool> & orc_ExistingStrings,
                                    const QString & orc_ProposedName);
   static void h_GetNumberAtStringEnd(const stw::scl::C_SclString & orc_ProposedName,
                                      stw::scl::C_SclString & orc_CutString, int32_t & ors32_Number);
   static std::vector<int32_t> h_CreateAscendingIndexMap(const std::vector<uint32_t> & orc_UnsortedIndices);
   static bool h_CheckSortedAscending(const std::vector<uint32_t> & orc_Indices);
   template <typename T>
   static void h_SortIndicesAscendingAndSync(std::vector<uint32_t> & orc_IndicesTmp, std::vector<T> & orc_SyncContent);
   static QString h_GetAbsolutePathFromExe(const QString & orc_Path);
   static bool h_IsPathRelativeToDir(const QString & orc_PathIn, const QString & orc_AbsoluteReferenceDir,
                                     QString & orc_PathAbsolute, QString & orc_PathRelative);
   static QString h_ResolveProjIndependentPlaceholderVariables(const QString & orc_Path);
   static QString h_GetHashValueAsQtString(void);
   static QString h_GetValueAsHex(const uint64_t ou64_Value, const uint8_t ou8_FieldWidth = 0);
   static QString h_GetValueAsHex(const uint32_t ou32_Value, const uint8_t ou8_FieldWidth = 0);
   static void h_GetAllFilePathsInFolder(const QString & orc_FolderPath,
                                         std::vector<stw::scl::C_SclString> & orc_FilePaths);

private:
   C_Uti(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

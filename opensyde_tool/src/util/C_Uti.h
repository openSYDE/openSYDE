//-----------------------------------------------------------------------------
/*!
   \file
   \brief       OpenSYDE utility functions (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_UTI_H
#define C_UTI_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include <QColor>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QStyle>

#include "stwtypes.h"
#include "constants.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_Uti
{
public:
   static void h_Uniqueify(std::vector<stw_types::uint32> & orc_Indices);
   static std::vector<std::vector<stw_types::uint32> > h_GetContiguousSectionsAscending(
      const std::vector<stw_types::uint32> & orc_Indices);
   static bool h_CheckFloatHasNoFractionPart(const stw_types::float64 of64_Value);
   static stw_types::sint32 h_GetNumberOfDecimals(const stw_types::float64 of64_Value);
   static QString h_GetStringFromDouble(const stw_types::float64 of64_Value);
   static std::vector<stw_types::uint32> h_UniquifyAndSortAscending(const std::vector<stw_types::uint32> & orc_Items);
   static std::vector<stw_types::uint32> h_UniquifyAndSortDescending(const std::vector<stw_types::uint32> & orc_Items);
   static QColor h_ScaleColor(const QColor & orc_Color, const stw_types::uint32 ou32_Percentage);
   static QColor h_GetDisabledColorLook(const QColor & orc_DefaultColorLook);
   static QString h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                      const stw_types::sintn osn_Width);
   static QString h_AdaptStringToSize(const QString & orc_String, const QFontMetrics & orc_FontMetrics,
                                      const stw_types::float64 of64_Width);
   static QString h_GetByteCountAsString(const stw_types::uint32 ou32_ByteCount);
   static bool h_CheckKeyModifier(const Qt::KeyboardModifiers & orc_ActiveKeyModifiers,
                                  const Qt::KeyboardModifier & ore_CheckKeyModifier);
   static bool h_CheckStyleState(const QStyle::State & orc_ActiveState, const QStyle::StateFlag & ore_CheckState);
   static QString h_GetExePath(void);
   static QString h_CheckAndReplaceWithExePathIfNecessary(const QString & orc_Path);
   static QString h_GetLink(const QString & orc_DisplayedText,
                            const QColor & orc_Color = stw_opensyde_gui::mc_STYLE_GUIDE_COLOR_LINK,
                            const QString & orc_HyperLink = "...");
   static QString h_GetCompleteLogFileLocation(const QString & orc_Extension);
   static QString h_StwError(const stw_types::sint32 os32_Error);
   static QString h_MinimizePath(const QString & orc_Path, const QFont & orc_Font, const stw_types::uint32 ou32_Width,
                                 const stw_types::uint32 ou32_Buffer);
   static QString h_IpAddressToString(const stw_types::uint8 (&orau8_IpAddress)[4]);
   static QFont h_GetFontPixel(const QFont & orc_Font);
   static QString h_ConcatPathIfNecessary(const QString & orc_BaseDir,
                                          const QString & orc_RelativeOrAbsolutePath);
   static QString h_ConvertVersionToSTWStyle(const QString & orc_Version);
   static stw_scl::C_SCLString h_GetUniqueName(const std::map<stw_scl::C_SCLString, bool> & orc_ExistingStrings,
                                               const stw_scl::C_SCLString & orc_ProposedName);
   static QString h_GetUniqueNameQ(const std::map<stw_scl::C_SCLString, bool> & orc_ExistingStrings,
                                   const QString & orc_ProposedName);
   static void h_GetNumberAtStringEnd(const stw_scl::C_SCLString & orc_ProposedName,
                                      stw_scl::C_SCLString & orc_CutString, stw_types::sint32 & ors32_Number);
   static std::vector<stw_types::sint32> h_CreateAscendingIndexMap(
      const std::vector<stw_types::uint32> & orc_UnsortedIndices);
   static bool h_CheckSortedAscending(const std::vector<stw_types::uint32> & orc_Indices);
   template <typename T>
   static void h_SortIndicesAscendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                             std::vector<T> & orc_SyncContent);
   static QString h_GetAbsolutePathFromExe(const QString & orc_Path);
   static bool h_IsPathRelativeToDir(const QString & orc_PathIn, const QString & orc_AbsoluteReferenceDir,
                                     QString & orc_PathAbsolute, QString & orc_PathRelative);

private:
   C_Uti(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

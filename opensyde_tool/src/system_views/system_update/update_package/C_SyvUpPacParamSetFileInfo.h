//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Display class for parameter set file information and comparison (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPARAMSETFILEINFO_H
#define C_SYVUPPACPARAMSETFILEINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.h"
#include "C_PuiSvNodeUpdateParamInfo.h"
#include "C_OSCParamSetInterpretedData.h"
#include "C_SyvUpPacParamSetFileInfoComparisonDescription.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileInfo
{
public:
   C_SyvUpPacParamSetFileInfo(const QString & orc_Path, const QString & orc_StoragePath,
                              const stw_types::uint32 ou32_NodeIndex);

   stw_types::sint32 ReadFile(void);

   const stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & GetParamInfo(void) const;
   const QString & GetPath(void) const;
   const QString & GetComparisonResultsHtml(void) const;

private:
   const QString mc_Path;
   const QString mc_StoragePath;
   const stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCParamSetInterpretedData mc_InterpretedFileInfo;
   stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo mc_ParamInfo;
   QString mc_ComparisonHtml;
   std::vector<C_SyvUpPacParamSetFileInfoComparisonDescription> mc_ComparisonResults;
   static const QString mhc_START_HEADING_TD;
   static const QString mhc_CONTINUE_HEADING_TD;
   static const QString mhc_START_DATA_TD;
   static const QString mhc_CONTINUE_DATA_TD;

   void m_Comparison(const bool oq_OptionlContentMissing);
   void m_ConvertToHtmlString(const bool oq_OptionlContentMissing);
   void m_DisplayNoMatch(const QString & orc_Item, const QString & orc_Description,
                         const stw_types::uint32 ou32_LayerNum);
   void m_CompareString(const QString & orc_StrFile, const QString & orc_StrSD, const QString & orc_Description,
                        const stw_types::uint32 ou32_LayerNum);
   static QString mh_GetVersionString(const stw_types::uint32 ou32_VersionByte1,
                                      const stw_types::uint32 ou32_VersionByte2,
                                      const stw_types::uint32 ou32_VersionByte3);
   void m_AddTableRow(const QString & orc_Heading, const QString & orc_Content);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

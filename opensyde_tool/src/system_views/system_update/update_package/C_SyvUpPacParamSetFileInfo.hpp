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
#include "stwtypes.hpp"
#include "C_OscViewNodeUpdateParamInfo.hpp"
#include "C_OscParamSetInterpretedData.hpp"
#include "C_SyvUpPacParamSetFileInfoComparisonDescription.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileInfo
{
public:
   C_SyvUpPacParamSetFileInfo(const QString & orc_Path, const QString & orc_StoragePath, const uint32_t ou32_NodeIndex);

   int32_t ReadFile(void);

   const stw::opensyde_core::C_OscViewNodeUpdateParamInfo & GetParamInfo(void) const;
   const QString & GetPath(void) const;
   const QString & GetComparisonResultsHtml(void) const;

private:
   const QString mc_Path;
   const QString mc_StoragePath;
   const uint32_t mu32_NodeIndex;
   stw::opensyde_core::C_OscParamSetInterpretedData mc_InterpretedFileInfo;
   stw::opensyde_core::C_OscViewNodeUpdateParamInfo mc_ParamInfo;
   QString mc_ComparisonHtml;
   std::vector<C_SyvUpPacParamSetFileInfoComparisonDescription> mc_ComparisonResults;
   static const QString mhc_START_HEADING_TD;
   static const QString mhc_CONTINUE_HEADING_TD;
   static const QString mhc_START_DATA_TD;
   static const QString mhc_CONTINUE_DATA_TD;

   void m_Comparison(const bool oq_OptionlContentMissing);
   void m_ConvertToHtmlString(const bool oq_OptionlContentMissing);
   void m_DisplayNoMatch(const QString & orc_Item, const QString & orc_Description, const uint32_t ou32_LayerNum);
   void m_CompareString(const QString & orc_StrFile, const QString & orc_StrSd, const QString & orc_Description,
                        const uint32_t ou32_LayerNum);
   static QString mh_GetVersionString(const uint32_t ou32_VersionByte1, const uint32_t ou32_VersionByte2,
                                      const uint32_t ou32_VersionByte3);
   void m_AddTableRow(const QString & orc_Heading, const QString & orc_Content);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

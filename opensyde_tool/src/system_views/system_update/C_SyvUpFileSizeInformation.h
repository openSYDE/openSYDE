//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Wrapper class for all currently known file size information
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPFILESIZEINFORMATION_H
#define C_SYVUPFILESIZEINFORMATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include <QMap>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpFileSizeInformation
{
public:
   C_SyvUpFileSizeInformation(void);

   void Reset(void);
   stw_types::uint64 GetEstimatedTimeS(bool * const opq_Ok) const;
   stw_types::uint64 GetOverallFilesSize(void) const;
   void ReserveSpace(const stw_types::uint32 ou32_NumNodes);
   void AppendFiles(const stw_types::uint32 ou32_NodeIndex, const std::vector<stw_types::uint64> & orc_OtherFiles,
                    const std::vector<stw_types::uint64> & orc_ParamFiles);
   stw_types::uint32 GetNumParamFiles(void) const;
   stw_types::uint32 GetNumOtherFiles(void) const;
   stw_types::uint32 GetNumParamFilesForDevice(const stw_types::uint32 ou32_DeviceOrder) const;
   stw_types::uint32 GetNumOtherFilesForDevice(const stw_types::uint32 ou32_DeviceOrder) const;
   stw_types::uint64 GetParamFileSizeForDevice(const stw_types::uint32 ou32_DeviceOrder,
                                               const stw_types::uint32 ou32_FileIndex) const;
   stw_types::uint64 GetOtherFileSizeForDevice(const stw_types::uint32 ou32_DeviceOrder,
                                               const stw_types::uint32 ou32_FileIndex) const;
   void SetElapsedTimeForNode(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint64 ou64_ElapsedTimeMs);

   const QMap<stw_types::uint32, stw_types::float64> & GetBytesPerMsMapPerNode(void) const;

   void SetFileSizesByteMapPerNode(const QMap<stw_types::uint32, stw_types::float64> & orc_Value);

private:
   stw_types::uint64 mu64_OverallFilesSize;
   std::vector<std::vector<stw_types::uint64> > mc_FileSizesByte;
   std::vector<std::vector<stw_types::uint64> > mc_ParamFileSizesByte;
   QMap<stw_types::uint32, stw_types::float64> mc_FileSizesByteMapPerNode;
   QMap<stw_types::uint32, stw_types::float64> mc_BytesPerMsMapPerNode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

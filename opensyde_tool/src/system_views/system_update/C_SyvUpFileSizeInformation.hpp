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

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpFileSizeInformation
{
public:
   C_SyvUpFileSizeInformation(void);

   void Reset(void);
   uint64_t GetEstimatedTimeS(bool * const opq_Ok) const;
   uint64_t GetOverallFilesSize(void) const;
   void ReserveSpace(const uint32_t ou32_NumNodes);
   void AppendFiles(const uint32_t ou32_NodeIndex, const std::vector<uint64_t> & orc_OtherFiles,
                    const std::vector<uint64_t> & orc_ParamFiles);
   uint32_t GetNumParamFiles(void) const;
   uint32_t GetNumOtherFiles(void) const;
   uint32_t GetNumParamFilesForDevice(const uint32_t ou32_DeviceOrder) const;
   uint32_t GetNumOtherFilesForDevice(const uint32_t ou32_DeviceOrder) const;
   uint64_t GetParamFileSizeForDevice(const uint32_t ou32_DeviceOrder, const uint32_t ou32_FileIndex) const;
   uint64_t GetOtherFileSizeForDevice(const uint32_t ou32_DeviceOrder, const uint32_t ou32_FileIndex) const;
   void SetElapsedTimeForNode(const uint32_t ou32_NodeIndex, const uint64_t ou64_ElapsedTimeMs);

   //User settings
   void LoadUserSettings(const uint32_t ou32_ViewIndex);
   void SaveUserSettings(const uint32_t ou32_ViewIndex) const;

private:
   uint64_t mu64_OverallFilesSize;
   std::vector<std::vector<uint64_t> > mc_FileSizesByte;
   std::vector<std::vector<uint64_t> > mc_ParamFileSizesByte;
   QMap<uint32_t, float64_t> mc_FileSizesByteMapPerNode;
   QMap<uint32_t, float64_t> mc_BytesPerMsMapPerNode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

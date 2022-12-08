//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVNODEUPDATE_H
#define C_PUISVNODEUPDATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QString>
#include "stwtypes.hpp"
#include "C_PuiSvNodeUpdateParamInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvNodeUpdate
{
public:
   C_PuiSvNodeUpdate(void);

   enum E_GenericFileType
   {
      eFTP_DATA_BLOCK,
      eFTP_FILE_BASED
   };

   enum E_StateSecurity
   {
      eST_SEC_NO_CHANGE,
      eST_SEC_ACTIVATE,
      eST_SEC_DEACTIVATE
   };

   enum E_StateDebugger
   {
      eST_DEB_NO_CHANGE,
      eST_DEB_ACTIVATE,
      eST_DEB_DEACTIVATE
   };

   void CalcHash(uint32_t & oru32_HashValue) const;

   //Vectors
   void ClearParamPaths(void);
   void ClearPathsAsAppropriate(const E_GenericFileType oe_Type);
   const std::vector<C_PuiSvNodeUpdateParamInfo> & GetParamInfos(void) const;
   const std::vector<QString> & GetPaths(const E_GenericFileType oe_Type) const;
   const std::vector<bool> & GetSkipUpdateOfParamInfosFlags(void) const;
   const std::vector<bool> & GetSkipUpdateOfPathsFlags(const E_GenericFileType oe_Type) const;
   void SetParamInfos(const std::vector<C_PuiSvNodeUpdateParamInfo> & orc_Value);
   void SetPaths(const std::vector<QString> & orc_Value, const E_GenericFileType oe_Type);
   void SetSkipUpdateOfParamInfosFlags(const std::vector<bool> & orc_Value);
   void SetSkipUpdateOfPathsFlags(const std::vector<bool> & orc_Value, const E_GenericFileType oe_Type);
   int32_t SetParamInfoContent(const uint32_t ou32_Index, const QString & orc_FilePath,
                               const uint32_t ou32_LastKnownCrc);

   //Elements
   void AddPath(const QString & orc_Path, const E_GenericFileType oe_Type);
   void AddParamInfo(const C_PuiSvNodeUpdateParamInfo & orc_Value);
   int32_t SetPath(const uint32_t ou32_Index, const QString & orc_Value, const E_GenericFileType oe_Type);
   int32_t SetParamInfo(const uint32_t ou32_Index, const C_PuiSvNodeUpdateParamInfo & orc_Value);
   int32_t SetSkipUpdateOfPath(const uint32_t ou32_Index, const bool oq_SkipFile, const E_GenericFileType oe_Type);
   int32_t SetSkipUpdateOfParamInfo(const uint32_t ou32_Index, const bool oq_SkipFile);
   int32_t RemovePath(const uint32_t ou32_Index, const E_GenericFileType oe_Type);
   int32_t RemoveParamInfo(const uint32_t ou32_Index);

   // PEM file
   void SetPemFilePath(const QString & orc_Value);
   QString GetPemFilePath(void) const;
   void RemovePemFilePath(void);
   void SetSkipUpdateOfPemFile(const bool oq_Skip);
   bool GetSkipUpdateOfPemFile(void) const;

   //State elements
   void SetStates(const E_StateSecurity oe_StateSecurity, const E_StateDebugger oe_StateDebugger);
   void GetStates(E_StateSecurity & ore_StateSecurity, E_StateDebugger & ore_StateDebugger) const;

   void OnSyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationSourceIndex,
                                   const uint32_t ou32_ApplicationTargetIndex);
   void OnSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationResultPathSizeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                                                   const uint32_t ou32_OldSize, const uint32_t ou32_NewSize);

   uint32_t u32_NodeUpdatePosition; // Position of node in update sequence

private:
   std::vector<QString> mc_DataBlockPaths;
   std::vector<QString> mc_FileBasedPaths;
   std::vector<C_PuiSvNodeUpdateParamInfo> mc_ParamSetPaths;
   std::vector<std::vector<bool> > mc_SkipUpdateOfFiles; // Flags for skipping an update of the associated file
   // First level has three "layers":
   // 1: flags for datablock paths
   // 2: flags for file based paths
   // 3: flags for parameter set paths

   QString mc_PemFilePath;
   bool mq_SkipUpdateOfPemFile;

   E_StateSecurity me_StateSecurity;
   E_StateDebugger me_StateDebugger;

   static const int32_t mhs32_PARAMETER_SET_INDEX = 2;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

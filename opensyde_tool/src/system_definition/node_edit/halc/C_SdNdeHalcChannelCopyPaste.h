//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for HALC channel copy and paste.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELCOPYPASTE_H
#define C_SDNDEHALCCHANNELCOPYPASTE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QModelIndex>

#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelCopyPaste
{
public:
   static void h_Copy(const stw_types::uint32 ou32_NodeIndex, const QModelIndex & orc_CurrentChannel);
   static bool h_Paste(QWidget * const opc_Parent, const stw_types::uint32 ou32_NodeIndex,
                       const QModelIndexList & orc_TargetIndexes);

private:
   static bool mh_CheckDataFromClipboard(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_opensyde_core::C_OSCHalcConfigStandalone & orc_HalcConfig,
                                         QString & orc_ErrorDescription);
   static bool mh_CheckSelectedChannels(const stw_types::uint32 ou32_NodeIndex,
                                        const stw_opensyde_core::C_OSCHalcConfigStandalone & orc_HalcConfig,
                                        const QModelIndexList & orc_TargetIndexes, QString & orc_ErrorDescription);
   static bool mh_CheckSelectedChannel(const stw_types::uint32 ou32_NodeIndex,
                                       const stw_opensyde_core::C_OSCHalcConfigStandaloneDomain & orc_SourceDomain,
                                       const stw_opensyde_core::C_OSCHalcConfigChannel & orc_SourceChannel, const
                                       QModelIndex & orc_TargetIndex, QString & orc_InvalidChannels);
   static void mh_PasteToSelectedChannels(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_opensyde_core::C_OSCHalcConfigStandalone & orc_HalcConfig,
                                          const QModelIndexList & orc_TargetIndexes);
   static std::map<stw_scl::C_SCLString, bool> mh_GetExistingChannelNames(const stw_types::uint32 ou32_NodeIndex,
                                                                          const stw_types::uint32 ou32_DomainIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

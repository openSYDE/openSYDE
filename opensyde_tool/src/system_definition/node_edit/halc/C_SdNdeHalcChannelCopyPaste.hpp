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

#include "C_OscHalcConfigStandalone.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelCopyPaste
{
public:
   static void h_Copy(const uint32_t ou32_NodeIndex, const QModelIndex & orc_CurrentChannel);
   static bool h_Paste(QWidget * const opc_Parent, const uint32_t ou32_NodeIndex,
                       const QModelIndexList & orc_TargetIndexes);

private:
   static bool mh_CheckDataFromClipboard(const uint32_t ou32_NodeIndex,
                                         const stw::opensyde_core::C_OscHalcConfigStandalone & orc_HalcConfig,
                                         QString & orc_ErrorDescription);
   static bool  mh_CheckSelectedChannels(const uint32_t ou32_NodeIndex,
                                         const stw::opensyde_core::C_OscHalcConfigStandalone & orc_HalcConfig,
                                         const QModelIndexList & orc_TargetIndexes, bool & orq_ChannelCase,
                                         QString & orc_ErrorDescription, bool & orq_LinkedChange);
   static bool mh_CheckSelectedChannel(const uint32_t ou32_NodeIndex,
                                       const stw::opensyde_core::C_OscHalcConfigStandaloneDomain & orc_SourceDomain,
                                       const stw::opensyde_core::C_OscHalcConfigChannel & orc_SourceChannel, const
                                       QModelIndex & orc_TargetIndex, QString & orc_InvalidChannels,
                                       bool & orq_LinkedChange);
   static bool mh_CheckSelectedDomain(const uint32_t ou32_NodeIndex,
                                      const stw::opensyde_core::C_OscHalcConfigStandaloneDomain & orc_SourceDomain,
                                      const QModelIndex & orc_TargetIndex, QString & orc_InvalidDomain);
   static void mh_PasteToSelectedChannels(const uint32_t ou32_NodeIndex,
                                          const stw::opensyde_core::C_OscHalcConfigStandalone & orc_HalcConfig,
                                          const QModelIndexList & orc_TargetIndexes);
   static void mh_PasteToSelectedDomain(const uint32_t ou32_NodeIndex,
                                        const stw::opensyde_core::C_OscHalcConfigStandalone & orc_HalcConfig,
                                        const QModelIndexList & orc_TargetIndexes);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view update specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GINODESYVUPDATE_HPP
#define C_GINODESYVUPDATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_GiInfo.hpp"
#include "C_SyvUtil.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_GiSvNodeSyvBase.hpp"
#include "C_GiSvNodeData.hpp"
#include "C_SyvUpNodePropertiesDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeSyvUpdate :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   C_GiSvNodeSyvUpdate(const uint32_t ou32_ViewIndex, const int32_t & ors32_NodeIndex, const uint64_t & oru64_Id,
                       const float64_t & orf64_Width, const float64_t & orf64_Height,
                       QGraphicsItem * const opc_Parent = NULL);
   ~C_GiSvNodeSyvUpdate() override;

   void SetViewConnected(const bool oq_Connected) override;
   void SetConnecting(const bool oq_Active);
   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted, const uint32_t ou32_FailedApplicationIndex,
                                const uint32_t ou32_NodeIndex);
   void SetNodeError(const uint32_t ou32_NodeIndex);
   void SetNodeConnectStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SetNodeUpdateStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeUpdateStates> & orc_NodeStates);
   void ShowInfo(void);
   void UpdateInitialPackageStatus(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                   const uint32_t ou32_NodeIndex);
   bool HasNoResponseAndIsActive(void) const;
   std::vector<uint32_t> GetAllActiveStwDeviceIndices(void) const;
   bool IsActiveInView(void) const;
   bool HasNodeAnAvailableFlashloader(void) const;
   void UpdateIcons(void);

   void GenerateHint(void) override;
   void UpdateTransform(const QTransform & orc_Transform) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDiscardInfo(const uint32_t ou32_NodeIndex);

private:
   //Avoid call
   C_GiSvNodeSyvUpdate(const C_GiSvNodeSyvUpdate &);
   C_GiSvNodeSyvUpdate & operator =(const C_GiSvNodeSyvUpdate &) &; //lint !e1511 //we want to hide the base func.

   void m_InitIcons(void);
   void m_RefreshDialog(void);

   C_GiInfo * mpc_IconTopLeft;
   C_GiInfo * mpc_IconBottom;
   C_SyvUpNodePropertiesDialog * mpc_InfoDialog;

   C_GiSvNodeData mc_NodeData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

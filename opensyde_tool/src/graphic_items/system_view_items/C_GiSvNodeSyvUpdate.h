//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view update specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GINODESYVUPDATE_H
#define C_GINODESYVUPDATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_GiInfo.h"
#include "C_SyvUtil.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_GiSvNodeSyvBase.h"
#include "C_GiSvNodeData.h"
#include "C_SyvUpNodePropertiesDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeSyvUpdate :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   C_GiSvNodeSyvUpdate(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_NodeIndex,
                       const stw_types::uint64 & oru64_ID, const stw_types::float64 & orf64_Width,
                       const stw_types::float64 & orf64_Height, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvNodeSyvUpdate();

   virtual void SetViewConnected(const bool oq_Connected) override;
   void SetConnecting(const bool oq_Active);
   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const stw_types::uint32 ou32_FailedApplicationIndex,
                                const stw_types::uint32 ou32_NodeIndex);
   void SetNodeError(const stw_types::uint32 ou32_NodeIndex);
   void SetNodeConnectStates(const std::vector<stw_opensyde_core::C_OSCSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SetNodeUpdateStates(const std::vector<stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates> & orc_NodeStates);
   void ShowInfo(void);
   void UpdateInitialPackageStatus(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos,
                                   const stw_types::uint32 ou32_NodeIndex);
   bool HasNoResponseAndIsActive(void) const;
   std::vector<stw_types::uint32> GetAllNotRespondingAndActiveIndices(void) const;
   std::vector<stw_types::uint32> GetAllActiveSTWDeviceIndices(void) const;
   bool IsActiveInView(void) const;
   bool HasNodeAnAvailableFlashloader(void) const;
   void UpdateIcons(void);

   virtual void GenerateHint(void) override;
   virtual void UpdateTransform(const QTransform & orc_Transform) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDiscardInfo(const stw_types::uint32 ou32_NodeIndex);

private:
   //Avoid call
   C_GiSvNodeSyvUpdate(const C_GiSvNodeSyvUpdate &);
   C_GiSvNodeSyvUpdate & operator =(const C_GiSvNodeSyvUpdate &); //lint !e1511 //we want to hide the base func.

   void m_InitIcons(void);
   void m_RefreshDialog(void);

   C_GiInfo * mpc_IconTopLeft;
   C_GiInfo * mpc_IconBottom;
   C_SyvUpNodePropertiesDialog * mpc_InfoDialog;

   C_GiSvNodeData mc_NodeData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

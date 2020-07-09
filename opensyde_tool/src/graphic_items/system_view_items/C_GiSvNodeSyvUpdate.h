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

#include "C_SyvUtil.h"
#include "C_GiInfo.h"
#include "C_OsyHexFile.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_GiSvNodeSyvBase.h"
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
   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const stw_types::uint32 ou32_FailedApplicationIndex);
   void CheckThirdParty(void);
   void SetNoResponse(void);
   void ShowInfo(void);
   void UpdateInitialPackageStatus(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos);
   bool HasNoResponseAndIsActive(void) const;
   bool IsActiveInView(void) const;
   bool IsStwDevice(void) const;
   void UpdateIcons(void);

   virtual void GenerateHint(void) override;
   virtual void UpdateTransform(const QTransform & orc_Transform) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDiscardInfo(const stw_types::uint32 ou32_NodeIndex);

private:
   enum E_UpdateStatus
   {
      eU_UPDATING,
      eU_WAITING,
      eU_UP_TO_DATE,
      eU_UPDATE_SUCCESS,
      eU_UPDATE_DISABLED,
      eU_UNKNOWN
   };

   //Avoid call
   C_GiSvNodeSyvUpdate(const C_GiSvNodeSyvUpdate &);
   C_GiSvNodeSyvUpdate & operator =(const C_GiSvNodeSyvUpdate &);

   void m_InitIcons(void);
   bool m_CheckAlwaysUpdate(void) const;
   bool m_CheckUpdateDisabledState(QString * const opc_TooltipText = NULL) const;
   void m_RefreshDialog(void);

   bool mq_UpdateConnected;
   bool mq_UpdateInProgress;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus me_InitialStatus;
   E_UpdateStatus me_UpdateStatus;
   bool mq_UpdateFailed;
   bool mq_UpdateSuccess;
   bool mq_ValidStatus;
   bool mq_Discarded;
   stw_types::uint32 mu32_FailedApplicationIndex;
   std::vector<stw_diag_lib::C_XFLECUInformation> mc_HexFileInfos;
   std::vector<QString> mc_ParamFileInfos;
   std::vector<QString> mc_FileInfos;
   std::vector<bool> mc_HexAppInfoAmiguous;
   const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * mpc_STWDevice;
   const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * mpc_OSYDevice;
   C_GiInfo * mpc_IconTopLeft;
   C_GiInfo * mpc_IconBottom;
   C_SyvUpNodePropertiesDialog * mpc_InfoDialog;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

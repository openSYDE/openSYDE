//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view update specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GINODESYVUPDATE_H
#define C_GINODESYVUPDATE_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include "C_SyvUtil.h"
#include "C_GiInfo.h"
#include "C_OsyHexFile.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_GiSvNodeSyvBase.h"
#include "C_SyvUpNodePropertiesDialog.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvNodeSyvUpdate :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   C_GiSvNodeSyvUpdate(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_NodeIndex,
                       const stw_types::uint64 & oru64_ID, const stw_types::float64 & orf64_Width,
                       const stw_types::float64 & orf64_Height, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvNodeSyvUpdate();

   virtual void SetViewConnected(const bool oq_Connected);
   void SetConnected(const bool oq_Active);
   void SetUpdating(const bool oq_Active);
   void SetNodeUpdateInProgress(const bool oq_Active, const bool oq_Aborted,
                                const stw_types::uint32 ou32_FailedApplicationIndex);
   void CheckThirdParty(void);
   void SetNoResponse(void);
   void ShowInfo(void);
   void UpdateInitialPackageStatus(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceApplicationInfos);
   bool HasNoResponseAndIsActive(void) const;
   bool IsStwDeviceAndActive(void) const;

   virtual void GenerateHint(void) override;

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
   void m_UpdateIcons(void);

   bool mq_UpdateConnected;
   bool mq_UpdateInProgress;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus me_InitialStatus;
   E_UpdateStatus me_UpdateStatus;
   bool mq_UpdateFailed;
   stw_types::uint32 mu32_FailedApplicationIndex;
   std::vector<stw_diag_lib::C_XFLECUInformation> mc_FileInfos;
   const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * mpc_STWDevice;
   const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * mpc_OSYDevice;
   C_GiInfo * mpc_IconTopLeft;
   C_GiInfo * mpc_IconBottom;
   C_SyvUpNodePropertiesDialog * mpc_InfoDialog;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif

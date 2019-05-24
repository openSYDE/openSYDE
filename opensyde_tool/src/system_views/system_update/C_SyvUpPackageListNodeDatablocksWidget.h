//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling datablock files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEDATABLOCKSWIDGET_H
#define C_SYVUPPACKAGELISTNODEDATABLOCKSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeDatablocksWidget :
   public C_SyvUpUpdatePackageListNodeWidget
{
public:
   C_SyvUpPackageListNodeDatablocksWidget(QWidget * const opc_Parent = NULL);

   virtual void AdaptFile(const QString & orc_File, C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;
   virtual void RevertFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;

   virtual void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation) const
   override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const override;
   virtual stw_types::uint32 Type(void) const override;

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   virtual void m_InitStaticNames(void) const override;

private:
   //Avoid call
   C_SyvUpPackageListNodeDatablocksWidget(const C_SyvUpPackageListNodeDatablocksWidget &);
   C_SyvUpPackageListNodeDatablocksWidget & operator =(const C_SyvUpPackageListNodeDatablocksWidget &);

   static const QString mhc_RemovePathText;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

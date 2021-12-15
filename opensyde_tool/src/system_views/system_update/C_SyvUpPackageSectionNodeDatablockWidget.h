//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling datablock files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGESECTIONNODEDATABLOCKWIDGET_H
#define C_SYVUPPACKAGESECTIONNODEDATABLOCKWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPackageSectionNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageSectionNodeDatablockWidget :
   public C_SyvUpPackageSectionNodeWidget
{
public:
   C_SyvUpPackageSectionNodeDatablockWidget(QWidget * const opc_Parent = NULL);

   virtual void AdaptFile(const QString & orc_File, C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void RevertFile(C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpPackageListNodeItemWidget * const opc_App) override;

   virtual void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation)
   override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          QStringList * const opc_RelevantPemFilePaths,
                          C_SyvUpPackageListNodeItemWidget ** const oppc_App) const override;
   virtual stw_types::uint32 Type(void) const override;

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   virtual void m_InitStaticNames(void) override;
   virtual void m_UpdateTitle(void) override;

private:
   //Avoid call
   C_SyvUpPackageSectionNodeDatablockWidget(const C_SyvUpPackageSectionNodeDatablockWidget &);
   C_SyvUpPackageSectionNodeDatablockWidget & operator =(const C_SyvUpPackageSectionNodeDatablockWidget &);

   stw_opensyde_core::C_OSCNodeApplication::E_Type me_Type;

   static const QString mhc_RemovePathText;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

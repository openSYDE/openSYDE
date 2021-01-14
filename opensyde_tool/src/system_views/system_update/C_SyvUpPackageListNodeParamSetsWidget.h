//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling paramset files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEPARAMSETWIDGET_H
#define C_SYVUPPACKAGELISTNODEPARAMSETWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_PuiSvNodeUpdateParamInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeParamSetsWidget :
   public C_SyvUpUpdatePackageListNodeWidget
{
public:
   C_SyvUpPackageListNodeParamSetsWidget(QWidget * const opc_Parent = NULL);

   virtual void SetUpdateFinished(void) const override;

   virtual void AddFile(const QString & orc_File) override;
   virtual void AdaptFile(const QString & orc_File, C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const override;

   virtual stw_types::sint32 GetUpdatePackage(stw_opensyde_core::C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                              stw_opensyde_core::C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                              stw_types::uint32 & oru32_FilesUpdated) const override;
   virtual stw_types::uint32 Type(void) const override;

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   virtual void m_InitStaticNames(void) const override;

private:
   //Avoid call
   C_SyvUpPackageListNodeParamSetsWidget(const C_SyvUpPackageListNodeParamSetsWidget &);
   C_SyvUpPackageListNodeParamSetsWidget & operator =(const C_SyvUpPackageListNodeParamSetsWidget &);

   stw_types::sint32 m_GetParamsetFileInfo(const QString & orc_File,
                                           stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & orc_ParamFileInfo);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

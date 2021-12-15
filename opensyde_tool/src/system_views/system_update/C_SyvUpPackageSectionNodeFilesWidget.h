//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling any files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGESECTIONNODEFILESWIDGET_H
#define C_SYVUPPACKAGESECTIONNODEFILESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPackageSectionNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageSectionNodeFilesWidget :
   public C_SyvUpPackageSectionNodeWidget
{
public:
   C_SyvUpPackageSectionNodeFilesWidget(QWidget * const opc_Parent = NULL);

   virtual void AddFile(const QString & orc_File);
   virtual void AdaptFile(const QString & orc_File, C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpPackageListNodeItemWidget * const opc_App) override;
   virtual void OpenPemFileSettings(C_SyvUpPackageListNodeItemWidget * const opc_App) override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          QStringList * const opc_RelevantPemFilePaths,
                          C_SyvUpPackageListNodeItemWidget ** const oppc_App) const override;
   virtual stw_types::uint32 Type(void) const override;

   void SetCountSkippedParamSetFiles(const stw_types::uint32 ou32_CountSkippedFiles);

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   virtual void m_InitStaticNames(void) override;
   virtual void m_UpdateTitle(void) override;
   virtual stw_types::uint32 m_AdaptParamSetNumber(const stw_types::uint32 ou32_Number) override;

private:
   //Avoid call
   C_SyvUpPackageSectionNodeFilesWidget(const C_SyvUpPackageSectionNodeFilesWidget &);
   C_SyvUpPackageSectionNodeFilesWidget & operator =(const C_SyvUpPackageSectionNodeFilesWidget &);

   stw_types::uint32 mu32_CountSkippedParamSetFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling any files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECTIONNODEFILESWIDGET_H
#define C_SYVUPPACSECTIONNODEFILESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacSectionNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSectionNodeFilesWidget :
   public C_SyvUpPacSectionNodeWidget
{
public:
   C_SyvUpPacSectionNodeFilesWidget(QWidget * const opc_Parent = NULL);

   virtual void AddFile(const QString & orc_File);
   virtual void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void OpenPemFileSettings(C_SyvUpPacListNodeItemWidget * const opc_App) override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          QStringList * const opc_RelevantPemFilePaths,
                          C_SyvUpPacListNodeItemWidget ** const oppc_App) const override;
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
   C_SyvUpPacSectionNodeFilesWidget(const C_SyvUpPacSectionNodeFilesWidget &);
   C_SyvUpPacSectionNodeFilesWidget & operator =(const C_SyvUpPacSectionNodeFilesWidget &);

   stw_types::uint32 mu32_CountSkippedParamSetFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

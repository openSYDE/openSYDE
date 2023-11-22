//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling any files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECTIONNODEFILESWIDGET_HPP
#define C_SYVUPPACSECTIONNODEFILESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacSectionNodeWidget.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSectionNodeFilesWidget :
   public C_SyvUpPacSectionNodeWidget
{
public:
   C_SyvUpPacSectionNodeFilesWidget(QWidget * const opc_Parent = NULL);

   virtual void AddFile(const QString & orc_File);
   void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void OpenPemFileSettings(C_SyvUpPacListNodeItemWidget * const opc_App) override;

   void PrepareExportConfig(stw::opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const
   override;
   void LoadImportConfig(const stw::opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) override;

   bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                  QStringList * const opc_RelevantParamSetImagePaths, QStringList * const opc_RelevantPemFilePaths,
                  C_SyvUpPacListNodeItemWidget ** const oppc_App) const override;
   uint32_t Type(void) const override;

   void SetCountSkippedParamSetFiles(const uint32_t ou32_CountSkippedFiles);

protected:
   void m_InitSpecificItem(const stw::opensyde_core::C_OscNode & orc_Node,
                           const stw::opensyde_core::C_OscViewNodeUpdate & orc_UpdateInfo) override;
   void m_InitStaticNames(void) override;
   void m_UpdateTitle(void) override;
   uint32_t m_AdaptParamSetNumber(const uint32_t ou32_Number) override;

private:
   //Avoid call
   C_SyvUpPacSectionNodeFilesWidget(const C_SyvUpPacSectionNodeFilesWidget &);
   C_SyvUpPacSectionNodeFilesWidget & operator =(const C_SyvUpPacSectionNodeFilesWidget &) &;

   uint32_t mu32_CountSkippedParamSetFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

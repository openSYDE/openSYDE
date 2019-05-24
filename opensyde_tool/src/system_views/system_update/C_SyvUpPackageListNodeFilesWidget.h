//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling any files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEFILESWIDGET_H
#define C_SYVUPPACKAGELISTNODEFILESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpUpdatePackageListNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeFilesWidget :
   public C_SyvUpUpdatePackageListNodeWidget
{
public:
   C_SyvUpPackageListNodeFilesWidget(QWidget * const opc_Parent = NULL);

   virtual void AddFile(const QString & orc_File) override;
   virtual void AdaptFile(const QString & orc_File, C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App) override;

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
   C_SyvUpPackageListNodeFilesWidget(const C_SyvUpPackageListNodeFilesWidget &);
   C_SyvUpPackageListNodeFilesWidget & operator =(const C_SyvUpPackageListNodeFilesWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

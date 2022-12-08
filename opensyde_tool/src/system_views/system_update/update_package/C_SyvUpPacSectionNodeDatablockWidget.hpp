//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for listing and handling datablock files
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECTIONNODEDATABLOCKWIDGET_H
#define C_SYVUPPACSECTIONNODEDATABLOCKWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvUpPacSectionNodeWidget.hpp"
#include "C_PuiSvNodeUpdate.hpp"
#include "C_SyvUpDeviceInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSectionNodeDatablockWidget :
   public C_SyvUpPacSectionNodeWidget
{
public:
   C_SyvUpPacSectionNodeDatablockWidget(QWidget * const opc_Parent = NULL);

   void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;
   void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;

   void UpdateDeviceInformation(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation)
   override;

   void PrepareExportConfig(stw::opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const
   override;
   void LoadImportConfig(const stw::opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) override;

   bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                  QStringList * const opc_RelevantParamSetImagePaths, QStringList * const opc_RelevantPemFilePaths,
                  C_SyvUpPacListNodeItemWidget ** const oppc_App) const override;
   uint32_t Type(void) const override;

protected:
   void m_InitSpecificItem(const stw::opensyde_core::C_OscNode & orc_Node,
                           const stw::opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   void m_InitStaticNames(void) override;
   void m_UpdateTitle(void) override;

private:
   //Avoid call
   C_SyvUpPacSectionNodeDatablockWidget(const C_SyvUpPacSectionNodeDatablockWidget &);
   C_SyvUpPacSectionNodeDatablockWidget & operator =(const C_SyvUpPacSectionNodeDatablockWidget &) &;

   stw::opensyde_core::C_OscNodeApplication::E_Type me_Type;

   static const QString mhc_REMOVE_PATH_TEXT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

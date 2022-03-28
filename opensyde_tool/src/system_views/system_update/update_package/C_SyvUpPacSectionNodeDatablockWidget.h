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
#include "C_SyvUpPacSectionNodeWidget.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSectionNodeDatablockWidget :
   public C_SyvUpPacSectionNodeWidget
{
public:
   C_SyvUpPacSectionNodeDatablockWidget(QWidget * const opc_Parent = NULL);

   virtual void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;
   virtual void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) override;

   virtual void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation)
   override;

   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const
   override;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) override;

   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          QStringList * const opc_RelevantPemFilePaths,
                          C_SyvUpPacListNodeItemWidget ** const oppc_App) const override;
   virtual stw_types::uint32 Type(void) const override;

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) override;
   virtual void m_InitStaticNames(void) override;
   virtual void m_UpdateTitle(void) override;

private:
   //Avoid call
   C_SyvUpPacSectionNodeDatablockWidget(const C_SyvUpPacSectionNodeDatablockWidget &);
   C_SyvUpPacSectionNodeDatablockWidget & operator =(const C_SyvUpPacSectionNodeDatablockWidget &);

   stw_opensyde_core::C_OSCNodeApplication::E_Type me_Type;

   static const QString mhc_REMOVE_PATH_TEXT;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

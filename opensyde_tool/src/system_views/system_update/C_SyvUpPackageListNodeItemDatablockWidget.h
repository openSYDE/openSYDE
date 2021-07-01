//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific file associated to concrete datablock as part of a node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGELISTNODEITEMDATABLOCKWIDGET_H
#define C_SYVUPPACKAGELISTNODEITEMDATABLOCKWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "C_SyvUpPackageListNodeItemWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageListNodeItemDatablockWidget :
   public C_SyvUpPackageListNodeItemWidget
{
public:
   C_SyvUpPackageListNodeItemDatablockWidget(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_NodeIndex, const QString & orc_DeviceName,
                                             const bool oq_FileBased, const bool oq_StwFlashloader,
                                             QWidget * const opc_Parent = NULL);

   virtual stw_types::uint32 GetType(void) const override;
   virtual bool IsFileIdentical(const QString & orc_AppName, const QString & orc_AppVersion,
                                const QString & orc_AppBuildTime, const QString & orc_AppBuildDate) const override;
   virtual void ViewFileInfo(void) override;
   virtual bool IsViewFileInfoPossible(void) const override;

protected:
   virtual void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning,
                                      bool & orq_TriggerRemove) override;
   virtual QString m_CreateToolTipContent(void) const override;

private:
   //Avoid call
   C_SyvUpPackageListNodeItemDatablockWidget(const C_SyvUpPackageListNodeItemDatablockWidget &);
   C_SyvUpPackageListNodeItemDatablockWidget & operator =(const C_SyvUpPackageListNodeItemDatablockWidget &);

   QString mc_ProjectName;
   QString mc_Version;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

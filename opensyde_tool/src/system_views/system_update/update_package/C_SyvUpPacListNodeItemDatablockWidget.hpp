//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific file associated to concrete datablock as part of a node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMDATABLOCKWIDGET_HPP
#define C_SYVUPPACLISTNODEITEMDATABLOCKWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "C_SyvUpPacListNodeItemWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemDatablockWidget :
   public C_SyvUpPacListNodeItemWidget
{
public:
   C_SyvUpPacListNodeItemDatablockWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                         const QString & orc_DeviceName, const bool oq_FileBased,
                                         const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);

   uint32_t GetType(void) const override;
   bool IsFileIdentical(const QString & orc_AppName, const QString & orc_AppVersion, const QString & orc_AppBuildTime,
                        const QString & orc_AppBuildDate) const override;
   void ViewFileInfo(void) override;
   bool IsViewFileInfoPossible(void) const override;

protected:
   void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove) override;
   QString m_CreateToolTipContent(void) const override;

private:
   //Avoid call
   C_SyvUpPacListNodeItemDatablockWidget(const C_SyvUpPacListNodeItemDatablockWidget &);
   C_SyvUpPacListNodeItemDatablockWidget & operator =(const C_SyvUpPacListNodeItemDatablockWidget &) &;

   QString mc_ProjectName;
   QString mc_Version;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif

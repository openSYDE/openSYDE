//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Database list item (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASEITEMWIDGET_H
#define C_CAMMOSDATABASEITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDateTime>
#include <QFileInfo>

#include "C_OgeWiOnlyBackground.h"
#include "C_CamProDatabaseData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDatabaseItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   enum E_LoadingState
   {
      eOK = 0,
      eNOT_FOUND,
      eLOADING,
      eQUEUE,
      eLOAD_ERROR,
      eWRONG_FORMAT
   };

   explicit C_CamMosDatabaseItemWidget(const stw_opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
                                       QWidget * const opc_Parent = NULL);
   ~C_CamMosDatabaseItemWidget(void);

   void SetState(const E_LoadingState oe_State);
   void SetBusIndex(const stw_types::uint32 ou32_BusIndex);
   void SetBusses(const std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses);
   void UpdateTooltip(void) const;
   void UpdateDatabasePathRelativeness(const QString & orc_NewDatabaseName);
   void CheckFile(void);
   stw_opensyde_gui_logic::C_CamProDatabaseData GetDatabaseData(void) const;

   static QString h_BrowseForDatabasePath(QWidget * const opc_Parent = NULL);
   static QString h_AdaptPathToSystemDefinitionIfNecessary(const QFileInfo & orc_SydeFile);
   static bool h_IsDatabaseAlreadyUsed(const QString & orc_AbsolutePath, QWidget * const opc_Parent = NULL);
   static void h_AppendMessageWarningIfNecessary(const QString & orc_File, QString & orc_Description,
                                                 QString & orc_Details);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigRemoveDatabase(C_CamMosDatabaseItemWidget * const opc_Sender, const bool oq_AskUser);
   bool SigEnableDatabase(const C_CamMosDatabaseItemWidget * const opc_Sender, const bool oq_Select);
   void SigUpdateDatabasePath(C_CamMosDatabaseItemWidget * const opc_Sender,
                              const stw_opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
                              const bool oq_IsUpdate);
   void SigUpdateDatabaseOsySysdefBus(C_CamMosDatabaseItemWidget * const opc_Sender,
                                      const stw_types::sint32 os32_BusIndex);
   void SigNotifyMissingDataBase(const QString & orc_Path);

private:
   Ui::C_CamMosDatabaseItemWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_CamProDatabaseData mc_Database;
   std::vector<stw_opensyde_core::C_OSCSystemBus> mc_Busses;

   E_LoadingState me_State;
   bool mq_AlreadyAskedUserReload;
   bool mq_AlreadyAskedUserDelete;
   QDateTime mc_FileTimeStamp;
   bool mq_ButtonPressed;

   //Avoid call
   C_CamMosDatabaseItemWidget(const C_CamMosDatabaseItemWidget &);
   C_CamMosDatabaseItemWidget & operator =(const C_CamMosDatabaseItemWidget &);

   void m_OnSelectBus(void);
   void m_OnBrowse(void);
   void m_OnRemove(void);
   void m_OnChxToggle(const bool & orq_Checked);
   void m_SetMinimizedPath(void) const;
   void m_EnableCheckBoxTooltip(const bool oq_Enabled);
   void m_ButtonPressed(void);
   void m_ButtonReleased(void);

protected:
   virtual bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif

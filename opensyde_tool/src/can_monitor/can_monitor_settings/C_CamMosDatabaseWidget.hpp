//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring database files like DBC or syde_sysdef files.
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASEWIDGET_H
#define C_CAMMOSDATABASEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>

#include "C_OgeWiOnlyBackground.hpp"
#include "C_CamMosDatabaseItemWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDatabaseWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosDatabaseWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosDatabaseWidget(void) override;

   void LoadUserSettings(void) const;
   void OnLoadFinishedDbc(const int32_t os32_Result);
   void OnLoadFinishedOsySysDef(const int32_t os32_Result,
                                const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses);
   void OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition, const int32_t os32_Result);
   void OnCommunicationStarted(const bool oq_Online);
   void PrepareForExpanded(const bool oq_Expand) const;
   void OnSigSavedAsNew(void) const;
   void AddDroppedDatabase(const QString & orc_Path);
   void Clear(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigAddDatabaseDbc(const QString & orc_Path, const QString & orc_OrgPath);
   void SigAddDatabaseOsySysDef(const QString & orc_Path, const QString & orc_OrgPath, const uint32_t ou32_BusIndex);
   void SigRemoveDatabase(const QString & orc_Path, const QString & orc_OrgPath, const bool oq_IsUpdate);
   void SigActivateDatabase(const QString & orc_Path, const QString & orc_OrgPath, const bool oq_Active);
   void SigSetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                                   const uint32_t ou32_BusIndex);
   void SigHide(void);
   void SigNotifyMissingDataBase(const QString & orc_Path);

private:
   Ui::C_CamMosDatabaseWidget * mpc_Ui;
   std::vector<C_CamMosDatabaseItemWidget *> mc_Entries;
   QList<C_CamMosDatabaseItemWidget *> mc_DatabasesToLoad;
   QTimer mc_Timer;

   //Avoid call
   C_CamMosDatabaseWidget(const C_CamMosDatabaseWidget &);
   C_CamMosDatabaseWidget & operator =(const C_CamMosDatabaseWidget &) &;

   void m_InitUi(void);
   void m_LoadConfig(void);
   void m_RemoveDatabaseWidget(C_CamMosDatabaseItemWidget * const opc_ItemWidget) const;
   void m_OnAddClicked(void);
   void m_AddDatabaseWidget(const stw::opensyde_gui_logic::C_CamProDatabaseData & orc_DatabaseData);
   void m_RemoveDatabase(C_CamMosDatabaseItemWidget * const opc_ItemWidget, const bool oq_AskUser);
   bool m_ActivateDatabase(const C_CamMosDatabaseItemWidget * const opc_ItemWidget, const bool & orq_Enable);
   void m_UpdateDatabasePath(C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                             const stw::opensyde_gui_logic::C_CamProDatabaseData & orc_Database,
                             const bool oq_IsUpdate);
   void m_UpdateDatabaseOsySysDefBus(const C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                                     const int32_t os32_BusIndex);
   int32_t m_GetIndexFromWidget(const C_CamMosDatabaseItemWidget * const opc_ItemWidget);
   void m_AddToLoadingQueue(C_CamMosDatabaseItemWidget * const opc_ItemWidget);
   void m_CheckAndLoadDatabase(void);
   void m_LoadFinishedGeneric(const int32_t os32_ErrorCode);
   void m_OnFileCheckTimeout(void);
   void m_StartCheckTimer(const bool oq_Active);
   void m_OnExpand(const bool oq_Expand) const;
   void m_UpdateTitleDatabaseCount(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
